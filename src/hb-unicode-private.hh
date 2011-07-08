/*
 * Copyright © 2009  Red Hat, Inc.
 * Copyright © 2011  Codethink Limited
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

#ifndef HB_UNICODE_PRIVATE_HH
#define HB_UNICODE_PRIVATE_HH

#include "hb-private.hh"

#include "hb-unicode.h"
#include "hb-object-private.hh"

HB_BEGIN_DECLS


/*
 * hb_unicode_funcs_t
 */

#define HB_UNICODE_FUNCS_IMPLEMENT_CALLBACKS \
  HB_UNICODE_FUNC_IMPLEMENT (unsigned int, combining_class, 0) \
  HB_UNICODE_FUNC_IMPLEMENT (unsigned int, eastasian_width, 1) \
  HB_UNICODE_FUNC_IMPLEMENT (hb_unicode_general_category_t, general_category, HB_UNICODE_GENERAL_CATEGORY_OTHER_LETTER) \
  HB_UNICODE_FUNC_IMPLEMENT (hb_codepoint_t, mirroring, unicode) \
  HB_UNICODE_FUNC_IMPLEMENT (hb_script_t, script, HB_SCRIPT_UNKNOWN) \
  /* ^--- Add new callbacks here */

struct _hb_unicode_funcs_t {
  hb_object_header_t header;

  hb_unicode_funcs_t *parent;

  bool immutable;

#define HB_UNICODE_FUNC_IMPLEMENT(return_type, name, default_value) \
  inline return_type \
  get_##name (hb_codepoint_t unicode) \
  { return this->get.name (this, unicode, this->user_data.name); }

  HB_UNICODE_FUNCS_IMPLEMENT_CALLBACKS

#undef HB_UNICODE_FUNC_IMPLEMENT

  /* Don't access these directly.  Call get_*() instead. */

  struct {
#define HB_UNICODE_FUNC_IMPLEMENT(return_type, name, default_value) hb_unicode_get_##name##_func_t name;
    HB_UNICODE_FUNCS_IMPLEMENT_CALLBACKS
#undef HB_UNICODE_FUNC_IMPLEMENT
  } get;

  struct {
#define HB_UNICODE_FUNC_IMPLEMENT(return_type, name, default_value) void *name;
    HB_UNICODE_FUNCS_IMPLEMENT_CALLBACKS
#undef HB_UNICODE_FUNC_IMPLEMENT
  } user_data;

  struct {
#define HB_UNICODE_FUNC_IMPLEMENT(return_type, name, default_value) hb_destroy_func_t name;
    HB_UNICODE_FUNCS_IMPLEMENT_CALLBACKS
#undef HB_UNICODE_FUNC_IMPLEMENT
  } destroy;
};


#if HAVE_GLIB
extern HB_INTERNAL hb_unicode_funcs_t _hb_glib_unicode_funcs;
#define _hb_unicode_funcs_default _hb_glib_unicode_funcs
#elif HAVE_ICU
extern HB_INTERNAL hb_unicode_funcs_t _hb_icu_unicode_funcs;
#define _hb_unicode_funcs_default _hb_icu_unicode_funcs
#else
extern HB_INTERNAL hb_unicode_funcs_t _hb_unicode_funcs_nil;
#define _hb_unicode_funcs_default _hb_unicode_funcs_nil
#endif



HB_END_DECLS

#endif /* HB_UNICODE_PRIVATE_HH */
