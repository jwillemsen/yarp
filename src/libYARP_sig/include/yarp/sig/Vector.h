/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_VECTOR_H
#define YARP_SIG_VECTOR_H

#include <cstring>
#include <cstddef> //defines size_t
#include <yarp/os/Portable.h>
#include <string>
#include <yarp/os/ManagedBytes.h>

#include <yarp/sig/api.h>
#include <yarp/os/Log.h>

/**
* \file Vector.h contains the definition of a Vector type
*/
namespace yarp {

    namespace sig {
        class VectorBase;
        template<class T> class VectorOf;
        typedef VectorOf<double> Vector;
    }
}


/**
* \ingroup sig_class
*
* A Base class for a VectorOf<T>, provide default implementation for
* read/write methods. Warning: the current implementation assumes the same
* representation for data type (endianess).
*/
class YARP_sig_API yarp::sig::VectorBase : public yarp::os::Portable
{
public:
    virtual size_t getElementSize() const = 0;
    virtual int getBottleTag() const = 0;

    virtual size_t getListSize() const = 0;
    virtual const char *getMemoryBlock() const = 0;
    virtual char *getMemoryBlock() = 0;
    virtual void resize(size_t size) = 0;

    /*
    * Read vector from a connection.
    * return true iff a vector was read correctly
    */
    virtual bool read(yarp::os::ConnectionReader& connection) override;

    /**
    * Write vector to a connection.
    * return true iff a vector was written correctly
    */
    virtual bool write(yarp::os::ConnectionWriter& connection) const override;

protected:
    virtual std::string getFormatStr(int tag) const;

};

/*
* This is a simple function that maps a type into its corresponding BOTTLE tag.
* Used for bottle compatible serialization, called inside getBottleTag().
* Needs to be instantiated for each type T used in VectorOf<T>.
*/
template<class T>
inline int BottleTagMap () {
    /* make sure this is never called unspecified */
    yAssert(0);
    return 0;
  }

template<>
inline int BottleTagMap <double> () {
    return BOTTLE_TAG_FLOAT64;
  }

template<>
inline int BottleTagMap <int> () {
    return BOTTLE_TAG_INT32;
  }

/**
* \ingroup sig_class
*
* Provides:
* - push_back(), pop_back() to add/remove an element at the end of the vector
* - resize(), to create an array of elements
* - clear(), to clean the array (remove all elements)
* - use [] to access single elements without range checking
* - use size() to get the current size of the Vector
* - use operator= to copy Vectors
* - read/write network methods
* Warning: the class is designed to work with simple types (i.e. types
* that do not allocate internal memory). Template instantiation needs to
* be checked to avoid unresolved externals. Network communication assumes
* same data representation (endianess) between machines.
*/
template<class T>
class yarp::sig::VectorOf : public VectorBase
{
private:
    yarp::os::ManagedBytes bytes;
    T *first;
    size_t len;

    inline void _updatePointers() {
        len = bytes.used()/sizeof(T);
        if (len==0) {
            first = nullptr;
        } else {
            first = reinterpret_cast<T*>(bytes.get());
        }
    }

public:
    VectorOf() {
        bytes.allocate(16*sizeof(T)); // preallocate space for 16 elements
        bytes.setUsed(0);
        first = nullptr;
        len = 0;
    }

    VectorOf(size_t size) : bytes(size*sizeof(T)) {
        bytes.setUsed(size*sizeof(T));
        _updatePointers();
    }

    /**
    * Build a vector and initialize it with def.
    * @param s the size
    * @param def a default value used to fill the vector
    */
    VectorOf(size_t s, const T&def)
    {
        this->resize(s,def);
    }

    /**
    * Builds a vector and initialize it with
    * values from 'p'. Copies memory.
    * @param s the size of the data to be copied
    * @param T* the pointer to the data
    */
    VectorOf(size_t s, const T *p)
    {
        len = 0;
        this->resize(s);

        memcpy(this->data(), p, sizeof(T)*s);
    }

    VectorOf(const VectorOf &r) : VectorBase() {
        bytes = r.bytes;
        _updatePointers();
    }

    /**
    * Copy operator;
    */
    const VectorOf<T> &operator=(const VectorOf<T> &r) {

        if (this == &r) return *this;

        if(this->size() == r.size())
        {
            memcpy(this->data(), r.data(), sizeof(T)*r.size());
        }
        else
        {
            bytes = r.bytes;
            _updatePointers();
        }
        return *this;
    }

    virtual size_t getElementSize() const override {
        return sizeof(T);
    }

    virtual int getBottleTag() const override {
        return BottleTagMap <T>();
    }

    virtual size_t getListSize() const override
    {
        return len;
    }

    virtual const char* getMemoryBlock() const override
    {
        return bytes.get();
    }

    virtual char* getMemoryBlock() override
    {
        return bytes.get();
    }

    inline const T *getFirst() const
    {
        return first;
    }

    inline T *getFirst()
    {
        return first;
    }

    /**
    * Return a pointer to the first element of the vector.
    * @return a pointer to double (or nullptr if the vector is of zero length)
    */
    inline T *data()
    { return first; }

    /**
    * Return a pointer to the first element of the vector,
    * const version
    * @return a (const) pointer to double (or nullptr if the vector is of zero length)
    */
    inline const T *data() const
    { return first;}

    /**
    * Resize the vector.
    * @param s the new size
    */
    virtual void resize(size_t size) override
    {
        size_t prev_len = len;
        bytes.allocateOnNeed(size*sizeof(T),size*sizeof(T));
        bytes.setUsed(size*sizeof(T));
        _updatePointers();
        for (size_t i = prev_len; i < size; i++) {
            new (&((*this)[i])) T(); // non-allocating placement operator new
        }
    }

    /**
    * Resize the vector and initilize the element to a default value.
    * @param s the new size
    * @param def the default value
    */
    void resize(size_t size, const T&def)
    {
        bytes.allocateOnNeed(size*sizeof(T),size*sizeof(T));
        bytes.setUsed(size*sizeof(T));
        _updatePointers();
        for (size_t i = 0; i < size; i++) {
            new (&((*this)[i])) T(def); // non-allocating placement operator new
        }
    }

    /**
    * Push a new element in the vector: size is changed
    */
    inline void push_back (const T &elem)
    {
        bytes.allocateOnNeed(bytes.used()+sizeof(T),bytes.length()*2+sizeof(T));
        bytes.setUsed(bytes.used()+sizeof(T));
        _updatePointers();
        first[len-1] = elem;
    }

    /**
    * Pop an element out of the vector: size is changed
    */
    inline void pop_back (void)
    {
        if (bytes.used()>sizeof(T)) {
            bytes.setUsed(bytes.used()-sizeof(T));
            len--;
            _updatePointers();
        }
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline T &operator[](size_t i)
    {
        return first[i];
    }

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const T &operator[](size_t i) const
    {
        return first[i];
    }

    /**
    * Single element access, no range check.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline T &operator()(size_t i)
    {
        return first[i];
    }

    /**
    * Single element access, no range check, const version.
    * @param i the index of the element to access.
    * @return a reference to the requested element.
    */
    inline const T &operator()(size_t i) const
    {
        return first[i];
    }

    inline size_t size() const {
        return len;
    }

    /**
    * Get the length of the vector.
    * @return the length of the vector.
    */
    inline size_t length() const
    { return this->size();}

    /**
    * Zero the elements of the vector.
    */
    void zero()
    {
        memset(this->data(), 0, sizeof(T)*this->size());
    }

    /**
    * Creates a string object containing a text representation of the object. Useful for printing.
    * To get a nice format the optional parameters precision and width may be used (same meaning as in printf and cout).
    * @param precision the number of digits to be printed after the decimal point.
    * @param width minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces. The value is never truncated.
    * If width is specified the inter-value separator is a blank space, otherwise it is a tab.
    * Warning: the string format might change in the future. This method
    * is here to ease debugging.
    */
    yarp::os::ConstString toString(int precision=-1, int width=-1) const
    {
        yarp::os::ConstString ret = "";
        size_t c = 0;
        const size_t buffSize = 256;
        char tmp[buffSize];
        std::string formatStr;
        if (getBottleTag() == BOTTLE_TAG_FLOAT64) {
            if (width<0) {
                formatStr = "% .*lf\t";
                for (c=0;c<length();c++) {
                    snprintf(tmp, buffSize, formatStr.c_str(), precision, (*this)[c]);
                    ret+=tmp;
                }
            }
            else{
                formatStr = "% *.*lf ";
                for (c=0;c<length();c++){
                    snprintf(tmp, buffSize, formatStr.c_str(), width, precision, (*this)[c]);
                    ret+=tmp;
                }
            }
        }
        else {
            formatStr = "%" + getFormatStr(getBottleTag()) + " ";
            for (c=0;c<length();c++) {
                snprintf(tmp, buffSize, formatStr.c_str(), (*this)[c]);
                ret+=tmp;
            }
        }

        if (length()>=1)
            return ret.substr(0, ret.length()-1);
        return ret;
    }

    /**
    * Creates and returns a new vector, being the portion of the original
    * vector defined by the first and last indexes of the items to be included
    * in the subvector. The indexes are checked: if wrong, a null vector is
    * returned.
    */
    VectorOf<T> subVector(unsigned int first, unsigned int last) const
    {
        VectorOf<T> ret;
        if ((first<=last)&&((int)last<(int)this->size()))
        {
            ret.resize(last-first+1);
            for (unsigned int k=first; k<=last; k++)
                ret[k-first]=(*this)[k];
        }
        return ret;
    }

    /**
     * Set a portion of this vector with the values of the specified vector.
     * If the specified vector v is to big the method does not resize the vector,
     * but return false.
     *
     * @param position index of the first value to set
     * @param v vector containing the values to set
     * @return true if the operation succeeded, false otherwise
     */
    bool setSubvector(int position, const VectorOf<T> &v)
    {
        if (position+v.size() > this->size())
            return false;
        for (size_t i=0;i<v.size();i++)
            (*this)[position+i] = v(i);
        return true;
    }

    /**
    * Set all elements of the vector to a scalar.
    */
    const VectorOf<T> &operator=(T v)
    {
        T *tmp = this->data();

        for(size_t k=0; k<length(); k++)
            tmp[k]=v;

        return *this;
    }

    /**
    * True iff all elements of 'a' match all element of 'b'.
    */
    bool operator==(const VectorOf<T> &r) const
    {
        //check dimensions first
        size_t c=size();
        if (c!=r.size())
            return false;

        const T *tmp1=data();
        const T *tmp2=r.data();

        while(c--)
        {
            if (*tmp1++!=*tmp2++)
                return false;
        }

        return true;

    }

    void clear() {
        bytes.clear();
        bytes.setUsed(0);
        len = 0;
        first = nullptr;
    }

    virtual yarp::os::Type getType() const override {
        return yarp::os::Type::byName("yarp/vector");
    }
};


#ifdef _MSC_VER
/*YARP_sig_EXTERN*/ template class YARP_sig_API yarp::sig::VectorOf<double>;
#endif

#endif // YARP_SIG_VECTOR_H
