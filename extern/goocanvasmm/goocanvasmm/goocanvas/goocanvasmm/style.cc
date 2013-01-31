// Generated by gtkmmproc -- DO NOT MODIFY!


#include <goocanvasmm/style.h>
#include <goocanvasmm/private/style_p.h>


/* Copyright (C) 1998-2006 The gtkmm Development Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "style.h"
#include <goocanvasstyle.h>

namespace Goocanvas
{

}


namespace
{
} // anonymous namespace


namespace Glib
{

Glib::RefPtr<Goocanvas::Style> wrap(GooCanvasStyle* object, bool take_copy)
{
  return Glib::RefPtr<Goocanvas::Style>( dynamic_cast<Goocanvas::Style*> (Glib::wrap_auto ((GObject*)(object), take_copy)) );
  //We use dynamic_cast<> in case of multiple inheritance.
}

} /* namespace Glib */


namespace Goocanvas
{


/* The *_Class implementation: */

const Glib::Class& Style_Class::init()
{
  if(!gtype_) // create the GType if necessary
  {
    // Glib::Class has to know the class init function to clone custom types.
    class_init_func_ = &Style_Class::class_init_function;

    // This is actually just optimized away, apparently with no harm.
    // Make sure that the parent type has been created.
    //CppClassParent::CppObjectType::get_type();

    // Create the wrapper type, with the same class/instance size as the base type.
    register_derived_type(goo_canvas_style_get_type());

    // Add derived versions of interfaces, if the C type implements any interfaces:

  }

  return *this;
}


void Style_Class::class_init_function(void* g_class, void* class_data)
{
  BaseClassType *const klass = static_cast<BaseClassType*>(g_class);
  CppClassParent::class_init_function(klass, class_data);


}


Glib::ObjectBase* Style_Class::wrap_new(GObject* object)
{
  return new Style((GooCanvasStyle*)object);
}


/* The implementation: */

GooCanvasStyle* Style::gobj_copy()
{
  reference();
  return gobj();
}

Style::Style(const Glib::ConstructParams& construct_params)
:
  Glib::Object(construct_params)
{

}

Style::Style(GooCanvasStyle* castitem)
:
  Glib::Object((GObject*)(castitem))
{}


Style::~Style()
{}


Style::CppClassType Style::style_class_; // initialize static member

GType Style::get_type()
{
  return style_class_.init().get_type();
}


GType Style::get_base_type()
{
  return goo_canvas_style_get_type();
}


Style::Style()
:
  // Mark this class as non-derived to allow C++ vfuncs to be skipped.
  Glib::ObjectBase(0),
  Glib::Object(Glib::ConstructParams(style_class_.init()))
{
  

}

Glib::RefPtr<Style> Style::create()
{
  return Glib::RefPtr<Style>( new Style() );
}
Glib::RefPtr<Style> Style::copy() const
{
  return Glib::wrap(goo_canvas_style_copy(const_cast<GooCanvasStyle*>(gobj())));
}

Glib::RefPtr<Style> Style::get_parent() const
{

  Glib::RefPtr<Style> retvalue = Glib::wrap(goo_canvas_style_get_parent(const_cast<GooCanvasStyle*>(gobj())));
  if(retvalue)
    retvalue->reference(); //The function does not do a ref for us.
  return retvalue;

}

void Style::set_parent(const Glib::RefPtr<Style>& style)
{
goo_canvas_style_set_parent(gobj(), Glib::unwrap(style)); 
}

bool Style::set_stroke_options(const Cairo::RefPtr<Cairo::Context>& context)
{
  return goo_canvas_style_set_stroke_options(gobj(), (context)->cobj());
}

bool Style::set_fill_options(const Cairo::RefPtr<Cairo::Context>& context)
{
  return goo_canvas_style_set_fill_options(gobj(), (context)->cobj());
}


} // namespace Goocanvas

