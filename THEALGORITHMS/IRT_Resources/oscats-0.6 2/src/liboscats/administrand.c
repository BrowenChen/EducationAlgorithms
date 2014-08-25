/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Abstract Administrand Class
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
 * SECTION:administrand
 * @title:OscatsAdministrand
 * @short_description: Administrand Container Class
 */

#include "administrand.h"

G_DEFINE_ABSTRACT_TYPE(OscatsAdministrand, oscats_administrand, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_ID,
  PROP_FROZEN,
};

static gboolean static_initialized = FALSE;
static GTree *administrands = NULL;
static GHashTable *quark_to_char = NULL;
static GArray *char_to_quark = NULL;

static gint ptr_compare(gconstpointer a, gconstpointer b) {  return b-a;  }

static void oscats_administrand_dispose (GObject *object);
static void oscats_administrand_finalize (GObject *object);
static void oscats_administrand_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_administrand_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);

static inline void initialize_static()
{
  if (!static_initialized)
  {
    GQuark q = 0;
    administrands = g_tree_new(ptr_compare);
    quark_to_char = g_hash_table_new(g_direct_hash, g_direct_equal);
    char_to_quark = g_array_new(FALSE, FALSE, sizeof(GQuark));
    g_hash_table_insert(quark_to_char, 0, 0);
    g_array_append_val(char_to_quark, q);
    static_initialized = TRUE;
  }
}
                   
static gboolean ret_false() { return FALSE; }
static GQuark ret_zero() { return 0; }
static gpointer ret_null() { return NULL; }
static void null_op() { }

typedef void (*freeze_func) (OscatsAdministrand *item);
typedef gboolean (*check_type_func) (const OscatsAdministrand *item, GType type);
typedef gboolean (*check_model_func) (const OscatsAdministrand *item, GQuark model, GType type);
typedef gboolean (*check_dim_type_func) (const OscatsAdministrand *item, GQuark model, OscatsDim type);
typedef gboolean (*check_space_func) (const OscatsAdministrand *item, GQuark model, const OscatsSpace *space);
typedef void (*set_default_model_func) (OscatsAdministrand *item, GQuark name);
typedef GQuark (*get_default_model_func) (const OscatsAdministrand *item);
typedef void (*set_model_func) (OscatsAdministrand *item, GQuark name, OscatsModel *model);
typedef OscatsModel * (*get_model_func) (const OscatsAdministrand *item, GQuark name);

static void oscats_administrand_class_init (OscatsAdministrandClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;
  
  gobject_class->dispose = oscats_administrand_dispose;
  gobject_class->finalize = oscats_administrand_finalize;
  gobject_class->set_property = oscats_administrand_set_property;
  gobject_class->get_property = oscats_administrand_get_property;
  
  klass->freeze = (freeze_func)NULL;
  klass->unfreeze = (freeze_func)null_op;
  klass->check_type = (check_type_func)ret_false;
  klass->check_model = (check_model_func)ret_false;
  klass->check_dim_type = (check_dim_type_func)ret_false;
  klass->check_space = (check_space_func)ret_false;
  klass->set_default_model = (set_default_model_func)null_op;
  klass->get_default_model = (get_default_model_func)ret_zero;
  klass->set_model = (set_model_func)NULL;
  klass->get_model = (get_model_func)ret_null;

  initialize_static();
  
/**
 * OscatsAdministrand:id:
 *
 * A string identifier for the administrand.
 */
  pspec = g_param_spec_string("id", "ID", 
                              "String identifier for the administrand",
                              NULL,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_ID, pspec);

/**
 * OscatsAdministrand:frozen:
 *
 * Indicates whether changes to the administrand's models are permitted.
 * Once an administrand is added (directly or indirectly) to a test,
 * changes to its list of models are no longer permitted.
 */
  pspec = g_param_spec_boolean("frozen", "models frozen", 
                              "Are changes to administrand's models forbidden?",
                              FALSE,
                              G_PARAM_READABLE |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_FROZEN, pspec);

}

static void oscats_administrand_init (OscatsAdministrand *self)
{
  self->characteristics = g_bit_array_new(g_hash_table_size(quark_to_char));
  g_tree_insert(administrands, self, NULL);
}

static void oscats_administrand_dispose (GObject *object)
{
  OscatsAdministrand *self = OSCATS_ADMINISTRAND(object);
  G_OBJECT_CLASS(oscats_administrand_parent_class)->dispose(object);
  if (self->characteristics) g_object_unref(self->characteristics);
  self->characteristics = NULL;
  g_tree_remove(administrands, self);
}

static void oscats_administrand_finalize (GObject *object)
{
  OscatsAdministrand *self = OSCATS_ADMINISTRAND(object);
  g_free(self->id);
  G_OBJECT_CLASS(oscats_administrand_parent_class)->finalize(object);
}

static void oscats_administrand_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec)
{
  OscatsAdministrand *self = OSCATS_ADMINISTRAND(object);
  GString *id;
  switch (prop_id)
  {
    case PROP_ID:			// construction only
      self->id = g_value_dup_string(value);
      if (!self->id)
      {
        id = g_string_sized_new(18);
        g_string_printf(id, "[%s %p]", G_OBJECT_TYPE_NAME(self), self);
        self->id = id->str;
        g_string_free(id, FALSE);
      }
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_administrand_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
  OscatsAdministrand *self = OSCATS_ADMINISTRAND(object);
  switch (prop_id)
  {
    case PROP_ID:
      g_value_set_string(value, self->id);
      break;
    
    case PROP_FROZEN:
      g_value_set_boolean(value, self->freeze_count > 0);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static gboolean kill_characteristics(gpointer key, gpointer val, gpointer data)
{
  OscatsAdministrand *administrand = (OscatsAdministrand*)key;
  g_bit_array_resize(administrand->characteristics, 1);
  return FALSE;
}

static gboolean add_characteristic(gpointer key, gpointer val, gpointer data)
{
  OscatsAdministrand *administrand = (OscatsAdministrand*)key;
  g_bit_array_extend(administrand->characteristics, 1);
  return FALSE;
}

/**
 * oscats_administrand_freeze:
 * @item: an #OscatsAdministrand
 *
 * When an administrand is added to a test (directly or indirectly), changes
 * to its list of models (including which model is set as the default) are
 * no longer permitted.  When an #OscatsItemBank is added to an #OscatsTest,
 * the test calls oscats_administrand_freeze(), and all implementations of
 * #OscatsAdministrand <emphasis>must</emphasis> provide
 * #OscatsAdministrandClass.freeze to call oscats_administrand_freeze()
 * on all sub-administrands.
 */
void oscats_administrand_freeze(OscatsAdministrand *item)
{
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(item));
  item->freeze_count++;
  if (OSCATS_ADMINISTRAND_GET_CLASS(item)->freeze)
    OSCATS_ADMINISTRAND_GET_CLASS(item)->freeze(item);
  else
    g_critical("%s should provide oscats_administrand_freeze()!", G_OBJECT_TYPE_NAME(item));
}

/**
 * oscats_administrand_unfreeze:
 * @item: an #OscatsAdministrand
 *
 * When an #OscatsTest is disposed, it calls oscats_administrand_unfreeze()
 * to indicate that its administrands are no longer required not to allow
 * changes to their models.  If an administrand is added to multiple tests,
 * all of the tests must be disposed before changes are again permitted.
 * Implementations of #OscatsAdministrand with sub-administrands
 * <emphasis>should</emphasis> provide #OscatsAdministrandClass.unfreeze to
 * call oscats_administrand_unfreeze() on all sub-administrands.
 */
void oscats_administrand_unfreeze(OscatsAdministrand *item)
{
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(item));
  if (item->freeze_count > 0) item->freeze_count--;
  OSCATS_ADMINISTRAND_GET_CLASS(item)->unfreeze(item);
}

/**
 * oscats_administrand_reset_characteristics:
 *
 * Removes all characteristics from the internal characteristics table.
 * (Useful for more compact memory management if you shift from
 * one large set of characteristics to another in the same program.)
 */
void oscats_administrand_reset_characteristics()
{
  if (static_initialized)
  {
    g_hash_table_remove_all(quark_to_char);
    g_hash_table_insert(quark_to_char, 0, 0);
    g_array_set_size(char_to_quark, 1);
    g_tree_foreach(administrands, kill_characteristics, NULL);
  }
}

/**
 * oscats_administrand_register_characteristic:
 * @characteristic: the #GQuark characteristic to register
 *
 * Adds @characteristic to the internal characteristics table.  It is more
 * efficient to register all characteristics before creating administrands
 * than to add new characteristics after administrands already exist.
 */
void oscats_administrand_register_characteristic(GQuark characteristic)
{
  int c = (static_initialized ? g_hash_table_size(quark_to_char) : 1);
  initialize_static();
  g_hash_table_insert(quark_to_char, GUINT_TO_POINTER(characteristic),
                      GUINT_TO_POINTER(c));
  g_array_append_val(char_to_quark, characteristic);
  g_tree_foreach(administrands, add_characteristic, NULL);
}

/**
 * oscats_administrand_characteristic_from_string:
 * @name: the string name of the characteristic
 *
 * A wrapper of g_quark_from_string() for language bindings.
 *
 * Returns: the characteristic as a #GQuark
 */
GQuark oscats_administrand_characteristic_from_string(const gchar *name)
{
  return g_quark_from_string(name);
}

/**
 * oscats_administrand_characteristic_as_string:
 * @characteristic: a #GQuark characteristic
 *
 * A wrapper of g_quark_to_string() for language bindings.
 *
 * Returns: the string form of @characteristic
 */
const gchar * oscats_administrand_characteristic_as_string(GQuark characteristic)
{
  return g_quark_to_string(characteristic);
}

/**
 * oscats_administrand_set_characteristic:
 * @administrand: an #OscatsAdministrand
 * @characteristic: a #GQuark characteristic
 *
 * Indicate that @administrand has @characteristic.  (This will register
 * the characteristic if it has not been already.)
 */
void oscats_administrand_set_characteristic(OscatsAdministrand *administrand, GQuark characteristic)
{
  guint c;
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(administrand));
  c = GPOINTER_TO_UINT(g_hash_table_lookup(quark_to_char,
                                           GUINT_TO_POINTER(characteristic)));
  if (c == 0)
  {
    c = g_hash_table_size(quark_to_char);
    oscats_administrand_register_characteristic(characteristic);
  }
  g_bit_array_set_bit(administrand->characteristics, c);
}

/**
 * oscats_administrand_clear_characteristic:
 * @administrand: an #OscatsAdministrand
 * @characteristic: a #GQuark characteristic
 *
 * Indicate that @administrand does not have @characteristic.
 */
void oscats_administrand_clear_characteristic(OscatsAdministrand *administrand, GQuark characteristic)
{
  guint c;
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(administrand));
  c = GPOINTER_TO_UINT(g_hash_table_lookup(quark_to_char,
                                           GUINT_TO_POINTER(characteristic)));
  if (c) g_bit_array_clear_bit(administrand->characteristics, c);
}

/**
 * oscats_administrand_clear_characteristics:
 * @administrand: an #OscatsAdministrand
 *
 * Clear all characteristics for @administrand.
 */
void oscats_administrand_clear_characteristics(OscatsAdministrand *administrand)
{
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(administrand));
  g_bit_array_reset(administrand->characteristics, FALSE);
}

/**
 * oscats_administrand_has_characteristic:
 * @administrand: an #OscatsAdministrand
 * @characteristic: a #GQuark characteristic
 *
 * Returns: %TRUE if @administrand has @characteristic
 */
gboolean oscats_administrand_has_characteristic(OscatsAdministrand *administrand, GQuark characteristic)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(administrand), FALSE);
  return g_bit_array_get_bit(administrand->characteristics,
    GPOINTER_TO_UINT(g_hash_table_lookup(quark_to_char,
                                         GUINT_TO_POINTER(characteristic))));
}

/**
 * oscats_administrand_characteristics_iter_reset
 * @administrand: an #OscatsAdministrand
 *
 * Reset the characteristics iterator for @administrand.
 */
void oscats_administrand_characteristics_iter_reset(OscatsAdministrand *administrand)
{
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(administrand));
  g_bit_array_iter_reset(administrand->characteristics);
}

/**
 * oscats_administrand_characteristics_iter_next
 * @administrand: an #OscatsAdministrand
 *
 * Returns: the next #GQuark characteristic that @administrand has, or 0 if @administrand
 * has no more characteristics
 */
GQuark oscats_administrand_characteristics_iter_next(OscatsAdministrand *administrand)
{
  gint index;
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(administrand), 0);
  index = g_bit_array_iter_next(administrand->characteristics);
  return (index < 0 ? 0 : g_array_index(char_to_quark, GQuark, index) );
}

/**
 * oscats_administrand_check_type:
 * @item: an #OscatsAdministrand
 * @type: a sub-class of #OscatsAdministrand
 *
 * Returns: %TRUE if all sub-administrands in @item are inherited from @type
 */
gboolean oscats_administrand_check_type (const OscatsAdministrand *item, GType type)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(item), FALSE);
  g_return_val_if_fail(g_type_is_a(type, OSCATS_TYPE_ADMINISTRAND), FALSE);
  return OSCATS_ADMINISTRAND_GET_CLASS(item)->check_type(item, type);
}

/**
 * oscats_administrand_check_model:
 * @item: an #OscatsAdministrand
 * @model: a #GQuark identifier for a model name
 * @type: a sub-class of #OscatsModel
 *
 * Checks whether the administrand (or all sub-administrands) have a model
 * @model and if @model inherits from @type.  Use @model = 0 to check the
 * default model.
 *
 * Returns: %TRUE if @item has @model and @model inherits from @type
 */
gboolean oscats_administrand_check_model (const OscatsAdministrand *item, GQuark model, GType type)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(item), FALSE);
  g_return_val_if_fail(g_type_is_a(type, OSCATS_TYPE_MODEL), FALSE);
  return OSCATS_ADMINISTRAND_GET_CLASS(item)->check_model(item, model, type);
}

/**
 * oscats_administrand_check_dim_type:
 * @item: an #OscatsAdministrand
 * @model: a #GQuark identifier for a model name
 * @type: a dimension type (see #OscatsSpace)
 *
 * Checks whether the administrand (or all sub-administrands) have a model
 * @model and if the subspace for @model is of @type.  Use @model = 0 to
 * check the default model.
 *
 * Returns: %TRUE if @item has @model and the subspace of @model is @type
 */
gboolean oscats_administrand_check_dim_type (const OscatsAdministrand *item, GQuark model, OscatsDim type)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(item), FALSE);
  return OSCATS_ADMINISTRAND_GET_CLASS(item)->check_dim_type(item, model, type);
}

/**
 * oscats_administrand_check_space:
 * @item: an #OscatsAdministrand
 * @model: a #GQuark identifier for a model name
 * @space: an #OscatsSpace
 *
 * Checks whether the administrand (or all sub-administrands) have a model
 * @model and if the space for @model is compatible with @space.  Use @model
 * = 0 to check the default model.
 *
 * Returns: %TRUE if @item has @model and the space of @model is compatible
 * with @space
 */
gboolean oscats_administrand_check_space (const OscatsAdministrand *item, GQuark model, const OscatsSpace *space)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(item), FALSE);
  g_return_val_if_fail(OSCATS_IS_SPACE(space), FALSE);
  return OSCATS_ADMINISTRAND_GET_CLASS(item)->check_space(item, model, space);
}

/**
 * oscats_administrand_set_default_model:
 * @item: an #OscatsAdministrand
 * @name: the name of a model (as a #GQuark)
 *
 * Set the model @name as the default for @item.  Note, an administrand's
 * models may not be changed if it is frozen.  See
 * #OscatsAdministrand:frozen and oscats_administrand_freeze().
 */
void oscats_administrand_set_default_model (OscatsAdministrand *item, GQuark name)
{
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(item));
  g_return_if_fail(item->freeze_count == 0);
  OSCATS_ADMINISTRAND_GET_CLASS(item)->set_default_model(item, name);
}

/**
 * oscats_administrand_get_default_model:
 * @item: an #OscatsAdministrand
 *
 * Get which model name is set as the default for this administrand. 
 * Generally, an administrand's default model will be "default" by default,
 * but this may be implementation-specific.
 *
 * This function returns the key for the default model.  To actually
 * retrieve the #OscatsModel itself, use oscats_administrand_get_model().
 *
 * Returns: the model name for the default model (as a #GQuark)
 */
GQuark oscats_administrand_get_default_model (const OscatsAdministrand *item)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(item), 0);
  return OSCATS_ADMINISTRAND_GET_CLASS(item)->get_default_model(item);
}

/**
 * oscats_administrand_set_model:
 * @item: an #OscatsAdministrand
 * @name: the name of the model (as a #GQuark)
 * @model: (transfer full): the #OscatsModel to set
 *
 * Set the model @model as @name for @item.  If @item already has a model
 * @name, it is replaced with @model.  Note, @item takes ownership of @model.
 *
 * The @name may be 0 to indicate the default model.
 *
 * Note, an administrand's models may not be changed if it is frozen.  See
 * #OscatsAdministrand:frozen and oscats_administrand_freeze().
 *
 * Generally, model access is fastest for the first models added, so models
 * should be set in order of decreasing expected access frequency. 
 * Accessing the default model (by supplying the #GQuark 0) is fastest,
 * though.
 */
void oscats_administrand_set_model (OscatsAdministrand *item, GQuark name, OscatsModel *model)
{
  g_return_if_fail(OSCATS_IS_ADMINISTRAND(item));
  g_return_if_fail(OSCATS_IS_MODEL(model));
  g_return_if_fail(item->freeze_count == 0);
  if (OSCATS_ADMINISTRAND_GET_CLASS(item)->set_model)
    OSCATS_ADMINISTRAND_GET_CLASS(item)->set_model(item, name, model);
  else
    g_critical("%s does not implement oscats_administrand_set_model()!", G_OBJECT_TYPE_NAME(item));
}

/**
 * oscats_administrand_set_model_by_name:
 * @item: an #OscatsAdministrand
 * @name: the name of the model
 * @model: (transfer full): the #OscatsModel to set
 *
 * Convenience wrapper for oscats_administrand_set_model().
 */
void oscats_administrand_set_model_by_name (OscatsAdministrand *item, const gchar *name, OscatsModel *model)
{
  oscats_administrand_set_model(item, g_quark_from_string(name), model);
}

/**
 * oscats_administrand_get_model:
 * @item: an #OscatsAdministrand
 * @name: the name of the model (as a #GQuark)
 *
 * Get the model @model as @name for @item.  The @name may be 0 to indicate
 * the default model.
 *
 * Returns: (transfer none): the model @name for @item, or %NULL if @item
 * does not have a model @name
 */
OscatsModel * oscats_administrand_get_model (const OscatsAdministrand *item, GQuark name)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(item), NULL);
  return OSCATS_ADMINISTRAND_GET_CLASS(item)->get_model(item, name);
}

/**
 * oscats_administrand_get_model_by_name:
 * @item: an #OscatsAdministrand
 * @name: the name of the model
 *
 * Convenience wrapper for oscats_administrand_get_model().
 *
 * Returns: (transfer none): the model @name for @item, or %NULL if @item
 * does not have a model @name
 */
OscatsModel * oscats_administrand_get_model_by_name (const OscatsAdministrand *item, const gchar *name)
{
  g_return_val_if_fail(OSCATS_IS_ADMINISTRAND(item), NULL);
  return OSCATS_ADMINISTRAND_GET_CLASS(item)->get_model(item, g_quark_from_string(name));
}

