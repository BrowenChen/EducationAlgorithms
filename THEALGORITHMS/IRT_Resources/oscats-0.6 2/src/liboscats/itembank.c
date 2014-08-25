/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Item Bank Class
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
 * SECTION:itembank
 * @title:OscatsItemBank
 * @short_description: Item Bank Class
 *
 * Note: Since an #OscatsItemBank is also an #OscatsAdministrand, it can be
 * tagged with characteristics and can also serve as an item group for
 * multi-stage testing or testlets, with appropriately coded algorithms.
 *
 * #OscatsItemBank does not implement oscats_administrand_get_default_model(),
 * oscats_administrand_set_model(), or oscats_administrand_get_model().  For
 * oscats_administrand_set_default_model(), #OscatsItemBank will set the
 * default model for all items.
 */

#include "itembank.h"
#include "item.h"

G_DEFINE_TYPE(OscatsItemBank, oscats_item_bank, OSCATS_TYPE_ADMINISTRAND);

enum
{
  PROP_0,
  PROP_SIZE_HINT,
};

static void oscats_item_bank_dispose (GObject *object);
static void oscats_item_bank_finalize (GObject *object);
static void oscats_item_bank_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_item_bank_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);
                   
static void freeze (OscatsAdministrand *item);
static void unfreeze (OscatsAdministrand *item);
static gboolean check_type (const OscatsAdministrand *item, GType type);
static gboolean check_model (const OscatsAdministrand *item, GQuark model, GType type);
static gboolean check_dim_type (const OscatsAdministrand *item, GQuark model, OscatsDim type);
static gboolean check_space (const OscatsAdministrand *item, GQuark model, const OscatsSpace *space);
static void set_default_model (OscatsAdministrand *item, GQuark name);

static void oscats_item_bank_class_init (OscatsItemBankClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsAdministrandClass *admin_class = OSCATS_ADMINISTRAND_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_item_bank_dispose;
  gobject_class->finalize = oscats_item_bank_finalize;
  gobject_class->set_property = oscats_item_bank_set_property;
  gobject_class->get_property = oscats_item_bank_get_property;
  
  admin_class->freeze = freeze;
  admin_class->unfreeze = unfreeze;
  admin_class->check_type = check_type;
  admin_class->check_model = check_model;
  admin_class->check_dim_type = check_dim_type;
  admin_class->check_space = check_space;
  admin_class->set_default_model = set_default_model;

/**
 * OscatsItemBank:sizeHint:
 *
 * Hint of how many items will be in the item bank.
 */
  pspec = g_param_spec_uint("sizeHint", "Size Hint", 
                            "Hint of how many items will be in the bank",
                            0, G_MAXUINT, 1,
                            G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_SIZE_HINT, pspec);

}

static void oscats_item_bank_init (OscatsItemBank *self)
{
}

static void oscats_item_bank_dispose (GObject *object)
{
  OscatsItemBank *self = OSCATS_ITEM_BANK(object);
  G_OBJECT_CLASS(oscats_item_bank_parent_class)->dispose(object);
  g_ptr_array_set_size(self->items, 0);
}

static void oscats_item_bank_finalize (GObject *object)
{
  OscatsItemBank *self = OSCATS_ITEM_BANK(object);
  g_ptr_array_free(self->items, TRUE);
  G_OBJECT_CLASS(oscats_item_bank_parent_class)->finalize(object);
}

static void oscats_item_bank_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec)
{
  OscatsItemBank *self = OSCATS_ITEM_BANK(object);
  switch (prop_id)
  {
    case PROP_SIZE_HINT:		// construction only
      self->items = g_ptr_array_sized_new(g_value_get_uint(value));
      g_ptr_array_set_free_func(self->items, g_object_unref);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_item_bank_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
//  OscatsItemBank *self = OSCATS_ITEM_BANK(object);
  switch (prop_id)
  {
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void freeze (OscatsAdministrand *self)
{
  OscatsItemBank *bank = OSCATS_ITEM_BANK(self);
  gpointer *items = bank->items->pdata;
  guint i, num = bank->items->len;
  for (i=0; i < num; i++)
    oscats_administrand_freeze(items[i]);
}

static void unfreeze (OscatsAdministrand *self)
{
  OscatsItemBank *bank = OSCATS_ITEM_BANK(self);
  gpointer *items = bank->items->pdata;
  guint i, num = bank->items->len;
  for (i=0; i < num; i++)
    oscats_administrand_unfreeze(items[i]);
}

static gboolean check_type (const OscatsAdministrand *self, GType type)
{
  OscatsItemBank *bank = OSCATS_ITEM_BANK(self);
  gpointer *items = bank->items->pdata;
  guint i, num = bank->items->len;
  for (i=0; i < num; i++)
    if (!oscats_administrand_check_type(items[i], type))
      return FALSE;
  return TRUE;
}

static gboolean check_model (const OscatsAdministrand *self, GQuark model, GType type)
{
  OscatsItemBank *bank = OSCATS_ITEM_BANK(self);
  gpointer *items = bank->items->pdata;
  guint i, num = bank->items->len;
  for (i=0; i < num; i++)
    if (!oscats_administrand_check_model(items[i], model, type))
      return FALSE;
  return TRUE;
}

static gboolean check_dim_type (const OscatsAdministrand *self, GQuark model, OscatsDim type)
{
  OscatsItemBank *bank = OSCATS_ITEM_BANK(self);
  gpointer *items = bank->items->pdata;
  guint i, num = bank->items->len;
  for (i=0; i < num; i++)
    if (!oscats_administrand_check_dim_type(items[i], model, type))
      return FALSE;
  return TRUE;
}

static gboolean check_space (const OscatsAdministrand *self, GQuark model, const OscatsSpace *space)
{
  OscatsItemBank *bank = OSCATS_ITEM_BANK(self);
  gpointer *items = bank->items->pdata;
  guint i, num = bank->items->len;
  for (i=0; i < num; i++)
    if (!oscats_administrand_check_space(items[i], model, space))
      return FALSE;
  return TRUE;
}

static void set_default_model (OscatsAdministrand *self, GQuark name)
{
  OscatsItemBank *bank = OSCATS_ITEM_BANK(self);
  gpointer *items = bank->items->pdata;
  guint i, num = bank->items->len;
  for (i=0; i < num; i++)
    oscats_administrand_set_default_model(items[i], name);
}

/**
 * oscats_item_bank_add_item:
 * @bank: an #OscatsItemBank
 * @item: (transfer none): an #OscatsAdministrand
 *
 * Adds @item to the item bank @bank.  (Increases the @item reference count.)
 * Note that items cannot be added if @bank is frozen
 * (see #OscatsAdministrand:frozen).  Note, this function does not check
 * whether @item is already in @bank.
 */
void oscats_item_bank_add_item(OscatsItemBank *bank, OscatsAdministrand *item)
{
  g_return_if_fail(OSCATS_IS_ITEM_BANK(bank) && OSCATS_IS_ADMINISTRAND(item));
  g_return_if_fail(OSCATS_ADMINISTRAND(bank)->freeze_count == 0);
  g_ptr_array_add(bank->items, item);
  g_object_ref(item);
}

/**
 * oscats_item_bank_remove_item:
 * @bank: an #OscatsItemBank
 * @item: (transfer none): an #OscatsAdministrand
 *
 * Removes @item from the item bank @bank.  (Decreases the @item reference
 * count.) Note that items cannot be removed if @bank is frozen (see
 * #OscatsAdministrand:frozen).  If @item is not in @bank, the function
 * does nothing.
 */
void oscats_item_bank_remove_item(OscatsItemBank *bank, OscatsAdministrand *item)
{
  g_return_if_fail(OSCATS_IS_ITEM_BANK(bank) && OSCATS_IS_ADMINISTRAND(item));
  g_return_if_fail(OSCATS_ADMINISTRAND(bank)->freeze_count == 0);
  g_ptr_array_remove(bank->items, item);
}

/**
 * oscats_item_bank_num_items:
 * @bank: an #OscatsItemBank
 *
 * Returns: the number of items in the @bank
 */
guint oscats_item_bank_num_items(const OscatsItemBank *bank)
{
  g_return_val_if_fail(OSCATS_IS_ITEM_BANK(bank) && bank->items, 0);
  return bank->items->len;
}

/**
 * oscats_item_bank_get_item:
 * @bank: an #OscatsItemBank
 * @i: item index (0-based)
 *
 * Must have @i < number of items in the bank.
 * (Item's reference count is not increased.)
 *
 * Returns: (transfer none): the item @i
 */
const OscatsAdministrand * oscats_item_bank_get_item(const OscatsItemBank *bank, guint i)
{
  g_return_val_if_fail(OSCATS_IS_ITEM_BANK(bank) && bank->items &&
                       i < bank->items->len, NULL);
  return g_ptr_array_index(bank->items, i);
}
