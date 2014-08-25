/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Bit Array and Iterator
 * Copyright 2010, 2011 Michael Culbertson <culbert1@illinois.edu>
 *
 *  OSCATS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OSCATS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OSCATS.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LIBOSCATS_BITARRAY_H_
#define _LIBOSCATS_BITARRAY_H_
#include <glib-object.h>
G_BEGIN_DECLS

#define G_TYPE_BIT_ARRAY		(g_bit_array_get_type())
#define G_BIT_ARRAY(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_BIT_ARRAY, GBitArray))
#define G_IS_BIT_ARRAY(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_BIT_ARRAY))
#define G_BIT_ARRAY_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_BIT_ARRAY, GBitArrayClass))
#define G_IS_BIT_ARRAY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_BIT_ARRAY))
#define G_BIT_ARRAY_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_BIT_ARRAY, GBitArrayClass))

typedef struct _GBitArray GBitArray;
typedef struct _GBitArrayClass GBitArrayClass;

struct _GBitArray {
  GObject parent_instance;
  /*< read only >*/
  guint num_set;
  /*< private >*/
  guint8 *data;
  guint bit_len, byte_len;
  gint bit_pos, byte_pos;
};

struct _GBitArrayClass {
  GObjectClass parent_class;
};

GType g_bit_array_get_type();

GBitArray* g_bit_array_new(guint bit_length);
GBitArray* g_bit_array_resize(GBitArray* array, guint bit_length);
GBitArray* g_bit_array_extend(GBitArray* array, guint num);
void g_bit_array_copy(GBitArray* lhs, const GBitArray* rhs);
guint g_bit_array_get_len(const GBitArray* array);
guint g_bit_array_get_num_set(const GBitArray* array);

gboolean g_bit_array_get_bit(const GBitArray* array, guint pos);
GBitArray* g_bit_array_set_bit(GBitArray* array, guint pos);
GBitArray* g_bit_array_clear_bit(GBitArray* array, guint pos);
gboolean g_bit_array_flip_bit(GBitArray* array, guint pos);
GBitArray* g_bit_array_set_bit_val(GBitArray* array, guint pos, gboolean val);
GBitArray* g_bit_array_set_range(GBitArray* array, guint start, guint stop, gboolean val);
GBitArray* g_bit_array_reset(GBitArray* array, gboolean val);
gboolean g_bit_array_equal(GBitArray *lhs, GBitArray *rhs);
gint g_bit_array_serial_compare(const GBitArray *a, const GBitArray *b);

void g_bit_array_iter_reset(GBitArray* array);
gint g_bit_array_iter_next(GBitArray* array);

G_END_DECLS
#endif
