/*
 * Copyright © 2009  Red Hat, Inc.
 * Copyright © 2011 Codethink Limited
 * Copyright © 2010,2011  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Red Hat Author(s): Behdad Esfahbod
 * Codethink Author(s): Ryan Lortie
 * Google Author(s): Behdad Esfahbod
 */

#include "hb-private.hh"

#include "hb-unicode-private.hh"

HB_BEGIN_DECLS


/*
 * hb_unicode_funcs_t
 */

#define HB_UNICODE_FUNC_IMPLEMENT(return_type, name, default_value)		\
										\
										\
static return_type								\
hb_unicode_get_##name##_nil (hb_unicode_funcs_t *ufuncs    HB_UNUSED,		\
			     hb_codepoint_t      unicode   HB_UNUSED,		\
			     void               *user_data HB_UNUSED)		\
{										\
  return default_value;								\
}

  HB_UNICODE_FUNCS_IMPLEMENT_CALLBACKS
#undef HB_UNICODE_FUNC_IMPLEMENT


hb_unicode_funcs_t _hb_unicode_funcs_nil = {
  HB_OBJECT_HEADER_STATIC,

  NULL, /* parent */
  TRUE, /* immutable */
  {
#define HB_UNICODE_FUNC_IMPLEMENT(return_type, name, default_value) hb_unicode_get_##name##_nil,
    HB_UNICODE_FUNCS_IMPLEMENT_CALLBACKS
#undef HB_UNICODE_FUNC_IMPLEMENT
  }
};


hb_unicode_funcs_t *
hb_unicode_funcs_get_default (void)
{
  return &_hb_unicode_funcs_default;
}

hb_unicode_funcs_t *
hb_unicode_funcs_create (hb_unicode_funcs_t *parent)
{
  hb_unicode_funcs_t *ufuncs;

  if (!(ufuncs = hb_object_create<hb_unicode_funcs_t> ()))
    return &_hb_unicode_funcs_nil;

  if (!parent)
    parent = &_hb_unicode_funcs_nil;

  hb_unicode_funcs_make_immutable (parent);
  ufuncs->parent = hb_unicode_funcs_reference (parent);

  ufuncs->get = parent->get;

  /* We can safely copy user_data from parent since we hold a reference
   * onto it and it's immutable.  We should not copy the destroy notifiers
   * though. */
  ufuncs->user_data = parent->user_data;

  return ufuncs;
}

hb_unicode_funcs_t *
hb_unicode_funcs_get_empty (void)
{
  return &_hb_unicode_funcs_nil;
}

hb_unicode_funcs_t *
hb_unicode_funcs_reference (hb_unicode_funcs_t *ufuncs)
{
  return hb_object_reference (ufuncs);
}

void
hb_unicode_funcs_destroy (hb_unicode_funcs_t *ufuncs)
{
  if (!hb_object_destroy (ufuncs)) return;

#define HB_UNICODE_FUNC_IMPLEMENT(return_type, name, default_value) \
  if (ufuncs->destroy.name) ufuncs->destroy.name (ufuncs->user_data.name);
    HB_UNICODE_FUNCS_IMPLEMENT_CALLBACKS
#undef HB_UNICODE_FUNC_IMPLEMENT

  hb_unicode_funcs_destroy (ufuncs->parent);

  free (ufuncs);
}

hb_bool_t
hb_unicode_funcs_set_user_data (hb_unicode_funcs_t *ufuncs,
			        hb_user_data_key_t *key,
			        void *              data,
			        hb_destroy_func_t   destroy)
{
  return hb_object_set_user_data (ufuncs, key, data, destroy);
}

void *
hb_unicode_funcs_get_user_data (hb_unicode_funcs_t *ufuncs,
			        hb_user_data_key_t *key)
{
  return hb_object_get_user_data (ufuncs, key);
}


void
hb_unicode_funcs_make_immutable (hb_unicode_funcs_t *ufuncs)
{
  if (hb_object_is_inert (ufuncs))
    return;

  ufuncs->immutable = TRUE;
}

hb_bool_t
hb_unicode_funcs_is_immutable (hb_unicode_funcs_t *ufuncs)
{
  return ufuncs->immutable;
}

hb_unicode_funcs_t *
hb_unicode_funcs_get_parent (hb_unicode_funcs_t *ufuncs)
{
  return ufuncs->parent ? ufuncs->parent : &_hb_unicode_funcs_nil;
}


#define HB_UNICODE_FUNC_IMPLEMENT(return_type, name, default_value)		\
										\
void										\
hb_unicode_funcs_set_##name##_func (hb_unicode_funcs_t		   *ufuncs,	\
				    hb_unicode_get_##name##_func_t  func,	\
				    void			   *user_data,	\
				    hb_destroy_func_t		    destroy)	\
{										\
  if (ufuncs->immutable)							\
    return;									\
										\
  if (ufuncs->destroy.name)							\
    ufuncs->destroy.name (ufuncs->user_data.name);				\
										\
  if (func) {									\
    ufuncs->get.name = func;							\
    ufuncs->user_data.name = user_data;						\
    ufuncs->destroy.name = destroy;						\
  } else {									\
    ufuncs->get.name = ufuncs->parent->get.name;				\
    ufuncs->user_data.name = ufuncs->parent->user_data.name;			\
    ufuncs->destroy.name = NULL;						\
  }										\
}										\
										\
return_type									\
hb_unicode_get_##name (hb_unicode_funcs_t *ufuncs,				\
		       hb_codepoint_t      unicode)				\
{										\
  return ufuncs->get.name (ufuncs, unicode, ufuncs->user_data.name);		\
}

    HB_UNICODE_FUNCS_IMPLEMENT_CALLBACKS
#undef HB_UNICODE_FUNC_IMPLEMENT


HB_END_DECLS
