/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Support Algorithm: Select Item based on arbitrary Criterion
 * Copyright 2010 Michael Culbertson <culbert1@illinois.edu>
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

#include "random.h"
#include "algorithms/chooser.h"

enum {
  PROP_0,
  PROP_NUM,
  PROP_BANK,
};

G_DEFINE_TYPE(OscatsAlgChooser, oscats_alg_chooser, G_TYPE_OBJECT);

static void oscats_alg_chooser_dispose(GObject *object);
static void oscats_alg_chooser_finalize(GObject *object);
static void oscats_alg_chooser_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_chooser_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);

static void oscats_alg_chooser_class_init (OscatsAlgChooserClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_alg_chooser_dispose;
  gobject_class->finalize = oscats_alg_chooser_finalize;
  gobject_class->set_property = oscats_alg_chooser_set_property;
  gobject_class->get_property = oscats_alg_chooser_get_property;

/**
 * OscatsAlgChooser:num:
 *
 * Number of items from which to choose.  If one, then the exact closest
 * item is selected.  If greater than one, then a random item is chosen
 * from among the #OscatsAlgChooser:num closest items.
 */
  pspec = g_param_spec_uint("num", "", 
                            "Number of items from which to choose",
                            1, G_MAXUINT, 1,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM, pspec);

/**
 * OscatsAlgChooser:itembank:
 *
 * The item bank from which to choose items.
 */
  pspec = g_param_spec_object("itembank", "Item Bank", 
                              "Item Bank used for the test",
                              OSCATS_TYPE_ITEM_BANK,
                              G_PARAM_READWRITE |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_BANK, pspec);

}

static void oscats_alg_chooser_init (OscatsAlgChooser *self)
{
}

static void oscats_alg_chooser_dispose (GObject *object)
{
  OscatsAlgChooser *self = OSCATS_ALG_CHOOSER(object);
  G_OBJECT_CLASS(oscats_alg_chooser_parent_class)->dispose(object);
  if (self->bank) g_object_unref(self->bank);
  self->bank = NULL;
}

static void oscats_alg_chooser_finalize (GObject *object)
{
  OscatsAlgChooser *self = OSCATS_ALG_CHOOSER(object);
  if (self->dists) g_array_free(self->dists, TRUE);
  if (self->items) g_array_free(self->items, TRUE);
  G_OBJECT_CLASS(oscats_alg_chooser_parent_class)->finalize(object);
}

static void oscats_alg_chooser_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgChooser *self = OSCATS_ALG_CHOOSER(object);
  switch (prop_id)
  {
    case PROP_NUM:			// construction only
      self->num = g_value_get_uint(value);
      if (self->num > 1)
      {
        self->dists = g_array_sized_new(FALSE, FALSE, sizeof(gdouble), self->num);
        self->items = g_array_sized_new(FALSE, FALSE, sizeof(guint), self->num);
      }
      break;

    case PROP_BANK:
      if (self->bank) g_object_unref(self->bank);
      self->bank = g_value_dup_object(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_chooser_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgChooser *self = OSCATS_ALG_CHOOSER(object);
  switch (prop_id)
  {
    case PROP_NUM:
      g_value_set_uint(value, self->num);
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

/**
 * oscats_alg_chooser_set_c_criterion:
 * @chooser: an #OscatsAlgChooser object
 * @f: the criterion function to be minimized
 *
 * Sets the criterion function @f, which takes an #OscatsItem, an
 * #OscatsExaminee, and optional user data and returns the item's value
 * on the criterion.  The @chooser will <emphasis>minimize</emphasis> this
 * criterion.
 */
void oscats_alg_chooser_set_c_criterion(OscatsAlgChooser *chooser,
                                        OscatsAlgChooserCriterion f)
{
  g_return_if_fail(OSCATS_IS_ALG_CHOOSER(chooser) && f != NULL);
  chooser->criterion = f;
}
                                      
/**
 * oscats_alg_chooser_choose:
 * @chooser: an #OscatsAlgChooser with criterion set
 * @e: the #OscatsExaminee for which to choose the item
 * @eligible: a #GBitArray indicating which items in the bank are eligible
 * @data: optional user data for the criterion function
 *
 * Chooses an item that minimizes the given criterion for examinee @e.
 *
 * Returns: the index of the selected item, or -1 if no item is available
 */
gint oscats_alg_chooser_choose(OscatsAlgChooser *chooser,
                               const OscatsExaminee *e,
                               GBitArray *eligible,
                               gpointer data)
{
  GPtrArray *items;
  OscatsItem *item;
  gint i, item_index, num;
  gdouble dist;
  g_return_val_if_fail(OSCATS_IS_ALG_CHOOSER(chooser) &&
                       chooser->criterion != NULL, -1);
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e) && G_IS_BIT_ARRAY(eligible), -1);
  g_return_val_if_fail(oscats_item_bank_num_items(chooser->bank) ==
                       eligible->bit_len, -1);
  g_return_val_if_fail(eligible->num_set > 0, -1);
  
  items = chooser->bank->items;
  num = chooser->num;

  if (num == 1)
  {
    gdouble min;

    g_bit_array_iter_reset(eligible);
    item_index = g_bit_array_iter_next(eligible);
    item = g_ptr_array_index(items, item_index);
    min = (*(chooser->criterion))(item, e, data);

    while((i=g_bit_array_iter_next(eligible)) > 0)
    {
      item = g_ptr_array_index(items, i);
      dist = (*(chooser->criterion))(item, e, data);
      if (dist < min)
      {
        min = dist;
        item_index = i;
      }
    }

    return item_index;
  }

  if (eligible->num_set < num)
  {
    i = oscats_rnd_uniform_int_range(1, eligible->num_set);
    g_bit_array_iter_reset(eligible);
    for (; i; i--) item_index = g_bit_array_iter_next(eligible);
    return item_index;
  }

  // Sort first num items
  g_array_set_size(chooser->dists, 0);
  g_array_set_size(chooser->items, 0);
  g_bit_array_iter_reset(eligible);
  for (i=0; i < num; i++)
  {
    guint j;
    gboolean inserted = FALSE;
    item_index = g_bit_array_iter_next(eligible);
    item = g_ptr_array_index(items, item_index);
    dist = (*(chooser->criterion))(item, e, data);
    for (j=0; j < i; j++)
      if (dist < g_array_index(chooser->dists, gdouble, j))
      {
        g_array_insert_val(chooser->dists, j, dist);
        g_array_insert_val(chooser->items, j, item_index);
        inserted = TRUE;
        break;
      }
    if (!inserted)
    {
      g_array_append_val(chooser->dists, dist);
      g_array_append_val(chooser->items, item_index);
    }
  }
  // Insert any remaining items closer than first num items
  while ((item_index = g_bit_array_iter_next(eligible)) > 0)
  {
    item = g_ptr_array_index(items, item_index);
    dist = (*(chooser->criterion))(item, e, data);
    for (i=0; i < num; i++)
      if (dist < g_array_index(chooser->dists, gdouble, i))
      {
        // Remove last element
        g_array_set_size(chooser->dists, num-1);
        g_array_set_size(chooser->items, num-1);
        // Insert new element
        g_array_insert_val(chooser->dists, i, dist);
        g_array_insert_val(chooser->items, i, item_index);
        break;
      }
  }
  // Select a random item
  i = oscats_rnd_uniform_range(0, num-1);
  return g_array_index(chooser->items, gint, i);
}
