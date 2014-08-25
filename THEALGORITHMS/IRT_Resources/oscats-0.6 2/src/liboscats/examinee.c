/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Examinee Class
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
 * SECTION:examinee
 * @title:OscatsExaminee
 * @short_description: Examinee Class
 *
 * An #OscatsExaminee represents a single examinee taking a single test.
 * That is, a given #OscatsExaminee may be administered only one test at a
 * time.
 *
 * The examinee may be associated with several points in latent space. 
 * (Generally, all points will be from the same latent space, but complex
 * simulations may involve a collection of algorithms that operate in
 * several different spaces simulatneously.) Each point in latent space is
 * associated with a name or key.
 *
 * Each examinee has two special keys: one used as the default for
 * simulation and one used as the default for estimation/item selection.  If
 * these keys are not specified explicitly, they are taken as "simDefault"
 * and "estDefault", respectively.  Simulation, estimation, and item
 * selection algorithms are not required to use the default keys.  Refer to
 * the documentation for particular algorithms for details on which keys are
 * used.
 */

#include <math.h>
#include "examinee.h"

G_DEFINE_TYPE(OscatsExaminee, oscats_examinee, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_ID,
  PROP_COVARIATES,
};

GQuark simKey, estKey;

#define GET_THETA(E, K) (g_datalist_id_get_data(&((E)->theta), (K)))

static void oscats_examinee_dispose (GObject *object);
static void oscats_examinee_finalize (GObject *object);
static void oscats_examinee_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_examinee_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);
                   
static void oscats_examinee_class_init (OscatsExamineeClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  simKey = g_quark_from_string("simDefault");
  estKey = g_quark_from_string("estDefault");

  gobject_class->dispose = oscats_examinee_dispose;
  gobject_class->finalize = oscats_examinee_finalize;
  gobject_class->set_property = oscats_examinee_set_property;
  gobject_class->get_property = oscats_examinee_get_property;
  
/**
 * OscatsExaminee:id:
 *
 * A string identifier for the examinee.
 */
  pspec = g_param_spec_string("id", "ID", 
                            "String identifier for the examinee",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_ID, pspec);

/**
 * OscatsExaminee:covariates:
 *
 * A set of covariates for the examinee.
 */
  pspec = g_param_spec_object("covariates", "Covariates", 
                            "Covariates for the examinee",
                            OSCATS_TYPE_COVARIATES,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_COVARIATES, pspec);

}

static void oscats_examinee_init (OscatsExaminee *self)
{
  self->simKey = simKey;
  self->estKey = estKey;
  g_datalist_init(&(self->theta));
}

static void oscats_examinee_dispose (GObject *object)
{
  OscatsExaminee *self = OSCATS_EXAMINEE(object);
  G_OBJECT_CLASS(oscats_examinee_parent_class)->dispose(object);
  g_datalist_clear(&(self->theta));
  if (self->covariates) g_object_unref(self->covariates);
  if (self->items) g_ptr_array_unref(self->items);
  if (self->resp) g_array_unref(self->resp);
  self->simTheta = self->estTheta = NULL;
  self->covariates = NULL;
  self->items = NULL;
  self->resp = NULL;
}

static void oscats_examinee_finalize (GObject *object)
{
  OscatsExaminee *self = OSCATS_EXAMINEE(object);
  g_free(self->id);
  G_OBJECT_CLASS(oscats_examinee_parent_class)->finalize(object);
}

static void oscats_examinee_set_property(GObject *object, guint prop_id,
                                     const GValue *value, GParamSpec *pspec)
{
  OscatsExaminee *self = OSCATS_EXAMINEE(object);
  GString *id;
  switch (prop_id)
  {
    case PROP_ID:			// construction only
      self->id = g_value_dup_string(value);
      if (!self->id)
      {
        id = g_string_sized_new(22);
        g_string_printf(id, "[Examinee %p]", self);
        self->id = id->str;
        g_string_free(id, FALSE);
      }
      break;
    
    case PROP_COVARIATES:
      if (self->covariates) g_object_unref(self->covariates);
      self->covariates = g_value_dup_object(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_examinee_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
  OscatsExaminee *self = OSCATS_EXAMINEE(object);
  switch (prop_id)
  {
    case PROP_ID:
      g_value_set_string(value, self->id);
      break;
    
    case PROP_COVARIATES:
      if (!self->covariates)
        self->covariates = g_object_newv(OSCATS_TYPE_COVARIATES, 0, NULL);
      g_value_set_object(value, self->covariates);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/**
 * oscats_examinee_set_sim_key:
 * @e: an #OscatsExaminee
 * @name: the #GQuark name of the point for simulation
 *
 * @name may be 0 to refer to "simDefault".
 */
void oscats_examinee_set_sim_key(OscatsExaminee *e, GQuark name)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e));
  e->simKey = (name ? name : simKey);
  e->simTheta = GET_THETA(e, e->simKey);
}

/**
 * oscats_examinee_get_sim_key:
 * @e: an #OscatsExaminee
 *
 * Returns: the key for the #OscatsPoint used by default for simulation
 */
GQuark oscats_examinee_get_sim_key(const OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), 0);
  return e->simKey;
}

/**
 * oscats_examinee_set_est_key:
 * @e: an #OscatsExaminee
 * @name: the #GQuark name of the point for estimation/item selection
 *
 * @name may be 0 to refer to "estDefault".
 */
void oscats_examinee_set_est_key(OscatsExaminee *e, GQuark name)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e));
  e->estKey = (name ? name : estKey);
  e->estTheta = GET_THETA(e, e->estKey);
}

/**
 * oscats_examinee_get_est_key:
 * @e: an #OscatsExaminee
 *
 * Returns: the key for the #OscatsPoint used by default for estimation
 * and item selection
 */
GQuark oscats_examinee_get_est_key(const OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), 0);
  return e->estKey;
}

/**
 * oscats_examinee_set_sim_theta:
 * @e: an #OscatsExaminee
 * @theta: (transfer full): an #OscatsPoint to use for simulation
 *
 * Sets @theta as the point referred to by the simulation key for @e.
 * Note that @e takes ownership of @theta.
 */
void oscats_examinee_set_sim_theta(OscatsExaminee *e, OscatsPoint *theta)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e));
  oscats_examinee_set_theta(e, e->simKey, theta);
}

/**
 * oscats_examinee_get_sim_theta:
 * @e: an #OscatsExaminee
 *
 * Returns: (transfer none): the #OscatsPoint indicated to be used by
 * default for simulation, or %NULL if no point has been assigned
 */
OscatsPoint * oscats_examinee_get_sim_theta(const OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return e->simTheta;
}

/**
 * oscats_examinee_set_est_theta:
 * @e: an #OscatsExaminee
 * @theta: (transfer full): an #OscatsPoint to be used for estimation
 *
 * Set @theta as the point referred to by the estimation key for @e.
 * Note that @e takes ownership of @theta.
 */
void oscats_examinee_set_est_theta(OscatsExaminee *e, OscatsPoint *theta)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e));
  oscats_examinee_set_theta(e, e->estKey, theta);
}

/**
 * oscats_examinee_get_est_theta:
 * @e: an #OscatsExaminee
 *
 * Returns: (transfer none): the #OscatsPoint indicated to be used by
 * default for estimation and item selection, or %NULL if no point has been
 * assigned
 */
OscatsPoint * oscats_examinee_get_est_theta(const OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return e->estTheta;
}

/**
 * oscats_set_theta:
 * @e: an #OscatsExaminee
 * @name: the #GQuark name of the point to set
 * @theta: (transfer full): the #OscatsPoint to set
 *
 * Sets @point as the latent variable named @name for @e.  Any previous
 * point set as @name is replaced.  Note, @name may <emphasis>not</emphasis>
 * be 0.  Note that @e takes ownership of @theta.
 */
void oscats_examinee_set_theta(OscatsExaminee *e, GQuark name, OscatsPoint *theta)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e) && OSCATS_IS_POINT(theta));
  g_return_if_fail(name != 0);
  // Examinee takes ownership of theta
  g_datalist_id_set_data_full(&(e->theta), name, theta, g_object_unref);
  if (name == e->simKey) e->simTheta = theta;
  if (name == e->estKey) e->estTheta = theta;
}

/**
 * oscats_set_theta_by_name:
 * @e: an #OscatsExaminee
 * @name: the name of the point to set
 * @theta: (transfer full): the #OscatsPoint to set
 *
 * Convenience wrapper for oscats_set_theta().
 */
void oscats_examinee_set_theta_by_name(OscatsExaminee *e, const gchar *name, OscatsPoint *theta)
{
  oscats_examinee_set_theta(e, g_quark_from_string(name), theta);
}

/**
 * oscats_examinee_get_theta:
 * @e: an #OscatsExaminee
 * @name: the key for the #OscatsPoint to fetch
 *
 * Note, @name may <emphasis>not</emphasis> be 0.
 *
 * Returns: (transfer none): the #OscatsPoint for @e named @name
 */
OscatsPoint * oscats_examinee_get_theta(OscatsExaminee *e, GQuark name)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return GET_THETA(e, name);
}

/**
 * oscats_examinee_get_theta_by_name:
 * @e: an #OscatsExaminee
 * @name: the name of the #OscatsPoint to fetch
 *
 * Convenience wrapper for oscats_examinee_get_theta().
 *
 * Returns: (transfer none): the #OscatsPoint for @e named @name
 */
OscatsPoint * oscats_examinee_get_theta_by_name(OscatsExaminee *e, const gchar *name)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return GET_THETA(e, g_quark_from_string(name));
}

/**
 * oscats_examinee_init_sim_theta:
 * @e: an #OscatsExaminee
 * @space: (transfer none): the latent space from which to generate the #OscatsPoint
 *
 * Generate a new #OscatsPoint from @space for @e to use in simulation.
 * The point is stored under the current simulation key.
 *
 * Returns: (transfer none): the new #OscatsPoint
 */
OscatsPoint * oscats_examinee_init_sim_theta(OscatsExaminee *e, OscatsSpace *space)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return oscats_examinee_init_theta(e, e->simKey, space);
}

/**
 * oscats_examinee_init_est_theta:
 * @e: an #OscatsExaminee
 * @space: (transfer none): the latent space from which to generate the #OscatsPoint
 *
 * Generate a new #OscatsPoint from @space for @e to use in estimation and
 * item selection.  The point is stored under the current estimation key.
 *
 * Returns: (transfer none): the new #OscatsPoint
 */
OscatsPoint * oscats_examinee_init_est_theta(OscatsExaminee *e, OscatsSpace *space)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  return oscats_examinee_init_theta(e, e->estKey, space);
}

/**
 * oscats_examinee_init_theta:
 * @e: an #OscatsExaminee
 * @name: the #GQuark name for the new #OscatsPoint
 * @space: (transfer none): the latent space from which to generate the #OscatsPoint
 *
 * Generate a new #OscatsPoint from @space for @e under the key @name.
 * Any previous #OscatsPoint under @name in @e is replaced.
 * Note: @name may <emphasis>not</emphasis> be 0.
 *
 * Returns: (transfer none): the new #OscatsPoint
 */
OscatsPoint * oscats_examinee_init_theta(OscatsExaminee *e, GQuark name, OscatsSpace *space)
{
  OscatsPoint *theta;
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e) && OSCATS_IS_SPACE(space), NULL);
  g_return_val_if_fail(name != 0, NULL);
  theta = oscats_point_new_from_space(space);
  oscats_examinee_set_theta(e, name, theta);
  return theta;
}

/**
 * oscats_examinee_prep:
 * @e: an #OscatsExaminee
 * @length_hint: guess for test length
 *
 * Prepares the examinee for the CAT by reseting the item/resp arrays.
 * If the arrays do not already exist, they are preallocated to hold
 * @length_hint elements (recommended), the expected length of the test.
 */
void oscats_examinee_prep(OscatsExaminee *e, guint length_hint)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e));
  if (e->items)
    g_ptr_array_set_size(e->items, 0);
  else
  {
    e->items = g_ptr_array_sized_new(length_hint);
    g_ptr_array_set_free_func(e->items, g_object_unref);
  }
  if (e->resp)
    g_array_set_size(e->resp, 0);
  else
    e->resp = g_array_sized_new(FALSE, FALSE, sizeof(OscatsResponse),
                                length_hint);
}

/**
 * oscats_examinee_add_item:
 * @e: an #OscatsExaminee
 * @item: (transfer none): the #OscatsItem this examinee has taken
 * @resp: the examinee's response to the item
 *
 * Adds the (@item, @resp) pair to the list of items this examinee has been
 * administered.  The reference count for @item is increased.
 */
void oscats_examinee_add_item(OscatsExaminee *e, OscatsItem *item, OscatsResponse resp)
{
  g_return_if_fail(OSCATS_IS_EXAMINEE(e) && OSCATS_IS_ITEM(item));
  g_ptr_array_add(e->items, item);
  g_object_ref(item);
  g_array_append_val(e->resp, resp);
}

/**
 * oscats_examinee_num_items:
 * @e: an #OscatsExaminee
 *
 * Returns: the number of items this examinee has been administered
 */
guint oscats_examinee_num_items(const OscatsExaminee *e)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), 0);
  return (e->items ? e->items->len : 0);
}

/**
 * oscats_examinee_get_item:
 * @e: an #OscatsExaminee
 * @i: the item number
 *
 * Returns: (transfer none): the #OscatsItem @i for examinee @e
 */
OscatsItem * oscats_examinee_get_item(OscatsExaminee *e, guint i)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), NULL);
  g_return_val_if_fail(e->items && i < e->items->len, NULL);
  return g_ptr_array_index(e->items, i);
}

/**
 * oscats_examinee_get_resp:
 * @e: an #OscatsExaminee
 * @i: the item number
 *
 * Returns: the response to item @i for examineee @e
 */
OscatsResponse oscats_examinee_get_resp(OscatsExaminee *e, guint i)
{
  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), 0);
  g_return_val_if_fail(e->resp && i < e->resp->len, 0);
  return g_array_index(e->resp, OscatsResponse, i);
}

/**
 * oscats_examinee_logLik:
 * @e: an #OscatsExaminee
 * @theta: an #OscatsPoint
 * @modelKey: the model name to use
 *
 * Computes the log-likelihood of the responses from @e, given latent ability
 * @theta. The default model is used if @model == 0.
 *
 * Returns: the log-likelihood
 */
gdouble oscats_examinee_logLik(const OscatsExaminee *e, const OscatsPoint *theta, GQuark modelKey)
{
  OscatsAdministrand **items;
  OscatsResponse *resp;
  OscatsModel *model;
  gdouble L=0;
  guint i, num;

  g_return_val_if_fail(OSCATS_IS_EXAMINEE(e), G_MAXDOUBLE);
  g_return_val_if_fail(OSCATS_IS_POINT(theta), G_MAXDOUBLE);
  g_return_val_if_fail(e->items->len == e->resp->len, G_MAXDOUBLE);
  num = e->items->len;
  if (num == 0) return G_MAXDOUBLE;
  items = (OscatsAdministrand**)(e->items->pdata);
  resp = (OscatsResponse*)(e->resp->data);
  
  for (i=0; i < num; i++)
  {
    model = oscats_administrand_get_model(items[i], modelKey);
    L += log(oscats_model_P(model, resp[i], theta, e->covariates));
  }
  
  return L;
}

