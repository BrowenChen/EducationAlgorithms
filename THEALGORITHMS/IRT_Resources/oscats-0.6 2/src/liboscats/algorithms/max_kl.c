/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Select Item based on Kullback-Leibler Divergence
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

/*
 * initialize():
 *   Set e, Reset base_num, Inf.
 * select():
 *   Set theta_hat
 *   Call chooser(), which calls criterion() for each eligligble item.
 * criterion():
 *   Set model, max, p, p_sum
 *   If continuous, call integrator, which calls integrand().
 *   otherwise, call sum().
 * integrand():
 *   call sum() and tack on continuous prior, if necessary.
 * sum():
 *   call summand() and tack on a discrete prior, if necessary.
 * summand():
 *   returns KL(theta_hat || theta) { Prod_i P_i(x_i|theta) }
 *
 * Currently, the prior has discrete and continuous dimensions as independent,
 * but this restriction will be removed in the future.
 */

#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include "random.h"
#include "algorithms/max_kl.h"
#include "model.h"

enum {
  PROP_0,
  PROP_NUM,
  PROP_INF_BOUNDS,
  PROP_POSTERIOR,
  PROP_C,
  PROP_MU,
  PROP_SIGMA,
  PROP_DPRIOR,
  PROP_MODEL_KEY,
  PROP_THETA_KEY,
};

G_DEFINE_TYPE(OscatsAlgMaxKl, oscats_alg_max_kl, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_max_kl_dispose(GObject *object);
static void oscats_alg_max_kl_finalize(GObject *object);
static void oscats_alg_max_kl_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_max_kl_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);
static gdouble integrand(const GGslVector *theta, gpointer data);

static gboolean alloc_workspace(OscatsAlgMaxKl *self, OscatsSpace *space)
{
  guint num_cont, i;

  g_return_val_if_fail(OSCATS_IS_ALG_MAX_KL(self), FALSE);
  g_return_val_if_fail(OSCATS_IS_SPACE(space), FALSE);
  num_cont = space->num_cont;

  if (self->base_num > 0)
    g_warning("OscatsAlgMaxKl: Latent space dimension changed! Selection may be incorrect.");

  if (space->num_bin + space->num_nat == 0)
    self->numPatterns = 0;
  else
    self->numPatterns = 1;
  if (space->num_bin > 0)
  {
    g_return_val_if_fail(space->num_bin < sizeof(guint)*8, FALSE);
    self->numPatterns = (1 << space->num_bin);
  }
  for (i=0; i < space->num_nat; i++)
  {
    g_return_val_if_fail(self->numPatterns < G_MAXUINT/(space->max[i]+1), FALSE);
    self->numPatterns *= (space->max[i]+1);
  }

  if (self->theta) g_object_unref(self->theta);
  if (self->tmp) gsl_vector_free(self->tmp);
  if (self->tmp2) gsl_vector_free(self->tmp2);
  if (self->Inf) g_object_unref(self->Inf);
  if (self->Inf_inv) g_object_unref(self->Inf_inv);
  self->tmp = self->tmp2 = NULL;
  self->Inf = self->Inf_inv = NULL;

  self->theta = oscats_point_new_from_space(space);

  if (num_cont > 0)
  {
    if (self->posterior)
    {
      if (self->mu)
        g_return_val_if_fail(self->mu->size == num_cont, FALSE);
      if (self->Sigma_half)
        g_return_val_if_fail(self->Sigma_half->size1 == num_cont &&
                             self->Sigma_half->size2 == num_cont, FALSE);
      self->tmp = gsl_vector_alloc(num_cont);
      self->tmp2 = gsl_vector_alloc(num_cont);
    } else if (self->inf_bounds) {
      self->Inf = g_gsl_matrix_new(num_cont, num_cont);
      self->Inf_inv = g_gsl_matrix_new(num_cont, num_cont);
    }
    oscats_integrate_set_c_function(self->integrator, num_cont, integrand);
    oscats_integrate_link_point(self->integrator, self->theta);
  }

  if (self->space) g_object_unref(self->space);
  self->space = space;
  g_object_ref(space);

  return TRUE;
}

static void oscats_alg_max_kl_class_init (OscatsAlgMaxKlClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_alg_max_kl_dispose;
  gobject_class->finalize = oscats_alg_max_kl_finalize;
  gobject_class->set_property = oscats_alg_max_kl_set_property;
  gobject_class->get_property = oscats_alg_max_kl_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgMaxKl:num:
 *
 * Number of items from which to choose.  If one, then the exact optimal
 * item is selected.  If greater than one, then a random item is chosen
 * from among the #OscatsAlgMaxKl:num optimal items.
 */
  pspec = g_param_spec_uint("num", "", 
                            "Number of items from which to choose",
                            1, G_MAXUINT, 1,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM, pspec);

/**
 * OscatsAlgMaxKl:inf-bounds:
 *
 * If true, integrate continuous dimensions over the confidence ellipsoid.
 * Otherwise, integrate over the box theta.hat +/- c/sqrt(n).
 */
  pspec = g_param_spec_boolean("inf-bounds", "Fisher Information Bounds", 
                               "Integrate over confidence ellipsoid",
                               FALSE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_INF_BOUNDS, pspec);

/**
 * OscatsAlgMaxKl:posterior:
 *
 * If true, use posterior-weighted KL index.  (Note: if true, 
 * #OscatsAlgMaxKl:inf-bounds is ignored.)
 */
  pspec = g_param_spec_boolean("posterior", "Posterior-weighted", 
                               "Use posterior-weighted KL index",
                               FALSE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_POSTERIOR, pspec);

/**
 * OscatsAlgMaxKl:c:
 *
 * The constant c in integration bounds.
 */
  pspec = g_param_spec_double("c", "Scaling constant", 
                              "The constant c in integration bounds",
                              1e-16, 1e16, 3,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_C, pspec);

/**
 * OscatsAlgMaxKl:mu:
 *
 * Prior population mean for posterior weight for continuous dimensions.
 * (Note: The value is copied.) Default: 0.
 */
  pspec = g_param_spec_object("mu", "Prior mean", 
                              "Prior population mean for posterior weight",
                              G_TYPE_GSL_VECTOR,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_MU, pspec);

/**
 * OscatsAlgMaxKl:Sigma:
 *
 * Prior population covariance matrix for posterior weight for continuous
 * dimensions.  (Note: The value is copied.)  Default: identity.
 */
  pspec = g_param_spec_object("Sigma", "Prior covariance", 
                              "Prior population covariance matrix for posterior weight",
                              G_TYPE_GSL_MATRIX,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_SIGMA, pspec);

/**
 * OscatsAlgMaxKl:Dprior:
 *
 * Prior distribution for discrete dimensions as a vector of probabilities
 * for all Prod_i n_i patterns, where n_i is the number of categories for
 * discrete dimension i.  The values should be ordered so that the lowest
 * numbered binary dimension increases fasted, and the ordinal dimensions
 * follow binary dimensions.  The probabilities should sum to 1.  Default:
 * uniform.
 */
  pspec = g_param_spec_object("Dprior", "Discrete prior", 
                              "Prior distribution for discrete dimensions",
                              G_TYPE_GSL_VECTOR,
                              G_PARAM_READWRITE | 
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_DPRIOR, pspec);

/**
 * OscatsAlgMaxKl:modelKey:
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
 * OscatsAlgMaxKl:thetaKey:
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

static void oscats_alg_max_kl_init (OscatsAlgMaxKl *self)
{
  self->integrator = g_object_new(OSCATS_TYPE_INTEGRATE, NULL);
}

static void oscats_alg_max_kl_dispose (GObject *object)
{
  OscatsAlgMaxKl *self = OSCATS_ALG_MAX_KL(object);
  G_OBJECT_CLASS(oscats_alg_max_kl_parent_class)->dispose(object);
  if (self->chooser) g_object_unref(self->chooser);
  if (self->space) g_object_unref(self->space);
  if (self->Dprior) g_object_unref(self->Dprior);
  if (self->theta) g_object_unref(self->theta);
  if (self->integrator) g_object_unref(self->integrator);
  if (self->Inf) g_object_unref(self->Inf);
  if (self->Inf_inv) g_object_unref(self->Inf_inv);
  self->chooser = NULL;
  self->space = NULL;
  self->Dprior = NULL;
  self->theta = NULL;
  self->integrator = NULL;
  self->Inf = NULL;
  self->Inf_inv = NULL;
}

static void oscats_alg_max_kl_finalize (GObject *object)
{
  OscatsAlgMaxKl *self = OSCATS_ALG_MAX_KL(object);
  if (self->mu) gsl_vector_free(self->mu);
  if (self->Sigma_half) gsl_matrix_free(self->Sigma_half);
  if (self->p) g_free(self->p);
  if (self->tmp) gsl_vector_free(self->tmp);
  if (self->tmp2) gsl_vector_free(self->tmp2);
  G_OBJECT_CLASS(oscats_alg_max_kl_parent_class)->finalize(object);
}

static void oscats_alg_max_kl_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgMaxKl *self = OSCATS_ALG_MAX_KL(object);
  switch (prop_id)
  {
    case PROP_NUM:			// construction only
      self->chooser = g_object_new(OSCATS_TYPE_ALG_CHOOSER,
                                   "num", g_value_get_uint(value), NULL);
      break;
    
    case PROP_INF_BOUNDS:		// construction only
      self->inf_bounds = g_value_get_boolean(value);
      break;
    
    case PROP_POSTERIOR:		// construction only
      self->posterior = g_value_get_boolean(value);
      break;
    
    case PROP_C:
      self->c = g_value_get_double(value);
      break;
    
    case PROP_MU:
    {
      GGslVector *mu = g_value_get_object(value);
      if (mu)
      {
        if (self->mu && self->mu->size != mu->v->size)
        {
          gsl_vector_free(self->mu);
          self->mu = NULL;
        }
        if (self->mu == NULL) self->mu = gsl_vector_alloc(self->mu->size);
        gsl_vector_memcpy(self->mu, mu->v);
      } else {
        if (self->mu) gsl_vector_free(self->mu);
        self->mu = NULL;
      }
      break;
    }
    
    case PROP_SIGMA:
    {
      GGslMatrix *Sigma = g_value_get_object(value);
      if (Sigma)
      {
        g_return_if_fail(Sigma->v->size1 == Sigma->v->size2);
        if (self->Sigma_half == NULL)
          self->Sigma_half = gsl_matrix_alloc(Sigma->v->size1, Sigma->v->size2);
        gsl_matrix_memcpy(self->Sigma_half, Sigma->v);
        gsl_linalg_cholesky_decomp(self->Sigma_half);
      } else {
        if (self->Sigma_half) gsl_matrix_free(self->Sigma_half);
        self->Sigma_half = NULL;
      }
      break;
    }
    
    case PROP_DPRIOR:
      if (self->Dprior) g_object_unref(self->Dprior);
      self->Dprior = g_value_get_object(value);
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

static void oscats_alg_max_kl_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgMaxKl *self = OSCATS_ALG_MAX_KL(object);
  switch (prop_id)
  {
    case PROP_NUM:
      g_value_set_uint(value, self->chooser->num);
      break;
    
    case PROP_INF_BOUNDS:
      g_value_set_boolean(value, self->inf_bounds);
      break;
    
    case PROP_POSTERIOR:
      g_value_set_boolean(value, self->posterior);
      break;
    
    case PROP_C:
      g_value_set_double(value, self->c);
      break;
    
    case PROP_MU:
      if (self->mu)
      {
        GGslVector *mu = g_gsl_vector_new(self->mu->size);
        gsl_vector_memcpy(mu->v, self->mu);
        g_value_take_object(value, mu);
      } else
        g_value_set_object(value, NULL);
      break;
    
    case PROP_SIGMA:
      if (self->Sigma_half)
      {
        GGslMatrix *Sigma = g_gsl_matrix_new(self->Sigma_half->size1,
                                             self->Sigma_half->size2);
        // set lower triangle of Sigma->v
        guint i, j;
        for (i=0; i < Sigma->v->size1; i++)
          for (j=0; j <= i; j++)
            Sigma->v->data[i*Sigma->v->tda+j] =
              self->Sigma_half->data[i*self->Sigma_half->tda+j];
        // Sigma = Sigma_half Sigma_half'
        gsl_blas_dtrmm(CblasRight, CblasTrans, CblasLower, CblasNonUnit,
                       1, self->Sigma_half, Sigma->v);
        g_value_take_object(value, Sigma);
      } else
        g_value_set_object(value, NULL);
      break;
    
    case PROP_DPRIOR:
      g_value_set_object(value, self->Dprior);
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
  OscatsAlgMaxKl *self = OSCATS_ALG_MAX_KL(alg_data);
  if (self->Inf) g_gsl_matrix_set_all(self->Inf, 0);
  self->base_num = 0;
  self->e = e;
}

// - KL(theta_hat || theta) { Prod_i P_i(x_i|theta) }
static gdouble summand(OscatsAlgMaxKl *self)
{
  OscatsExaminee *e = self->e;
  OscatsResponse k;
  gdouble val=0, L=1;
  guint i;
  
  if (self->posterior)
    for (i=0; i < e->items->len; i++)
      L *= oscats_model_P(
             oscats_administrand_get_model(g_ptr_array_index(e->items, i), self->modelKey),
             e->resp->data[i], self->theta, e->covariates);

  for (k=0; k <= self->max; k++)
    val += self->p[k] * log(oscats_model_P(self->model, k, self->theta, e->covariates));
  return (val - self->p_sum) * L;
}

// Sum KL(theta_hat || theta) { Prod_i P_i(x_i|theta) } g_discr(theta)
static gdouble sum(OscatsAlgMaxKl *alg_data)
{
  OscatsNatural i, junk=0, *max;
  gdouble val=0, *Dprior=NULL;
  guint numNat = alg_data->space->num_nat;
  guint numBin = alg_data->space->num_bin;
  guint I, J, n=0, stride=1;

  if (numBin == 0 && numNat == 0)	// only continuous dimensions
    return summand(alg_data);
  
  if (numNat > 0) max = alg_data->space->max;
  else  // Fake a natural dimension
  {
    numNat = 1;
    max = &junk;
  }
  
  if (alg_data->Dprior)
  {
    if (alg_data->Dprior->v->size != alg_data->numPatterns)
      g_warning("Incorrect length for discrete prior. Using uniform distribution.");
    else
    {
      stride = alg_data->Dprior->v->stride;
      Dprior = alg_data->Dprior->v->data;
    }
  }

  for (I=0; I < numNat; I++)		// natural dimension
    for (i=0; i <= max[I]; i++)		// value of this natural dimension
    {
      if (alg_data->theta->nat) alg_data->theta->nat[I] = i;
      if (numBin == 0)
        val += summand(alg_data) * (Dprior ? Dprior[stride*(n++)] : 1);
      else
        for (J=0; J < numBin; J++)	// binary dimension
        {
          // binary dim == 0
          g_bit_array_clear_bit(alg_data->theta->bin, J);
          val += summand(alg_data) * (Dprior ? Dprior[stride*(n++)] : 1);
          
          // binary dim == 1
          g_bit_array_set_bit(alg_data->theta->bin, J);
          val += summand(alg_data) * (Dprior ? Dprior[stride*(n++)] : 1);
        }
    }
      
  return val;
}

static gdouble integrand(const GGslVector *theta, gpointer data)
{
  OscatsAlgMaxKl *self = (OscatsAlgMaxKl*)data;
  
  if (self->posterior)
  {
    gdouble g;
    gsl_vector_memcpy(self->tmp, theta->v);
    if (self->mu) gsl_vector_sub(self->tmp, self->mu);
    if (self->Sigma_half)
      gsl_linalg_cholesky_solve(self->Sigma_half, self->tmp, self->tmp2);
    else
      gsl_vector_memcpy(self->tmp2, self->tmp);
    gsl_blas_ddot(self->tmp, self->tmp2, &g);
    return sum(self) * exp(-g/2);
  }
  else return sum(self);
}

// This value will be minimized
// Return values have been negated accordingly.
static gdouble criterion(const OscatsItem *item,
                         const OscatsExaminee *e,
                         gpointer data)
{
  OscatsAlgMaxKl *alg_data = OSCATS_ALG_MAX_KL(data);
  OscatsModel *model = oscats_administrand_get_model(OSCATS_ADMINISTRAND(item), alg_data->modelKey);
  gdouble I = 0;
  guint k;

  g_return_val_if_fail(model != NULL, 0);
  if (!(alg_data->space &&
        oscats_space_compatible(alg_data->space, model->space)))
    g_return_val_if_fail(alloc_workspace(alg_data, model->space), 0);

  alg_data->model = model;
  alg_data->max = oscats_model_get_max(model);
  if (alg_data->max >= alg_data->p_num)
  {
    if (alg_data->p) g_free(alg_data->p);
    alg_data->p_num = alg_data->max+1;
    alg_data->p = g_new(gdouble, alg_data->p_num);
  }
  for (k=0; k <= alg_data->max; k++)
  {
    gdouble p = oscats_model_P(model, k, alg_data->theta_hat, e->covariates);
    alg_data->p[k] = p;
    I += p * log(p);
  }
  alg_data->p_sum = I;

  if (alg_data->space->num_cont == 0)
    return sum(alg_data);
  
  if (alg_data->posterior)
    return oscats_integrate_space(alg_data->integrator, alg_data);
  else if (alg_data->inf_bounds)
  {
    if (alg_data->base_num == 0)
      return oscats_integrate_space(alg_data->integrator, alg_data);
    else
      return oscats_integrate_ellipse(alg_data->integrator,
                            oscats_point_cont_as_vector(alg_data->theta_hat),
                                      alg_data->Inf_inv,
                                      alg_data->c,
                                      alg_data);
  }
  else
    return oscats_integrate_cube(alg_data->integrator,
                            oscats_point_cont_as_vector(alg_data->theta_hat),
                                 alg_data->c / 
                                 (alg_data->e->items->len > 0 ?
                                  sqrt(alg_data->e->items->len) : 1),
                                 alg_data);
}

static gint select (OscatsTest *test, OscatsExaminee *e,
                    GBitArray *eligible, gpointer alg_data)
{
  OscatsAlgMaxKl *self = OSCATS_ALG_MAX_KL(alg_data);
  self->theta_hat = ( self->thetaKey ?
                        oscats_examinee_get_theta(e, self->thetaKey) :
                        oscats_examinee_get_est_theta(e) );

  if (self->Inf)
  {
    for (; self->base_num < e->items->len; self->base_num++)
      oscats_model_fisher_inf(
        oscats_administrand_get_model(g_ptr_array_index(e->items, self->base_num), self->modelKey),
        self->theta_hat, e->covariates, self->Inf);
    g_gsl_matrix_copy(self->Inf_inv, self->Inf);
    gsl_linalg_cholesky_decomp(self->Inf_inv->v);
    gsl_linalg_cholesky_invert(self->Inf_inv->v);
  }

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
  OscatsAlgMaxKl *self = OSCATS_ALG_MAX_KL(alg_data);

  self->chooser->bank = g_object_ref(test->itembank);
  self->chooser->criterion = criterion;

  g_signal_connect_data(test, "initialize", G_CALLBACK(initialize),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_signal_connect_data(test, "select", G_CALLBACK(select),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_object_ref(alg_data);
}
                   
/**
 * oscats_alg_max_kl_resize:
 * @alg_data: the #OscatsAlgMaxKl data
 * @space: the space for the test
 *
 * Reallocate working memory for a new test space.  This function only needs
 * to be called if the test switches to a set of models from a different
 * latent space (which is <emphasis>not</emphasis> usual).
 */
void oscats_alg_max_kl_resize(OscatsAlgMaxKl *alg_data, OscatsSpace *space)
{
  g_return_if_fail(OSCATS_IS_ALG_MAX_KL(alg_data));
  alg_data->base_num = 0;
  alloc_workspace(alg_data, space);
}
