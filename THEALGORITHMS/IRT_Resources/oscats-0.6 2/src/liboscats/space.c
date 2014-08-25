/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Latent Space Class
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

/**
 * SECTION:space
 * @title:OscatsSpace
 * @short_description: Latent Space Definition
 *
 * A multidimensional latent space consisting of continuos-, binary- (0, 1),
 * and natural- (0, 1, 2, ...) valued dimensions.
 */

#include "space.h"

GType oscats_dim_type_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GFlagsValue values[] = {
      { OSCATS_DIM_CONT, "OSCATS_DIM_CONT", "cont" },
      { OSCATS_DIM_BIN, "OSCATS_DIM_BIN", "bin" },
      { OSCATS_DIM_NAT, "OSCATS_DIM_NAT", "nat" },
      { OSCATS_DIM_TYPE_MASK, "OSCATS_DIM_TYPE_MASK", "type-mask" },
      { OSCATS_DIM_MASK, "OSCATS_DIM_MASK", "mask" },
      { OSCATS_DIM_MAX, "OSCATS_DIM_MAX", "max" },
      { 0, NULL, NULL }
    };
    etype = g_flags_register_static ("OscatsDimType", values);
  }
  return etype;
}

G_DEFINE_TYPE(OscatsSpace, oscats_space, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_ID,
  PROP_NUM_CONT,
  PROP_NUM_BIN,
  PROP_NUM_NAT,
  PROP_NAMES,
  PROP_MAX,
};

static void oscats_space_constructed (GObject *object);
static void oscats_space_finalize (GObject *object);
static void oscats_space_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_space_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);

static void oscats_space_class_init (OscatsSpaceClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec, *espec;

  gobject_class->constructed = oscats_space_constructed;
  gobject_class->finalize = oscats_space_finalize;
  gobject_class->set_property = oscats_space_set_property;
  gobject_class->get_property = oscats_space_get_property;
  
/**
 * OscatsSpace:id:
 *
 * A string identifier for the space.
 */
  pspec = g_param_spec_string("id", "ID", 
                            "String identifier for the space",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_ID, pspec);

/**
 * OscatsSpace:numCont:
 *
 * The number of continuous dimensions in the space.
 */
  pspec = g_param_spec_uint("numCont", "numCont", 
                            "Number of continuous dimensions",
                            0, OSCATS_DIM_MAX, 0,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM_CONT, pspec);

/**
 * OscatsSpace:numBin:
 *
 * The number of binary dimensions in the space.
 */
  pspec = g_param_spec_uint("numBin", "numBin", 
                            "Number of binary dimensions",
                            0, OSCATS_DIM_MAX, 0,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM_BIN, pspec);

/**
 * OscatsSpace:numNat:
 *
 * The number of natural (0, 1, 2, ...) dimensions in the space.
 */
  pspec = g_param_spec_uint("numNat", "numNat", 
                            "Number of natural dimensions",
                            0, OSCATS_DIM_MAX, 0,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM_NAT, pspec);

/**
 * OscatsSpace:names:
 *
 * Dimension names. Names are ordered continuous dimensions first, followed
 * by binary dimensions, followed by natural dimensions.  Default names
 * are Cont.1, Cont.2, ..., Bin.1, Bin.2, ..., Nat.1, Nat.2, ....
 */
  espec = g_param_spec_string("dimName", "dim name", "A dimension name",
                            NULL,
                            G_PARAM_READWRITE | 
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  pspec = g_param_spec_value_array("names", "dim names", "Dimension names",
                            espec,
                            G_PARAM_READWRITE | 
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NAMES, pspec);

/**
 * OscatsSpace:max:
 *
 * The maximum for each natural dimension.  May be 1, though binary
 * dimensions should probably use %OSCATS_DIM_BIN instead.
 */
  espec = g_param_spec_uint("dimMax", "dim max",
                            "Maximum for a natural dimension",
                            1, G_MAXUINT16, 2,
                            G_PARAM_READWRITE | 
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  pspec = g_param_spec_value_array("max", "dim maxima",
                            "Maxmia for natural dimensions",
                            espec,
                            G_PARAM_READWRITE | 
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_MAX, pspec);

}

static void oscats_space_init (OscatsSpace *self)
{
}

static void oscats_space_constructed (GObject *object)
{
  OscatsSpace *self = OSCATS_SPACE(object);
  OscatsDim I;
  GString *str;
  guint i;
//  G_OBJECT_CLASS(oscats_space_parent_class)->constructed(object);

  if (self->num_cont + self->num_bin + self->num_nat == 0)
    g_warning("OscatsSpace %s has 0 size!", self->id);
  if (self->names == NULL)
    self->names = g_hash_table_new(g_direct_hash, g_direct_equal);

  // Initialize dim names
  self->names_cont = g_new(GQuark, self->num_cont + self->num_bin + self->num_nat);
  self->names_bin = self->names_cont + self->num_cont;
  self->names_nat = self->names_bin + self->num_bin;
  str = g_string_new(NULL);
  for (i=0, I=OSCATS_DIM_CONT; i < self->num_cont; i++, I++)
  {
    g_string_printf(str, "Cont.%d", i+1);
    oscats_space_set_dim_name(self, I, str->str);
  }
  for (i=0, I=OSCATS_DIM_BIN; i < self->num_bin; i++, I++)
  {
    g_string_printf(str, "Bin.%d", i+1);
    oscats_space_set_dim_name(self, I, str->str);
  }
  for (i=0, I=OSCATS_DIM_NAT; i < self->num_nat; i++, I++)
  {
    g_string_printf(str, "Nat.%d", i+1);
    oscats_space_set_dim_name(self, I, str->str);
  }
  g_string_free(str, TRUE);

  // Initialize maxima
  if (self->num_nat > 0)
  {
    self->max = g_new(guint16, self->num_nat);
    for (i=0; i < self->num_nat; i++)
      self->max[i] = 2;
  }

}

static void oscats_space_finalize (GObject *object)
{
  OscatsSpace *self = OSCATS_SPACE(object);
  if (self->id) g_free(self->id);
  if (self->names) g_hash_table_unref(self->names);
  if (self->names_cont) g_free(self->names_cont);
  if (self->max) g_free(self->max);
  self->id = NULL;
  self->names = NULL;
  self->names_cont = self->names_bin = self->names_nat = NULL;
  self->max = NULL;
  G_OBJECT_CLASS(oscats_space_parent_class)->finalize(object);
}

static void oscats_space_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec)
{
  OscatsSpace *self = OSCATS_SPACE(object);
  OscatsDim I;
  GString *id;
  GValueArray *array;
  guint i, j;
  
  switch (prop_id)
  {
    case PROP_ID:			// construction only
      self->id = g_value_dup_string(value);
      if (!self->id)
      {
        id = g_string_sized_new(18);
        g_string_printf(id, "[Space %p]", self);
        self->id = id->str;
        g_string_free(id, FALSE);
      }
      break;
    
    case PROP_NUM_CONT:			// construction only
      self->num_cont = g_value_get_uint(value);
      break;
    
    case PROP_NUM_BIN:			// construction only
      self->num_bin = g_value_get_uint(value);
      break;
    
    case PROP_NUM_NAT:			// construction only
      self->num_nat = g_value_get_uint(value);
      break;
    
    case PROP_NAMES:
      array = g_value_get_boxed(value);
      if (!array) return;
      g_return_if_fail(array->n_values == self->num_cont + self->num_bin + self->num_nat);
      j=0;
      for (i=0, I=OSCATS_DIM_CONT; i < self->num_cont; i++, I++, j++)
        oscats_space_set_dim_name(self, I,
                                  g_value_get_string(array->values+j));
      for (i=0, I=OSCATS_DIM_BIN; i < self->num_bin; i++, I++, j++)
        oscats_space_set_dim_name(self, I,
                                  g_value_get_string(array->values+j));
      for (i=0, I=OSCATS_DIM_NAT; i < self->num_nat; i++, I++, j++)
        oscats_space_set_dim_name(self, I,
                                  g_value_get_string(array->values+j));
      break;
    
    case PROP_MAX:
      array = g_value_get_boxed(value);
      if (!array) return;
      g_return_if_fail(array->n_values == self->num_nat);
      for (i=0; i < self->num_nat; i++)
        self->max[i] = g_value_get_uint(array->values+i);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_space_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
  OscatsSpace *self = OSCATS_SPACE(object);
  GValueArray *array;
  GValue v = { 0, };
  guint i, num;
  
  switch (prop_id)
  {
    case PROP_ID:
      g_value_set_string(value, self->id);
      break;
    
    case PROP_NUM_CONT:
      g_value_set_uint(value, self->num_cont);
      break;
    
    case PROP_NUM_BIN:
      g_value_set_uint(value, self->num_bin);
      break;
    
    case PROP_NUM_NAT:
      g_value_set_uint(value, self->num_nat);
      break;
    
    case PROP_NAMES:
      num = self->num_cont + self->num_bin + self->num_nat;
      array = g_value_array_new(num);
      g_value_init(&v, G_TYPE_STRING);
      for (i=0; i < num; i++)
      {
        g_value_set_static_string(&v, g_quark_to_string(self->names_cont[i]));
        g_value_array_append(array, &v);
      }
      g_value_take_boxed(value, array);
      break;
    
    case PROP_MAX:
      num = self->num_nat;
      array = g_value_array_new(num);
      g_value_init(&v, G_TYPE_UINT);
      for (i=0; i < num; i++)
      {
        g_value_set_uint(&v, self->max[i]);
        g_value_array_append(array, &v);
      }
      g_value_take_boxed(value, array);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/**
 * oscats_space_size:
 * @space: an #OscatsSpace
 *
 * Returns: the total number of dimensions
 */
guint16 oscats_space_size(const OscatsSpace *space)
{
  g_return_val_if_fail(OSCATS_IS_SPACE(space), 0);
  return space->num_cont + space->num_bin + space->num_nat;
}

/**
 * oscats_space_set_dim_name:
 * @space: an #OscatsSpace
 * @dim: the dimension to set
 * @name: (transfer none): the new name for the dimension
 *
 * Sets the name of @dim to @name.  The @dim must be valid.
 */
void oscats_space_set_dim_name(OscatsSpace *space, OscatsDim dim, const gchar *name)
{
  guint16 i = dim & OSCATS_DIM_MASK;
  g_return_if_fail(OSCATS_IS_SPACE(space));
  switch (dim & OSCATS_DIM_TYPE_MASK)
  {
    case OSCATS_DIM_CONT:
      g_return_if_fail(i < space->num_cont);
      g_hash_table_remove(space->names, GUINT_TO_POINTER(space->names_cont[i]));
      space->names_cont[i] = g_quark_from_string(name);
      g_hash_table_insert(space->names, GUINT_TO_POINTER(space->names_cont[i]),
                          GUINT_TO_POINTER((guint)dim));
      break;
    
    case OSCATS_DIM_BIN:
      g_return_if_fail(i < space->num_bin);
      g_hash_table_remove(space->names, GUINT_TO_POINTER(space->names_bin[i]));
      space->names_bin[i] = g_quark_from_string(name);
      g_hash_table_insert(space->names, GUINT_TO_POINTER(space->names_bin[i]),
                          GUINT_TO_POINTER((guint)dim));
      break;
    
    case OSCATS_DIM_NAT:
      g_return_if_fail(i < space->num_nat);
      g_hash_table_remove(space->names, GUINT_TO_POINTER(space->names_nat[i]));
      space->names_nat[i] = g_quark_from_string(name);
      g_hash_table_insert(space->names, GUINT_TO_POINTER(space->names_nat[i]),
                          GUINT_TO_POINTER((guint)dim));
      break;
    
    default:  // Invalid dimension
      g_return_if_reached();
  }
}

/**
 * oscats_space_has_dim:
 * @space: an #OscatsSpace
 * @name: the #GQuark for a potential dimension name
 *
 * Returns: %TRUE if @space has a dimension named @name
 */
gboolean oscats_space_has_dim(const OscatsSpace *space, GQuark name)
{
  g_return_val_if_fail(OSCATS_IS_SPACE(space), FALSE);
  return g_hash_table_lookup(space->names, GUINT_TO_POINTER(name)) != NULL;
}

/**
 * oscats_space_has_dim_name:
 * @space: an #OscatsSpace
 * @name: a potential dimension name
 *
 * Returns: %TRUE if @space has a dimension named @name
 */
gboolean oscats_space_has_dim_name(const OscatsSpace *space, const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  g_return_val_if_fail(OSCATS_IS_SPACE(space), FALSE);
  if (q) return g_hash_table_lookup(space->names, GUINT_TO_POINTER(q)) != NULL;
  else return FALSE;
}

/**
 * oscats_space_get_dim:
 * @space: an #OscatsSpace
 * @name: the #GQuark for a potential dimension name
 *
 * The @space must have a dimension named @name.
 *
 * Returns: the #OscatsDim for the dimension named @name
 */
OscatsDim oscats_space_get_dim(const OscatsSpace *space, GQuark name)
{
  g_return_val_if_fail(OSCATS_IS_SPACE(space), 0);
  return GPOINTER_TO_UINT(g_hash_table_lookup(space->names,
                                              GUINT_TO_POINTER(name)));
}

/**
 * oscats_space_get_dim_by_name:
 * @space: an #OscatsSpace
 * @name: a potential dimension name
 *
 * The @space must have a dimension named @name.
 *
 * Returns: the #OscatsDim for the dimension named @name
 */
OscatsDim oscats_space_get_dim_by_name(const OscatsSpace *space, const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  g_return_val_if_fail(OSCATS_IS_SPACE(space), 0);
  return GPOINTER_TO_UINT(g_hash_table_lookup(space->names,
                                              GUINT_TO_POINTER(q)));
}

/**
 * oscats_space_dim_get_name:
 * @space: an #OscatsSpace
 * @dim: an #OscatsDim indicating the dimension
 *
 * The @dim must be a valid dimension for @space.
 *
 * Returns: (transfer none): the name for @dim in @space
 */
const gchar * oscats_space_dim_get_name(const OscatsSpace *space, OscatsDim dim)
{
  guint16 i = dim & OSCATS_DIM_MASK;
  g_return_val_if_fail(OSCATS_IS_SPACE(space), NULL);
  switch (dim & OSCATS_DIM_TYPE_MASK)
  {
    case OSCATS_DIM_CONT:
      g_return_val_if_fail(i < space->num_cont, NULL);
      return g_quark_to_string(space->names_cont[i]);
    
    case OSCATS_DIM_BIN:
      g_return_val_if_fail(i < space->num_bin, NULL);
      return g_quark_to_string(space->names_bin[i]);
    
    case OSCATS_DIM_NAT:
      g_return_val_if_fail(i < space->num_nat, NULL);
      return g_quark_to_string(space->names_nat[i]);
    
    default:  // Invalid dimension
      g_return_val_if_reached(NULL);
  }
}

/**
 * oscats_space_dim_get_max:
 * @space: an #OscatsSpace
 * @dim: an #OscatsDim indicating the dimension
 *
 * The @dim must be a valid <emphasis>natural</emphasis> dimension for @space.
 *
 * Returns: the maximum value for natural dimension @dim in @space
 */
OscatsNatural oscats_space_dim_get_max(const OscatsSpace *space, OscatsDim dim)
{
  guint16 i = dim & OSCATS_DIM_MASK;
  g_return_val_if_fail(OSCATS_IS_SPACE(space), 0);
  g_return_val_if_fail((dim & OSCATS_DIM_TYPE_MASK) == OSCATS_DIM_NAT, 0);
  g_return_val_if_fail(i < space->num_nat, 0);
  return space->max[i];
}

/**
 * oscats_space_dim_validate:
 * @space: an #OscatsSpace
 * @dim: an #OscatsDim indicating the dimension
 * @x: a potential value in @space
 *
 * Returns: %TRUE if @dim is a valid dimension of @space and @x is within the
 * range of @dim
 */
gboolean oscats_space_validate(const OscatsSpace *space, OscatsDim dim, OscatsNatural x)
{
  guint16 i = dim & OSCATS_DIM_MASK;
  g_return_val_if_fail(OSCATS_IS_SPACE(space), FALSE);
  switch (dim & OSCATS_DIM_TYPE_MASK)
  {
    case OSCATS_DIM_CONT:
      return (i < space->num_cont);
    
    case OSCATS_DIM_BIN:
      return (i < space->num_bin) && (x == 0 || x == 1);
    
    case OSCATS_DIM_NAT:
      return (i < space->num_nat) && (x <= space->max[i]);
    
    default:  // Invalid dimension, silently ignore
      return FALSE;
  }
}

/**
 * oscats_space_compatible:
 * @lhs: an #OscatsSpace
 * @rhs: an #OscatsSpace
 *
 * Tests whether two spaces have the same geometry---that is, the same
 * number of each type of dimension.  For natural-valued dimensions, both
 * spaces must also have the same range for each dimension.  Note: This does
 * not compare dimension names.
 *
 * Returns: %TRUE if @lhs and @rhs have the same geometry
 */
gboolean oscats_space_compatible(const OscatsSpace *lhs, const OscatsSpace *rhs)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_SPACE(lhs) && OSCATS_IS_SPACE(rhs), FALSE);
  if (lhs == rhs) return TRUE;
  if (lhs->num_cont != rhs->num_cont ||
      lhs->num_bin != rhs->num_bin ||
      lhs->num_nat != rhs->num_nat)
    return FALSE;
  for (i=0; i < lhs->num_nat; i++)
    if (lhs->max[i] != rhs->max[i]) return FALSE;
  return TRUE;
}
