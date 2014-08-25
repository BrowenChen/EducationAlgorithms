/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Support Algorithm: Stratify items based on some criterion
 * Copyright 2011 Michael Culbertson <culbert1@illinois.edu>
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

#include "algorithms/stratify.h"

enum {
  PROP_0,
  PROP_NUM,
  PROP_BANK,
};

G_DEFINE_TYPE(OscatsAlgStratify, oscats_alg_stratify, G_TYPE_OBJECT);

static void oscats_alg_stratify_dispose(GObject *object);
static void oscats_alg_stratify_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_stratify_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);

static void oscats_alg_stratify_class_init (OscatsAlgStratifyClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_alg_stratify_dispose;
  gobject_class->set_property = oscats_alg_stratify_set_property;
  gobject_class->get_property = oscats_alg_stratify_get_property;

/**
 * OscatsAlgStratify:num:
 *
 * Number of strata.
 */
  pspec = g_param_spec_uint("num", "", 
                            "Number of strata",
                            0, G_MAXUINT, 1,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM, pspec);

/**
 * OscatsAlgStratify:itembank:
 *
 * The item bank to stratify.
 */
  pspec = g_param_spec_object("itembank", "Item Bank", 
                              "Item Bank to stratify",
                              OSCATS_TYPE_ITEM_BANK,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_BANK, pspec);

}

static void oscats_alg_stratify_init (OscatsAlgStratify *self)
{
}

static void oscats_alg_stratify_dispose (GObject *object)
{
  OscatsAlgStratify *self = OSCATS_ALG_STRATIFY(object);
  G_OBJECT_CLASS(oscats_alg_stratify_parent_class)->dispose(object);
  if (self->bank) g_object_unref(self->bank);
  if (self->strata) g_ptr_array_unref(self->strata);
  self->bank = NULL;
  self->strata = NULL;
}

static void oscats_alg_stratify_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgStratify *self = OSCATS_ALG_STRATIFY(object);
  switch (prop_id)
  {
    case PROP_BANK:			// construction only
      self->bank = g_value_dup_object(value);
      break;

    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_stratify_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgStratify *self = OSCATS_ALG_STRATIFY(object);
  switch (prop_id)
  {
    case PROP_NUM:
      g_value_set_uint(value, self->strata ? self->strata->len : 0);
      break;

    case PROP_BANK:
      g_value_set_object(value, self->bank);
      break;

    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

typedef struct
{
  guint item;
  gdouble val;
} Pair;

static gint pair_compare(gconstpointer A, gconstpointer B)
{
  const Pair *a = A, *b = B;
  if (a->val == b->val) return 0;
  return (a->val < b->val ? -1 : 1);
}

/**
 * oscats_alg_stratify_stratify:
 * @stratify: an #OscatsAlgStratify object
 * @n_strata: the number of strata
 * @f: the criterion function by which to stratify
 * @f_data: (allow-none): user data to supply to @f
 * @n_blocks: the number of blocks
 * @block: (allow-none): the criterion function by which to block
 * @b_data: (allow-none): user data to supply to @block
 *
 * Stratify the #OscatsAlgStratify:itembank of @stratify using the criterion
 * @f within blocks specified by @block.  The bank can be restratified by
 * calling this function multiple times.
 */
void oscats_alg_stratify_stratify(OscatsAlgStratify *stratify,
           guint n_strata, OscatsAlgStratifyCriterion f,     gpointer f_data,
           guint n_blocks, OscatsAlgStratifyCriterion block, gpointer b_data)
{
  GBitArray **strata;
  Pair *pairs;
  GList *blocks_p, *items_p, *items, *blocks = NULL;
  guint i, b, s, n_items, rem, block_size, stratum_size;

  g_return_if_fail(OSCATS_IS_ALG_STRATIFY(stratify));
  g_return_if_fail(OSCATS_IS_ITEM_BANK(stratify->bank));
  g_return_if_fail(f != NULL);
  g_return_if_fail(n_strata > 0);
  if (n_blocks > 1) g_return_if_fail(block != NULL);
  else n_blocks = 1;
  n_items = oscats_item_bank_num_items(stratify->bank);
  g_return_if_fail(n_items > 0);
  
  // Reset the bit arrays
  if (stratify->strata == NULL)
    stratify->strata = g_ptr_array_new_with_free_func(g_object_unref);
  if (stratify->strata->len != n_strata)
    g_ptr_array_set_size(stratify->strata, n_strata);
  strata = (GBitArray**)(stratify->strata->pdata);
  for (s=0; s < n_strata; s++)
  {
    if (strata[s] == NULL) strata[s] = g_bit_array_new(n_items);
    g_bit_array_reset(strata[s], FALSE);
  }
  
  // Compute the block criteria
  pairs = g_new(Pair, n_items);
  g_return_if_fail(pairs != NULL);
  for (i=0; i < n_items; i++)
  {
    pairs[i].item = i;
    if (n_blocks > 1)
      pairs[i].val = block(oscats_item_bank_get_item(stratify->bank, i), b_data);
    else
      pairs[i].val = 0;
    blocks = g_list_insert_sorted(blocks, pairs+i, pair_compare);
  }
    
  for (b=0, blocks_p=blocks; b < n_blocks; b++, n_items -= block_size)
  {
    items = NULL;
    block_size = n_items/(n_blocks-b);

    // Compute the stratification criteria
    for (i=0; i < block_size; i++, blocks_p = g_list_next(blocks_p))
    {
      Pair *pair = blocks_p->data;
      pair->val = f(oscats_item_bank_get_item(stratify->bank, pair->item), f_data);
      items = g_list_insert_sorted(items, pair, pair_compare);
    }

    // Divide into strata
    items_p = items;
    for (s=0, rem=block_size; s < n_strata; s++, rem -= stratum_size)
      for (i=0, stratum_size=rem/(n_strata-s);
           i < stratum_size;
           i++, items_p = g_list_next(items_p))
        g_bit_array_set_bit(strata[s], ((Pair*)(items_p->data))->item);

    g_list_free(items);
  }
  
  g_list_free(blocks);
  g_free(pairs);
}
                                      
/**
 * oscats_alg_stratify_get_stratum:
 * @stratify: an #OscatsAlgStratify object
 * @stratum: the stratum to fetch (0-based index)
 *
 * Returns: (transfer none): the stratum @stratum of @stratify
 */
GBitArray * oscats_alg_stratify_get_stratum(const OscatsAlgStratify *stratify,
                                                  guint stratum)
{
  g_return_val_if_fail(OSCATS_IS_ALG_STRATIFY(stratify), NULL);
  g_return_val_if_fail(stratify->strata != NULL, NULL);
  g_return_val_if_fail(stratum < stratify->strata->len, NULL);
  return stratify->strata->pdata[stratum];
}

/**
 * oscats_alg_stratify_reset:
 * @stratify: an #OscatsAlgStratify object
 *
 * Reset the internal stratum iterator.
 */
void oscats_alg_stratify_reset(OscatsAlgStratify *stratify)
{
  g_return_if_fail(OSCATS_IS_ALG_STRATIFY(stratify));
  stratify->next = 0;
}

/**
 * oscats_alg_stratify_next:
 * @stratify: an #OscatsAlgStratify object
 *
 * Returns the next stratum using the internal stratum iterator and
 * increments the iterator.  Returns %NULL if there are no more strata.
 *
 * Returns: (transfer none): the next stratum
 */
GBitArray * oscats_alg_stratify_next(OscatsAlgStratify *stratify)
{
  g_return_val_if_fail(OSCATS_IS_ALG_STRATIFY(stratify), NULL);
  g_return_val_if_fail(stratify->strata != NULL, NULL);
  if (stratify->next >= stratify->strata->len) return NULL;
  return stratify->strata->pdata[stratify->next++];
}

