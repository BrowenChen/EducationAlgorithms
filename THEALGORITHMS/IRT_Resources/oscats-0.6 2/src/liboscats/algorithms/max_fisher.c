/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Select Item based on Fisher Information
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

#include "random.h"
#include "algorithms/max_fisher.h"
#include "model.h"

enum {
  PROP_0,
  PROP_NUM,
  PROP_TYPE,
  PROP_MODEL_KEY,
  PROP_THETA_KEY,
};

G_DEFINE_TYPE(OscatsAlgMaxFisher, oscats_alg_max_fisher, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_max_fisher_dispose(GObject *object);
static void oscats_alg_max_fisher_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_max_fisher_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void clear_workspace(OscatsAlgMaxFisher *self)
{
  if (self->base_num > 0)
    g_warning("OscatsAlgMaxFisher: Latent space dimension changed! Selection may be incorrect.");
  self->base_num = 0;
  if (self->base) g_object_unref(self->base);
  if (self->work) g_object_unref(self->work);
  if (self->inv) g_object_unref(self->inv);
  if (self->perm) g_object_unref(self->perm);
  self->base = self->work = self->inv = NULL;
  self->perm = NULL;
  self->dim = 0;
}

static void alloc_workspace(OscatsAlgMaxFisher *self, guint num)
{
  self->work = g_gsl_matrix_new(num, num);
  if (num > 1)
  {
    self->base = g_gsl_matrix_new(num, num);
    self->inv = g_gsl_matrix_new(num, num);
    self->perm = g_gsl_permutation_new(num);
  }
  self->dim = num;
}

static void oscats_alg_max_fisher_class_init (OscatsAlgMaxFisherClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_alg_max_fisher_dispose;
  gobject_class->set_property = oscats_alg_max_fisher_set_property;
  gobject_class->get_property = oscats_alg_max_fisher_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgMaxFisher:num:
 *
 * Number of items from which to choose.  If one, then the exact optimal
 * item is selected.  If greater than one, then a random item is chosen
 * from among the #OscatsAlgMaxFisher:num optimal items.
 */
  pspec = g_param_spec_uint("num", "", 
                            "Number of items from which to choose",
                            1, G_MAXUINT, 1,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM, pspec);

/**
 * OscatsAlgMaxFisher:A-opt:
 *
 * If true, use the A-optimality critierion.  Otherwise, use the
 * D-optimality criterion.
 */
  pspec = g_param_spec_boolean("A-opt", "A-optimality", 
                               "Use A-optimality instead of D-optimality",
                               FALSE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_TYPE, pspec);

/**
 * OscatsAlgMaxFisher:modelKey:
 *
 * The key indicating which model to use for selection.  A %NULL value or
 * empty string indicates the item's default model.
 */
  pspec = g_param_spec_string("modelKey", "model key", 
                            "Which model to use for selection",
                            NULL,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_MODEL_KEY, pspec);

/**
 * OscatsAlgMaxFisher:thetaKey:
 *
 * The key indicating which latent variable to use for selection.  A %NULL
 * value or empty string indicates the examinee's default estimation theta.
 */
  pspec = g_param_spec_string("thetaKey", "ability key", 
                            "Which latent variable to use for selection",
                            NULL,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_THETA_KEY, pspec);

}

static void oscats_alg_max_fisher_init (OscatsAlgMaxFisher *self)
{
}

static void oscats_alg_max_fisher_dispose (GObject *object)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(object);
  G_OBJECT_CLASS(oscats_alg_max_fisher_parent_class)->dispose(object);
  if (self->chooser) g_object_unref(self->chooser);
  if (self->base) g_object_unref(self->base);
  if (self->work) g_object_unref(self->work);
  if (self->inv) g_object_unref(self->inv);
  if (self->perm) g_object_unref(self->perm);
  self->chooser = NULL;
  self->base = self->work = self->inv = NULL;
  self->perm = NULL;
}

static void oscats_alg_max_fisher_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(object);
  switch (prop_id)
  {
    case PROP_NUM:			// construction only
      self->chooser = g_object_new(OSCATS_TYPE_ALG_CHOOSER,
                                   "num", g_value_get_uint(value), NULL);
      break;
    
    case PROP_TYPE:			// construction only
      self->A_opt = g_value_get_boolean(value);
      break;
    
    case PROP_MODEL_KEY:
    {
      const gchar *key = g_value_get_string(value);
      if (key == NULL || key[0] == '\0') self->modelKey = 0;
      else self->modelKey = g_quark_from_string(key);
    }
      break;
    
    case PROP_THETA_KEY:
    {
      const gchar *key = g_value_get_string(value);
      if (key == NULL || key[0] == '\0') self->thetaKey = 0;
      else self->thetaKey = g_quark_from_string(key);
    }
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_max_fisher_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(object);
  switch (prop_id)
  {
    case PROP_NUM:
      g_value_set_uint(value, self->chooser->num);
      break;
    
    case PROP_TYPE:
      g_value_set_boolean(value, self->A_opt);
      break;
    
    case PROP_MODEL_KEY:
      g_value_set_string(value, self->modelKey ?
                         g_quark_to_string(self->modelKey) : "");
      break;
    
    case PROP_THETA_KEY:
      g_value_set_string(value, self->thetaKey ?
                         g_quark_to_string(self->thetaKey) : "");
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void initialize(OscatsTest *test, OscatsExaminee *e, gpointer alg_data)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(alg_data);
  if (self->base) g_gsl_matrix_set_all(self->base, 0);
  self->base_num = 0;
}

// This value will be minimized
static gdouble criterion(const OscatsItem *item,
                         const OscatsExaminee *e,
                         gpointer data)
{
  OscatsAlgMaxFisher *alg_data = (OscatsAlgMaxFisher*)data;
  OscatsModel *model = oscats_administrand_get_model(OSCATS_ADMINISTRAND(item), alg_data->modelKey);
  guint k, dim;
  g_return_val_if_fail(model != NULL && OSCATS_IS_SPACE(model->space), 0);
  dim = model->space->num_cont;
  if (alg_data->dim != dim)
  {
    clear_workspace(alg_data);
    alloc_workspace(alg_data, dim);
  }
  if (alg_data->base)
    g_gsl_matrix_copy(alg_data->work, alg_data->base);
  else
    g_gsl_matrix_set_all(alg_data->work, 0);
  oscats_model_fisher_inf(model, alg_data->theta, e->covariates,
                           alg_data->work);
  if (dim == 1)
    return -alg_data->work->v->data[0];
    // max I_j(theta) <==> min -I_j(theta)
  if (alg_data->A_opt)
  {
    gdouble ret = 0, *data = alg_data->inv->v->data;
    guint stride = alg_data->inv->v->tda;
    g_gsl_matrix_invert(alg_data->work, alg_data->inv, alg_data->perm);
    for (k=0; k < dim; k++)
      ret += data[k*stride+k];
    return ret;
    // min tr{[sum I_j(theta)]^-1}
  } else  // D_optimality
    return -g_gsl_matrix_det(alg_data->work, alg_data->perm);
    // max det[sum I_j(theta)] <==> min -det[sum I_j(theta)]
}

static gint select (OscatsTest *test, OscatsExaminee *e,
                    GBitArray *eligible, gpointer alg_data)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(alg_data);
  self->theta = ( self->thetaKey ?
                    oscats_examinee_get_theta(e, self->thetaKey) :
                    oscats_examinee_get_est_theta(e) );
  
  if (self->base)
    for (; self->base_num < e->items->len; self->base_num++)
      oscats_model_fisher_inf(
        oscats_administrand_get_model(g_ptr_array_index(e->items, self->base_num), self->modelKey),
        self->theta, e->covariates, self->base);

  return oscats_alg_chooser_choose(self->chooser, e, eligible, alg_data);
}

/*
 * Note that unless someone does something naughty, alg_data will be of the
 * appropriate type, and test will be an OscatsTest.  The signal connections
 * should include oscats_algorithm_closure_finalize as the destruction
 * callback.  The first connection should take alg_data's reference.  Any
 * subsequent connections should be accompanied by g_object_ref(alg_data).
 */
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test)
{
  OscatsAlgMaxFisher *self = OSCATS_ALG_MAX_FISHER(alg_data);

  self->chooser->bank = g_object_ref(test->itembank);
  self->chooser->criterion = criterion;

  g_signal_connect_data(test, "initialize", G_CALLBACK(initialize),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_signal_connect_data(test, "select", G_CALLBACK(select),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_object_ref(alg_data);
}
                   
/**
 * oscats_alg_max_fisher_resize:
 * @alg_data: the #OscatsAlgMaxFisher data
 * @num: the number of continuous dimensions of the test's latent space
 *
 * Set the size of the continuous portion of the test's latent space.  This
 * function only needs to be called if the test switches to a set of models
 * from a different latent space (which is <emphasis>not</emphasis> usual).
 */
void oscats_alg_max_fisher_resize(OscatsAlgMaxFisher *alg_data, guint num)
{
  g_return_if_fail(OSCATS_IS_ALG_MAX_FISHER(alg_data));
  alg_data->base_num = 0;
  clear_workspace(alg_data);
  alloc_workspace(alg_data, num);
}
