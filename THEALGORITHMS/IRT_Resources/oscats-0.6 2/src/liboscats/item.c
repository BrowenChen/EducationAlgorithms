/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Item Class
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
 * SECTION:item
 * @title:OscatsItem
 * @short_description: Item Container Class
 *
 * Implements all #OscatsAdministrand methods.
 */

#include "item.h"

G_DEFINE_TYPE(OscatsItem, oscats_item, OSCATS_TYPE_ADMINISTRAND);

#define GET_MODEL(I, K) ((K) ? g_datalist_id_get_data(&((I)->models), (K)) : (I)->defaultmodel)

static GQuark defaultKey;

static void oscats_item_dispose (GObject *object);

static void freeze (OscatsAdministrand *item) { }
static gboolean check_type (const OscatsAdministrand *item, GType type);
static gboolean check_model (const OscatsAdministrand *item, GQuark model, GType type);
static gboolean check_dim_type (const OscatsAdministrand *item, GQuark model, OscatsDim type);
static gboolean check_space (const OscatsAdministrand *item, GQuark model, const OscatsSpace *space);
static void set_default_model (OscatsAdministrand *item, GQuark name);
static GQuark get_default_model (const OscatsAdministrand *item);
static void set_model (OscatsAdministrand *item, GQuark name, OscatsModel *model);
static OscatsModel * get_model (const OscatsAdministrand *item, GQuark name);
                   
static void oscats_item_class_init (OscatsItemClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsAdministrandClass *admin_class = OSCATS_ADMINISTRAND_CLASS(klass);

  gobject_class->dispose = oscats_item_dispose;
  
  admin_class->freeze = freeze;
  admin_class->check_type = check_type;
  admin_class->check_model = check_model;
  admin_class->check_dim_type = check_dim_type;
  admin_class->check_space = check_space;
  admin_class->set_default_model = set_default_model;
  admin_class->get_default_model = get_default_model;
  admin_class->set_model = set_model;
  admin_class->get_model = get_model;

  defaultKey = g_quark_from_string("default");
}

static void oscats_item_init (OscatsItem *self)
{
  self->defaultKey = defaultKey;
  g_datalist_init(&(self->models));
}

static void oscats_item_dispose (GObject *object)
{
  OscatsItem *self = OSCATS_ITEM(object);
  G_OBJECT_CLASS(oscats_item_parent_class)->dispose(object);
  g_datalist_clear(&(self->models));
  self->defaultmodel = NULL;
}

static gboolean check_type (const OscatsAdministrand *item, GType type)
{
  return g_type_is_a(G_OBJECT_TYPE(item), type);
}

static gboolean check_model (const OscatsAdministrand *self, GQuark model, GType type)
{
  OscatsItem *item = OSCATS_ITEM(self);
  OscatsModel *m = GET_MODEL(item, model);
  if (m) return g_type_is_a(G_OBJECT_TYPE(m), type);
  else return FALSE;
}

static gboolean check_dim_type (const OscatsAdministrand *self, GQuark model, OscatsDim type)
{
  OscatsItem *item = OSCATS_ITEM(self);
  OscatsModel *m = GET_MODEL(item, model);
  if (m) return (m->dimType == type);
  else return FALSE;
}

static gboolean check_space (const OscatsAdministrand *self, GQuark model, const OscatsSpace *space)
{
  OscatsItem *item = OSCATS_ITEM(self);
  OscatsModel *m = GET_MODEL(item, model);
  if (m) return oscats_space_compatible(m->space, space);
  else return FALSE;
}

static void set_default_model (OscatsAdministrand *self, GQuark name)
{
  OscatsItem *item = OSCATS_ITEM(self);
  if (name == 0) name = defaultKey;
  item->defaultKey = name;
  item->defaultmodel = GET_MODEL(item, name);
}

static GQuark get_default_model (const OscatsAdministrand *self)
{
  return OSCATS_ITEM(self)->defaultKey;
}

static void set_model (OscatsAdministrand *self, GQuark name, OscatsModel *model)
{
  OscatsItem *item = OSCATS_ITEM(self);
  if (name == 0) name = item->defaultKey;
  // Item takes ownership of model
  g_datalist_id_set_data_full(&(item->models), name, model, g_object_unref);
  if (name == item->defaultKey) item->defaultmodel = model;
}

static OscatsModel * get_model (const OscatsAdministrand *self, GQuark name)
{
  return GET_MODEL(OSCATS_ITEM(self), name);
}

/**
 * oscats_item_new:
 * @name: name for default model (as a #GQuark)
 * @model: (transfer full): an #OscatsModel to set as the default model
 *
 * Creates a new #OscatsItem using the supplied @model as the default model. 
 * The default model key is set to @name, or if @name == 0, "default" is
 * used.  Note: the new #OscatsItem takes ownership of @model.
 *
 * Returns: (transfer full): the new #OscatsItem
 */
OscatsItem * oscats_item_new(GQuark name, OscatsModel *model)
{
  OscatsItem *item;
  g_return_val_if_fail(OSCATS_IS_MODEL(model), NULL);
  item = g_object_newv(OSCATS_TYPE_ITEM, 0, NULL);
  if (!item) return NULL;
  if (name) item->defaultKey = name;
  set_model((OscatsAdministrand*)item, name, model);
  return item;
}
