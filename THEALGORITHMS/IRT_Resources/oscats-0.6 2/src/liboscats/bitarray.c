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

/**
 * SECTION:bitarray
 * @title:GBitArray
 * @short_description: An array of bit flags
 */

#include "bitarray.h"

G_DEFINE_TYPE(GBitArray, g_bit_array, G_TYPE_OBJECT);

static void g_bit_array_finalize (GObject *object);

static void g_bit_array_class_init (GBitArrayClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = g_bit_array_finalize;
}

static void g_bit_array_init (GBitArray *self)
{
  self->data = NULL;
  g_bit_array_resize(self, 0);
}

static void g_bit_array_finalize (GObject *object)
{
  GBitArray *self = G_BIT_ARRAY(object);
  g_free(self->data);
  G_OBJECT_CLASS(g_bit_array_parent_class)->finalize(object);
}

static void count_bits(GBitArray *array)
{
  guint i, pos, num=0, max = array->bit_len;
  guint8 mask=0x1;
  for (pos=0, i=0; i < max; i++, mask <<=1)
  {
    if (!mask) { pos++; mask = 0x1; }
    if (array->data[pos] & mask) num++;
  }
  array->num_set = num;
}

/**
 * g_bit_array_new:
 * @bit_length: the number of bits
 *
 * Returns a new #GBitArray with the given length.
 * Note that the bit states are undefined.
 *
 * Returns: the new #GBitArray
 */
GBitArray* g_bit_array_new(guint bit_length)
{
  GBitArray *p;
  p = g_object_new(G_TYPE_BIT_ARRAY, NULL);
  g_bit_array_resize(p, bit_length);
  return p;
}

/**
 * g_bit_array_resize:
 * @array: a #GBitArray
 * @bit_length: The non-negative number of bits
 *
 * Resizes the #GBitArray to the specified length.
 * All bits are cleared after this operation.
 *
 * Returns: the resized #GBitArray
 */
GBitArray* g_bit_array_resize(GBitArray* array, guint bit_length)
{
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), NULL);
  g_free(array->data);
  array->bit_len = bit_length;
  array->byte_len = bit_length / 8;
  if (bit_length & 0x7) array->byte_len++;
  if (array->byte_len > 0) array->data = g_new0(guint8, array->byte_len);
  else array->data = NULL;
  array->byte_pos = array->byte_len+1;
  array->num_set = 0;
  return array;
}

/**
 * g_bit_array_extend:
 * @array: a #GBitArray
 * @num: number of bits to add
 *
 * Extends @array by @num bits.  Unlike g_bit_array_resize(), maintains
 * the existing bits.  New bits are cleared.
 *
 * Returns: the extended #GBitArray
 */
GBitArray* g_bit_array_extend(GBitArray* array, guint num)
{
  guint8 *data;
  guint i, n;
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), NULL);
  if (num == 0) return array;
  i = array->bit_len;
  if (i & 0x7)
  {
    n = 8-(i & 0x7);			// Bits remaining in last byte
    if (n > num) n = num;
    array->bit_len += n;
    num -= n;
    if (num == 0) return array;
    i += n;
  }
  n = (i + num)/8;			// New byte count
  if ((i + num) & 0x7) n++;
  data = g_new(guint8, n);
  for (i=0; i < array->byte_len; i++)	// Copy old bits
    data[i] = array->data[i];
  for (; i < n; i++)			// Clear new bits
    data[i] = 0;
  g_free(array->data);
  array->data = data;
  array->bit_len += num;
  array->byte_len = n;
  return array;
}

/**
 * g_bit_array_copy:
 * @lhs: #GBitArray to be overwritten
 * @rhs: #GBitArray to be copied
 *
 * Resizes @lhs as necessary.
 */
void g_bit_array_copy(GBitArray *lhs, const GBitArray *rhs)
{
  guint i;
  g_return_if_fail(G_IS_BIT_ARRAY(lhs) && G_IS_BIT_ARRAY(rhs));
  if (lhs->byte_len != rhs->byte_len)
    g_bit_array_resize(lhs, rhs->bit_len);
  for (i=0; i < rhs->byte_len; i++)
    lhs->data[i] = rhs->data[i];
  lhs->num_set = rhs->num_set;
}

/**
 * g_bit_array_get_len:
 * @array: a #GBitArray
 *
 * Returns the bit length of the array.
 *
 * Returns: the length of the array in bits
 */
guint g_bit_array_get_len(const GBitArray*array)
{
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), 0);
  return array->bit_len;
}

/**
 * g_bit_array_get_num_set:
 * @array: a #GBitArray
 *
 * Returns: the number of bits that are set
 */
guint g_bit_array_get_num_set(const GBitArray*array)
{
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), 0);
  return array->num_set;
}

/**
 * g_bit_array_get_bit:
 * @array: a #GBitArray
 * @pos: the index of the bit to query
 *
 * Returns whether or not the bit indexed by @pos is set.
 * The index must satisfy 0 <= @pos < length.
 *
 * Returns: gboolean status
 */
gboolean g_bit_array_get_bit(const GBitArray* array, guint pos)
{
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), FALSE);
  g_return_val_if_fail(pos < array->bit_len, FALSE);
  return array->data[pos/8] & (0x1 << (pos & 0x7));
}

/**
 * g_bit_array_set_bit:
 * @array: a #GBitArray
 * @pos: the index of the bit to set
 *
 * Sets the bit indexed by @pos to %TRUE.
 * The index must satisfy 0 <= @pos < length.
 *
 * Returns: the #GBitArray
 */
GBitArray* g_bit_array_set_bit(GBitArray* array, guint pos)
{
  guint8 mask = (0x1 << (pos & 0x7));
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), NULL);
  g_return_val_if_fail(pos < array->bit_len, NULL);
  pos /= 8;
  if (!(array->data[pos] & mask))
  {
    array->num_set++;
    array->data[pos] |= mask;
  }
  return array;
}

/**
 * g_bit_array_clear_bit:
 * @array: a #GBitArray
 * @pos: the index of the bit to clear
 *
 * Sets the bit indexed by @pos to %FALSE.
 * The index must satisfy 0 <= @pos < length.
 *
 * Returns: the #GBitArray
 */
GBitArray* g_bit_array_clear_bit(GBitArray* array, guint pos)
{
  guint8 mask = (0x1 << (pos & 0x7));
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), NULL);
  g_return_val_if_fail(pos < array->bit_len, NULL);
  pos /= 8;
  if (array->data[pos] & mask)
  {
    array->num_set--;
    array->data[pos] &= ~mask;
  }
  return array;
}

/**
 * g_bit_array_flip_bit:
 * @array: a #GBitArray
 * @pos: the index of the bit to flip
 *
 * Negates the bit indexed by @pos.
 * The index must satisfy 0 <= @pos < length.
 *
 * Returns: the new value of the bit
 */
gboolean g_bit_array_flip_bit(GBitArray* array, guint pos)
{
  guint8 mask = (0x1 << (pos & 0x7));
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), FALSE);
  g_return_val_if_fail(pos < array->bit_len, FALSE);
  pos /= 8;
  if (array->data[pos] & mask) array->num_set--;
  else                         array->num_set++;
  array->data[pos] ^= mask;
  return array->data[pos] & mask;
}

/**
 * g_bit_array_set_bit_val:
 * @array: a #GBitArray
 * @pos: the index of the bit to clear
 * @val: a boolean value
 *
 * Sets the bit indexed by @pos to @val.
 * The index must satisfy 0 <= @pos < length.
 *
 * Returns: the #GBitArray
 */
GBitArray* g_bit_array_set_bit_val(GBitArray* array, guint pos, gboolean val)
{
  if (val) return g_bit_array_set_bit(array, pos);
  else return g_bit_array_clear_bit(array, pos);
}

/**
 * g_bit_array_set_range:
 * @array: a #GBitArray
 * @start: the index of the first bit to set
 * @stop: the index of the last bit to set
 * @val: a boolean value
 *
 * Sets all of the bits between @start and @stop (inclusive) to @val.
 * The indeces must satisfy 0 <= @start <= @stop < length.
 *
 * Returns: the #GBitArray
 */
GBitArray* g_bit_array_set_range(GBitArray* array, guint start, guint stop,
                                 gboolean val)
{
  static const guint8 masks[] = { 255, 254, 252, 248, 240, 224, 192, 128, 0 };
  guint8 V = (val ? 255 : 0);
  guint i;
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), NULL);
  g_return_val_if_fail(start <= stop && stop < array->bit_len, NULL);
  if (start/8 == stop/8)			// Only within one byte
  {
    for (i=start; i <= stop; i++)
      g_bit_array_set_bit_val(array, i, val);
    return array;
  }
  if (val)
  {
    array->data[start/8] |= masks[start & 0x7];		// First byte
    array->data[stop/8] |= ~masks[(stop & 0x7)+1];	// Last byte
  } else {
    array->data[start/8] &= ~masks[start & 0x7];	// First byte
    array->data[stop/8] &= masks[(stop & 0x7)+1];	// Last byte
  }
  stop /= 8;
  for (i = start/8 + 1; i < stop; i++)
    array->data[i] = V;
  count_bits(array);
  return array;
}

/**
 * g_bit_array_reset:
 * @array: a #GBitArray
 * @val: a boolean value
 *
 * Sets all of the bits in the array to @val.
 *
 * Returns: the #GBitArray
 */
GBitArray* g_bit_array_reset(GBitArray* array, gboolean val)
{
  guint8 V = (val ? 255 : 0);
  guint i;
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), NULL);
  for (i=0; i < array->byte_len; i++)
    array->data[i] = V;
  array->num_set = (val ? array->bit_len : 0);
  return array;
}

/**
 * g_bit_array_equal:
 * @lhs: a #GBitArray
 * @rhs: another #GBitArray
 *
 * If @lhs and @rhs are not the same length, returns %FALSE.
 *
 * Returns: %TRUE if @lhs and @rhs have the same value
 */
gboolean g_bit_array_equal(GBitArray *lhs, GBitArray *rhs)
{
  guint i;
  g_return_val_if_fail(G_IS_BIT_ARRAY(lhs) && G_IS_BIT_ARRAY(rhs), FALSE);
  if (lhs->bit_len != rhs->bit_len) return FALSE;
  if (lhs->num_set != rhs->num_set) return FALSE;
  for (i=0; i < lhs->byte_len; i++)
    if (lhs->data[i] != lhs->data[i]) return FALSE;
  return TRUE;
}

/**
 * g_bit_array_serial_compare:
 * @a: a #GBitArray object
 * @b: a #GBitArray object
 *
 * Compares a serialization of @a and @b.  The serialization interprets
 * the bit array as a base-2 integer.
 *
 * Returns: 0 if @a == @b, -1 if @a < @b, and 1 if @a > @b.
 */
gint g_bit_array_serial_compare(const GBitArray *a, const GBitArray *b)
{
  gint i;
  g_return_val_if_fail(G_IS_BIT_ARRAY(a) && G_IS_BIT_ARRAY(b), 0);
  g_return_val_if_fail(a->bit_len == b->bit_len, 0);
  // Last byte is most significant
  for (i=a->byte_len-1; i >= 0 && a->data[i] == b->data[i]; i--) ;
  if (i < 0) return 0;
  if (a->data[i] < b->data[i]) return -1;
  else return 1;
}

/**
 * g_bit_array_iter_reset:
 * @array: a #GBitArray
 *
 * Restarts the iterator at the beginning.  The next call to
 * g_bit_array_iter_next() will return the index of the first true bit.
 */
void g_bit_array_iter_reset(GBitArray* array)
{
  g_return_if_fail(G_IS_BIT_ARRAY(array));
  array->byte_pos = -1;
  array->bit_pos = 7;
}

/**
 * g_bit_array_iter_next:
 * @array: a #GBitArray
 *
 * Returns the index of the next %TRUE bit.
 * Returns -1 when the end of the array has been reached.
 *
 * Returns: the bit index, or -1
 */
gint g_bit_array_iter_next(GBitArray* array)
{
  guint8 mask, byte;
  gint i, max, ret;
  g_return_val_if_fail(G_IS_BIT_ARRAY(array), 0);
  max = array->byte_len;
  i = array->byte_pos;
  if (i >= max) return -1;
  mask = (1 << (array->bit_pos+1));	// Increment last position
  if (mask == 0) { i++; mask = 1; }
  byte = array->data[i];
  for (; i < max; )
  {
    if (byte & mask)
    {
      array->byte_pos = i;
      for (array->bit_pos=0, mask >>= 1; mask; array->bit_pos++, mask >>= 1) ;
      ret = i*8 + array->bit_pos;
      return (ret >= array->bit_len ? -1 : ret);
    }
    if ((mask <<= 1) == 0)
    {
      if (++i < max) byte = array->data[i];
      mask = 1;
    }
  }
  array->byte_pos = i;
  return -1;
}
