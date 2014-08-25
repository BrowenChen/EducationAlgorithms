/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Point in Latent Space
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
 * SECTION:point
 * @title:OscatsPoint
 * @short_description: Point in Latent Space
 *
 * A point in a multidimensional latent space as defined by #OscatsSpace.
 */

#include <math.h>
#include "point.h"

G_DEFINE_TYPE(OscatsPoint, oscats_point, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_SPACE,
};

static void oscats_point_dispose (GObject *object);
static void oscats_point_finalize (GObject *object);
static void oscats_point_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_point_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);

static void oscats_point_class_init (OscatsPointClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_point_dispose;
  gobject_class->finalize = oscats_point_finalize;
  gobject_class->set_property = oscats_point_set_property;
  gobject_class->get_property = oscats_point_get_property;
  
/**
 * OscatsPoint:space:
 *
 * The #OscatsSpace in which this #OscatsPoint resides.
 * The #OscatsPoint:space <emphasis>must</emphasis> be specified at
 * construction, otherwise the #OscatsPoint will not be usable.
 */
  pspec = g_param_spec_object("space", "space", 
                            "Latent space of the point",
                            OSCATS_TYPE_SPACE,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_SPACE, pspec);

}

static void oscats_point_init (OscatsPoint *self)
{
}

static void oscats_point_dispose (GObject *object)
{
  OscatsPoint *self = OSCATS_POINT(object);
  G_OBJECT_CLASS(oscats_point_parent_class)->dispose(object);
  if (self->space) g_object_unref(self->space);
  if (self->cont && !self->link) g_free(self->cont);
  if (self->bin) g_object_unref(self->bin);
  if (self->link) g_object_unref(self->link);
  self->space = NULL;
  self->cont = NULL;
  self->bin = NULL;
  self->link = NULL;
}

static void oscats_point_finalize (GObject *object)
{
  OscatsPoint *self = OSCATS_POINT(object);
  if (self->nat) g_free(self->nat);
  self->nat = NULL;
  G_OBJECT_CLASS(oscats_point_parent_class)->finalize(object);
}

static void oscats_point_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec)
{
  OscatsPoint *self = OSCATS_POINT(object);
  
  switch (prop_id)
  {
    case PROP_SPACE:			// construction only
      self->space = g_value_dup_object(value);
      g_return_if_fail(self->space != NULL);
      if (self->space->num_cont > 0)
        self->cont = g_new0(gdouble, self->space->num_cont);
      if (self->space->num_bin > 0)
        self->bin = g_bit_array_new(self->space->num_bin);
      if (self->space->num_nat > 0)
        self->nat = g_new0(OscatsNatural, self->space->num_nat);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_point_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
  OscatsPoint *self = OSCATS_POINT(object);
  
  switch (prop_id)
  {
    case PROP_SPACE:
      g_value_set_object(value, self->space);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/**
 * oscats_point_new_from_space:
 * @space: an #OscatsSpace defining the latent space for the new point
 *
 * This is simply a convenience function for g_object_new() with the
 * @space construction property.
 *
 * Returns: (transfer full): a new #OscatsPoint in @space
 */
OscatsPoint *oscats_point_new_from_space(OscatsSpace *space)
{
  GParameter param = { "space", { 0, } };
  g_return_val_if_fail(OSCATS_IS_SPACE(space), NULL);
  g_value_init(&(param.value), OSCATS_TYPE_SPACE);
  g_value_take_object(&(param.value), space);
  return g_object_newv(OSCATS_TYPE_POINT, 1, &param);
}

/**
 * oscats_point_same_space:
 * @lhs: an #OscatsPoint
 * @rhs: another #OscatsPoint
 *
 * Returns: %TRUE if @lhs and @rhs are in the same #OscatsSpace
 */
gboolean oscats_point_same_space(const OscatsPoint *lhs, const OscatsPoint *rhs)
{
  g_return_val_if_fail(OSCATS_IS_POINT(lhs) && OSCATS_IS_POINT(rhs), FALSE);
  return (lhs->space == rhs->space);
}

/**
 * oscats_point_space_compatible:
 * @lhs: an #OscatsPoint
 * @rhs: another #OscatsPoint
 *
 * This ignores dimension names.
 *
 * Returns: %TRUE if @lhs and @rhs come from geometrically compatible spaces
 */
gboolean oscats_point_space_compatible(const OscatsPoint *lhs, const OscatsPoint *rhs)
{
  g_return_val_if_fail(OSCATS_IS_POINT(lhs) && OSCATS_IS_POINT(rhs), FALSE);
  return oscats_space_compatible(lhs->space, rhs->space);
}

/**
 * oscats_point_equal:
 * @lhs: an #OscatsPoint
 * @rhs: another #OscatsPoint
 * @tol: comparison tolerance (must be non-negative, may be 0)
 *
 * Compares two points in latent space withing the tolerance @tol.  That is,
 * returns all(abs(@rhs-@lhs) <= @tol).  If @tol == 0, the points must be
 * exactly the same.  For continuos latent spaces, to avoid problems with
 * floating point round-off errors, it's recommended to use a small positive
 * @tol, such as %OSCATS_EPS.
 *
 * The points must come from geometrically compatible spaces.
 *
 * Returns: %TRUE if @lhs and @rhs have the same coordinates
 */
gboolean oscats_point_equal(const OscatsPoint *lhs, const OscatsPoint *rhs, gdouble tol)
{
  guint i, num;
  g_return_val_if_fail(oscats_point_space_compatible(lhs, rhs), FALSE);
  g_return_val_if_fail(tol >= 0, FALSE);
  for (i=0, num=lhs->space->num_cont; i < num; i++)
    if (fabs(rhs->cont[i] - lhs->cont[i]) > tol) return FALSE;
  if (lhs->bin && !g_bit_array_equal(lhs->bin, rhs->bin)) return FALSE;
  for (i=0, num=lhs->space->num_nat; i < num; i++)
    if (fabs((gdouble)(rhs->nat[i] - lhs->nat[i])) > tol) return FALSE;
  return TRUE;
}

/**
 * oscats_point_copy:
 * @lhs: an #OscatsPoint to be replaced
 * @rhs: the #OscatsPoint to copy
 *
 * Copies replaces @lhs with @rhs.  Points must be from compatible spaces.
 */
void oscats_point_copy(OscatsPoint *lhs, const OscatsPoint *rhs)
{
  guint i, num;
  g_return_if_fail(OSCATS_IS_POINT(lhs) && OSCATS_IS_POINT(rhs));
  g_return_if_fail(oscats_space_compatible(lhs->space, rhs->space));
  num = lhs->space->num_cont;
  for (i=0; i < num; i++) lhs->cont[i] = rhs->cont[i];
  if (rhs->bin) g_bit_array_copy(lhs->bin, rhs->bin);
  num = lhs->space->num_nat;
  for (i=0; i < num; i++) lhs->nat[i] = rhs->nat[i];
}

/**
 * oscats_point_get_double:
 * @point: an #OscatsPoint
 * @dim: the coordinate to fetch
 *
 * The @dim must be valid for @point.
 *
 * Returns: the coordinate @dim of @point as a #gdouble.
 */
gdouble oscats_point_get_double(const OscatsPoint *point, OscatsDim dim)
{
  g_return_val_if_fail(OSCATS_IS_POINT(point), 0);
  g_return_val_if_fail(oscats_space_validate(point->space, dim, 0), 0);
  switch (dim & OSCATS_DIM_TYPE_MASK)
  {
    case OSCATS_DIM_CONT:
      return point->cont[dim & OSCATS_DIM_MASK];
    case OSCATS_DIM_BIN:
      return (g_bit_array_get_bit(point->bin, dim & OSCATS_DIM_MASK) ? 1 : 0);
    case OSCATS_DIM_NAT:
      return (gdouble)(point->nat[dim & OSCATS_DIM_MASK]);
    default:
      g_return_val_if_reached(0);
  }
}

/**
 * oscats_point_get_cont:
 * @point: an #OscatsPoint
 * @dim: the coordinate to fetch
 *
 * The @dim must be %OSCATS_DIM_CONT.
 *
 * Returns: the value of the continuous coordinate @dim for @point
 */
gdouble oscats_point_get_cont(const OscatsPoint *point, OscatsDim dim)
{
  g_return_val_if_fail(OSCATS_IS_POINT(point), 0);
  g_return_val_if_fail(oscats_space_validate(point->space, dim, 0), 0);
  g_return_val_if_fail((dim & OSCATS_DIM_TYPE_MASK) == OSCATS_DIM_CONT, 0);
  return point->cont[dim & OSCATS_DIM_MASK];
}

/**
 * oscats_point_get_bin:
 * @point: an #OscatsPoint
 * @dim: the coordinate to fetch
 *
 * The @dim must be %OSCATS_DIM_BIN.
 *
 * Returns: the value of the binary coordinate @dim for @point
 */
gboolean oscats_point_get_bin(const OscatsPoint *point, OscatsDim dim)
{
  g_return_val_if_fail(OSCATS_IS_POINT(point), 0);
  g_return_val_if_fail(oscats_space_validate(point->space, dim, 0), 0);
  g_return_val_if_fail((dim & OSCATS_DIM_TYPE_MASK) == OSCATS_DIM_BIN, 0);
  return g_bit_array_get_bit(point->bin, dim & OSCATS_DIM_MASK);
}

/**
 * oscats_point_get_nat:
 * @point: an #OscatsPoint
 * @dim: the coordinate to fetch
 *
 * The @dim must be %OSCATS_DIM_NAT.
 *
 * Returns: the value of the natural coordinate @dim for @point
 */
OscatsNatural oscats_point_get_nat(const OscatsPoint *point, OscatsDim dim)
{
  g_return_val_if_fail(OSCATS_IS_POINT(point), 0);
  g_return_val_if_fail(oscats_space_validate(point->space, dim, 0), 0);
  g_return_val_if_fail((dim & OSCATS_DIM_TYPE_MASK) == OSCATS_DIM_NAT, 0);
  return point->nat[dim & OSCATS_DIM_MASK];
}

/**
 * oscats_point_set_cont:
 * @point: an #OscatsPoint
 * @dim: the coordinate to set
 * @value: the value to set
 *
 * The @dim must be %OSCATS_DIM_CONT.
 */
void oscats_point_set_cont(OscatsPoint *point, OscatsDim dim, gdouble value)
{
  g_return_if_fail(OSCATS_IS_POINT(point));
  g_return_if_fail(oscats_space_validate(point->space, dim, 0));
  g_return_if_fail((dim & OSCATS_DIM_TYPE_MASK) == OSCATS_DIM_CONT);
  point->cont[dim & OSCATS_DIM_MASK] = value;
}

/**
 * oscats_point_set_cont:
 * @point: an #OscatsPoint
 * @dim: the coordinate to set
 * @value: the value to set
 *
 * The @dim must be %OSCATS_DIM_BIN.
 */
void oscats_point_set_bin(OscatsPoint *point, OscatsDim dim, gboolean value)
{
  g_return_if_fail(OSCATS_IS_POINT(point));
  g_return_if_fail(oscats_space_validate(point->space, dim, 0));
  g_return_if_fail((dim & OSCATS_DIM_TYPE_MASK) == OSCATS_DIM_BIN);
  if (value) g_bit_array_set_bit(point->bin, dim & OSCATS_DIM_MASK);
  else     g_bit_array_clear_bit(point->bin, dim & OSCATS_DIM_MASK);
}

/**
 * oscats_point_set_cont:
 * @point: an #OscatsPoint
 * @dim: the coordinate to set
 * @value: the value to set
 *
 * The @dim must be %OSCATS_DIM_NAT and @value must be valid for @point.
 */
void oscats_point_set_nat(OscatsPoint *point, OscatsDim dim, OscatsNatural value)
{
  g_return_if_fail(OSCATS_IS_POINT(point));
  g_return_if_fail(oscats_space_validate(point->space, dim, value));
  g_return_if_fail((dim & OSCATS_DIM_TYPE_MASK) == OSCATS_DIM_NAT);
  point->nat[dim & OSCATS_DIM_MASK] = value;
}

/**
 * oscats_point_cont_as_vector:
 * @point: an #OscatsPoint with at least one continuous dimension
 *
 * Create a link between the continuous dimensions of  @point and a
 * #GGslVector so that when either is changed, the other is updated
 * (they share the same memory segment).
 *
 * Returns: (transfer none): a #GGslVector linked with the continuous
 * dimensions of @point
 */
GGslVector * oscats_point_cont_as_vector(OscatsPoint *point)
{
  gdouble *data;
  guint i, num;
  
  g_return_val_if_fail(OSCATS_IS_POINT(point), NULL);
  if (point->link) return point->link;

  g_return_val_if_fail(OSCATS_IS_SPACE(point->space), NULL);
  num = point->space->num_cont;
  g_return_val_if_fail(num > 0 && point->cont, NULL);

  data = point->cont;
  point->link = g_gsl_vector_new(num);
  point->cont = point->link->v->data;
  if (data)
  {
    for (i=0; i < num; i++) point->cont[i] = data[i];
    g_free(data);
  }
  return point->link;
}
