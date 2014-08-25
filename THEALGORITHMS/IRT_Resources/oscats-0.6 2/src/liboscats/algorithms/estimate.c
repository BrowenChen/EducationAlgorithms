/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Estimate latent IRT ability
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

#include <math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include "gsl.h"
#include "algorithm.h"
#include "algorithms/estimate.h"

#define MAX_MLE_ITERS 10

enum {
  PROP_0,
  PROP_INDEPENDENT,
  PROP_POSTERIOR,
  PROP_N_POSTERIOR,
  PROP_MU,
  PROP_SIGMA,
  PROP_DPRIOR,
  PROP_TOL,
  PROP_MODEL_KEY,
  PROP_THETA_KEY,
};

G_DEFINE_TYPE(OscatsAlgEstimate, oscats_alg_estimate, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_estimate_dispose (GObject *object);
static void oscats_alg_estimate_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_estimate_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_estimate_class_init (OscatsAlgEstimateClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;
  
  gobject_class->dispose = oscats_alg_estimate_dispose;
  gobject_class->set_property = oscats_alg_estimate_set_property;
  gobject_class->get_property = oscats_alg_estimate_get_property;
  
  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgEstimate:independent:
 *
 * Continuous and discrete dimensions are independent.
 */
  pspec = g_param_spec_boolean("independent", "independent estimation", 
                              "Continuous and discrete dimensions are independent",
                              TRUE,
                              G_PARAM_READWRITE |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_INDEPENDENT, pspec);

/**
 * OscatsAlgEstimate:posterior:
 *
 * Always use Expected A Posteriori (EAP) estimation for continuous
 * dimensions and Maximum A Posteriori (MAP) estimation for discrete
 * dimensions, instead of Maximum Likelihood (MLE).  Note, EAP/MAP is always
 * used as a fallback if the MLE fails to converge, e.g. for perfect
 * response patterns.
 */
  pspec = g_param_spec_boolean("posterior", "Use Posterior", 
                              "Find EAP/MAP instead of MLE",
                              FALSE,
                              G_PARAM_READWRITE |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_POSTERIOR, pspec);

/**
 * OscatsAlgEstimate:Nposterior:
 *
 * For the first N items, use Expected A Posteriori (EAP) estimation for
 * continuous dimensions and Maximum A Posteriori (MAP) estimation for
 * discrete dimensions.  Switch to Maximum Likelihood (MLE) estimation after
 * N items have been recorded.  Note, EAP/MAP is always used as a fallback
 * if the MLE fails to converge, e.g. for perfect response patterns.
 */
  pspec = g_param_spec_uint("Nposterior", "Nposterior", 
                              "Number of items for EAP/MAP",
                              0, G_MAXUINT, 0,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_POSTERIOR, pspec);

/**
 * OscatsAlgEstimate:mu:
 *
 * Population mean for the normal prior under EAP.  (Note: The value is
 * copied.) Default: 0.
 */
  pspec = g_param_spec_object("mu", "Pop mean", 
                              "Population mean for EAP",
                              G_TYPE_GSL_VECTOR,
                              G_PARAM_READWRITE |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_MU, pspec);

/**
 * OscatsAlgEstimate:Sigma:
 *
 * Population covariance matrix for the normal prior under EAP.  (Note: The
 * value is copied.) Default: identity.
 */
  pspec = g_param_spec_object("Sigma", "Pop covariance", 
                              "Population covariance matrix for EAP",
                              G_TYPE_GSL_MATRIX,
                              G_PARAM_READWRITE |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_SIGMA, pspec);

/**
 * OscatsAlgEstimate:Dprior:
 *
 * Prior distribution for discrete dimensions as a vector of probabilities
 * for all Prod_i n_i patterns, where n_i is the number of categories for
 * discrete dimension i.  The values should be ordered so that the lowest
 * numbered binary dimension increases fasted, and the ordinal dimensions
 * follow binary dimensions.  The probabilities should sum to 1.  Default:
 * uniform. Note, this is used only when #OscatsAlgEstimate:posterior is
 * %TRUE.
 */
  pspec = g_param_spec_object("Dprior", "Discrete prior", 
                              "Prior distribution for discrete dimensions",
                              G_TYPE_GSL_VECTOR,
                              G_PARAM_READWRITE | 
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_DPRIOR, pspec);

/**
 * OscatsAlgEstimate:tol:
 *
 * Tolerance for Newton-Raphson maximization (algorithm iterates until
 * largest change in any dimension is less than tolerance).
 */
  pspec = g_param_spec_double("tol", "tolerance", 
                              "Newton-Raphson tolerance",
                              G_MINDOUBLE, G_MAXDOUBLE, 1e-6,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_TOL, pspec);

/**
 * OscatsAlgEstimate:modelKey:
 *
 * The key indicating which model to use for estimation.  A %NULL value or
 * empty string indicates the item's default model.
 */
  pspec = g_param_spec_string("modelKey", "model key", 
                            "Which model to use for estimation",
                            NULL,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_MODEL_KEY, pspec);

/**
 * OscatsAlgEstimate:thetaKey:
 *
 * The key indicating which latent variable to use for estimation.  A %NULL
 * value or empty string indicates the examinee's default estimation theta.
 */
  pspec = g_param_spec_string("thetaKey", "ability key", 
                            "Which latent variable to use for estimation",
                            NULL,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_THETA_KEY, pspec);

}

static void oscats_alg_estimate_init (OscatsAlgEstimate *self)
{
  self->integrator = g_object_new(OSCATS_TYPE_INTEGRATE, NULL);
  self->normalizer = g_object_new(OSCATS_TYPE_INTEGRATE, NULL);
}

static void oscats_alg_estimate_dispose (GObject *object)
{
  OscatsAlgEstimate *self = OSCATS_ALG_ESTIMATE(object);
  G_OBJECT_CLASS(oscats_alg_estimate_parent_class)->dispose(object);
  if (self->mu) g_object_unref(self->mu);
  if (self->Sigma_half) g_object_unref(self->Sigma_half);
  if (self->Dprior) g_object_unref(self->Dprior);
  if (self->integrator) g_object_unref(self->integrator);
  if (self->normalizer) g_object_unref(self->normalizer);
  if (self->x) g_object_unref(self->x);
  if (self->tmp) gsl_vector_free(self->tmp);
  if (self->tmp2) gsl_vector_free(self->tmp2);
  self->mu = NULL;
  self->Sigma_half = NULL;
  self->Dprior = NULL;
  self->integrator = self->normalizer = NULL;
  self->x = NULL;
  self->tmp = self->tmp2 = NULL;
}

static void oscats_alg_estimate_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgEstimate *self = OSCATS_ALG_ESTIMATE(object);
  switch (prop_id)
  {
    case PROP_POSTERIOR:
      self->eap = g_value_get_boolean(value);
      break;

    case PROP_N_POSTERIOR:
      self->Nposterior = g_value_get_uint(value);
      break;

    case PROP_INDEPENDENT:
      self->independent = g_value_get_boolean(value);
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
    
    case PROP_TOL:
      self->tol = g_value_get_double(value);
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

static void oscats_alg_estimate_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgEstimate *self = OSCATS_ALG_ESTIMATE(object);
  switch (prop_id)
  {
    case PROP_POSTERIOR:
      g_value_set_boolean(value, self->eap);
      break;
    
    case PROP_N_POSTERIOR:
      g_value_set_uint(value, self->Nposterior);
      break;
    
    case PROP_INDEPENDENT:
      g_value_set_boolean(value, self->independent);
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
    
    case PROP_TOL:
      g_value_set_double(value, self->tol);
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

// returns TRUE if fails to converge
static gboolean NR(OscatsExaminee *e, OscatsPoint *theta, OscatsAlgEstimate *alg_data)
{
  OscatsItem *item;
  OscatsModel *model;
  GGslVector *grad, *delta;
  GGslMatrix *hes;
  GGslPermutation *perm;
  guint dim, num, i, iters = 0;
  gdouble diff, x;
  gboolean fail = FALSE;

  item = g_ptr_array_index(e->items, 0);
  model = oscats_administrand_get_model(OSCATS_ADMINISTRAND(item), alg_data->modelKey);
  g_return_val_if_fail(model && oscats_space_compatible(model->space, theta->space), TRUE);

  num = e->items->len;
  dim = theta->space->num_cont;
  grad = g_gsl_vector_new(dim);
  delta = g_gsl_vector_new(dim);
  hes = g_gsl_matrix_new(dim, dim);
  perm = g_gsl_permutation_new(dim);
  
  do
  {
    g_gsl_vector_set_all(grad, 0);
    g_gsl_matrix_set_all(hes, 0);
    for (i=0; i < num; i++)
    {
      item = g_ptr_array_index(e->items, i);
      oscats_model_logLik_dtheta(
        oscats_administrand_get_model(OSCATS_ADMINISTRAND(item), alg_data->modelKey),
        e->resp->data[i], theta, e->covariates, grad, hes);
    }
    // delta = hes^(-1) * grad
    g_gsl_matrix_solve(hes, grad, delta, perm);
    // theta <- theta - delta
    diff = 0;
    for (i=0; i < dim; i++)
    {
      x = gsl_vector_get(delta->v, i);
      if (fabs(x) > diff) diff = fabs(x);
      x = (theta->cont[i] -= x);
      if (!isfinite(x)) fail = TRUE;
    }
    if (++iters == MAX_MLE_ITERS) fail = TRUE;
  } while (diff > alg_data->tol && !fail);

  // FIXME: Improve performance by putting persistent instances in alg_data
  g_object_unref(grad);
  g_object_unref(delta);
  g_object_unref(hes);
  g_object_unref(perm);
  return fail;
}

// returns TRUE if fails to converge
static gboolean MLE(OscatsExaminee *e, OscatsPoint *theta, OscatsAlgEstimate *alg_data)
{
  OscatsPoint *x = alg_data->x;
  guint numCont = theta->space->num_cont;
  guint numBin = theta->space->num_bin;
  guint numNat = theta->space->num_nat;
  gboolean haveDiscr = (numBin + numNat > 0);
  gboolean fail = FALSE;
  OscatsNatural i, junk=0, *dimMax;
  gdouble max = -G_MAXDOUBLE, L;
  guint I, J;

  if (numCont > 0 && (alg_data->independent || !haveDiscr))
  {
    fail = NR(e, theta, alg_data);
    if (fail || !haveDiscr) return fail;
  }
  
  g_return_val_if_fail(haveDiscr, TRUE);

  if (numNat > 0) dimMax = theta->space->max;
  else  // Fake a natural dimension
  {
    numNat = 1;
    dimMax = &junk;
  }
  
  oscats_point_copy(x, theta);
  for (I=0; I < numNat; I++)            // natural dimension
    for (i=0; i <= dimMax[I]; i++)         // value of this natural dimension
    {
      if (theta->nat) theta->nat[I] = i;
      if (numBin == 0)
      {
        if (!alg_data->independent && numCont > 0) fail = NR(e, x, alg_data);
        if (fail) return TRUE;
        L = oscats_examinee_logLik(e, x, alg_data->modelKey);
        if (L > max)
        {
          max = L;
          oscats_point_copy(theta, x);
        }
      } else
        for (J=0; J < numBin; J++)      // binary dimension
        {
          // binary dim == 0
          g_bit_array_clear_bit(x->bin, J);
          if (!alg_data->independent && numCont > 0) fail = NR(e, x, alg_data);
          if (fail) return TRUE;
          L = oscats_examinee_logLik(e, x, alg_data->modelKey);
          if (L > max)
          {
            max = L;
            oscats_point_copy(theta, x);
          }
          
          // binary dim == 1
          g_bit_array_set_bit(x->bin, J);
          if (!alg_data->independent && numCont > 0) fail = NR(e, x, alg_data);
          if (fail) return TRUE;
          L = oscats_examinee_logLik(e, x, alg_data->modelKey);
          if (L > max)
          {
            max = L;
            oscats_point_copy(theta, x);
          }
        }
    }

  return FALSE;
}

// Note: the normalizing constant for the Normal prior is canceled.
static gdouble eap_integrand(const GGslVector *x, gpointer data)
{
  OscatsAlgEstimate *self = OSCATS_ALG_ESTIMATE(data);
  gdouble g;
  gsl_vector_memcpy(self->tmp, x->v);
  if (self->mu) gsl_vector_sub(self->tmp, self->mu);
  if (self->Sigma_half)
    gsl_linalg_cholesky_solve(self->Sigma_half, self->tmp, self->tmp2);
  else
    gsl_vector_memcpy(self->tmp2, self->tmp);
  gsl_blas_ddot(self->tmp, self->tmp2, &g);
  return self->x->cont[self->dim] *
         exp(oscats_examinee_logLik(self->e, self->x, self->modelKey) - g/2);
}

static gdouble norm_integrand(const GGslVector *x, gpointer data)
{
  OscatsAlgEstimate *self = OSCATS_ALG_ESTIMATE(data);
  gdouble g;
  gsl_vector_memcpy(self->tmp, x->v);
  if (self->mu) gsl_vector_sub(self->tmp, self->mu);
  if (self->Sigma_half)
    gsl_linalg_cholesky_solve(self->Sigma_half, self->tmp, self->tmp2);
  else
    gsl_vector_memcpy(self->tmp2, self->tmp);
  gsl_blas_ddot(self->tmp, self->tmp2, &g);
  return exp(oscats_examinee_logLik(self->e, self->x, self->modelKey) - g/2);
}

// Note: This is only over cont dimensions, given the discr val of alg_data->x
// Stores the final EAP back in alg_data->x.
static void EAP(OscatsAlgEstimate *alg_data)
{
  guint i, num = alg_data->x->space->num_cont;
  gdouble norm, eap[num];

  for (alg_data->dim=0; alg_data->dim < num; alg_data->dim++)
    eap[alg_data->dim] = oscats_integrate_space(alg_data->integrator, alg_data);
  norm = oscats_integrate_space(alg_data->normalizer, alg_data);
  for (i=0; i < num; i++)
    alg_data->x->cont[i] = eap[i] / norm;
}

// EAP over continuous dimensions, MAP over discrete dimensions
static void MAP(OscatsExaminee *e, OscatsPoint *theta, OscatsAlgEstimate *alg_data)
{
  OscatsPoint *x = alg_data->x;
  guint numCont = theta->space->num_cont;
  guint numBin = theta->space->num_bin;
  guint numNat = theta->space->num_nat;
  gboolean haveDiscr = (numBin + numNat > 0);
  OscatsNatural i, junk=0, *dimMax;
  gdouble max = -G_MAXDOUBLE, L;
  guint I, J;

  oscats_point_copy(x, theta);
  if (numCont > 0 && (alg_data->independent || !haveDiscr))
  {
    EAP(alg_data);
    if (!haveDiscr)
    {
      oscats_point_copy(theta, x);
      return;
    }
  }
  
  g_return_if_fail(haveDiscr);

  if (numNat > 0) dimMax = theta->space->max;
  else  // Fake a natural dimension
  {
    numNat = 1;
    dimMax = &junk;
  }
  
  for (I=0; I < numNat; I++)            // natural dimension
    for (i=0; i <= dimMax[I]; i++)         // value of this natural dimension
    {
      if (theta->nat) theta->nat[I] = i;
      if (numBin == 0)
      {
        if (!alg_data->independent && numCont > 0) EAP(alg_data);
        L = oscats_examinee_logLik(e, x, alg_data->modelKey);
        if (L > max)
        {
          max = L;
          oscats_point_copy(theta, x);
        }
      } else
        for (J=0; J < numBin; J++)      // binary dimension
        {
          // binary dim == 0
          g_bit_array_clear_bit(x->bin, J);
          if (!alg_data->independent && numCont > 0) EAP(alg_data);
          L = oscats_examinee_logLik(e, x, alg_data->modelKey);
          if (L > max)
          {
            max = L;
            oscats_point_copy(theta, x);
          }
          
          // binary dim == 1
          g_bit_array_set_bit(x->bin, J);
          if (!alg_data->independent && numCont > 0) EAP(alg_data);
          L = oscats_examinee_logLik(e, x, alg_data->modelKey);
          if (L > max)
          {
            max = L;
            oscats_point_copy(theta, x);
          }
        }
    }
}

// FIXME: This isn't quite right for multidimensional tests
static void administered (OscatsTest *test, OscatsExaminee *e,
                          OscatsItem *item, guint resp, gpointer alg_data)
{
  OscatsAlgEstimate *self = OSCATS_ALG_ESTIMATE(alg_data);
  OscatsPoint *theta = ( self->thetaKey ?
                           oscats_examinee_get_theta(e, self->thetaKey) :
                           oscats_examinee_get_est_theta(e) );
  guint dims;

  g_return_if_fail(OSCATS_IS_POINT(theta) && OSCATS_IS_SPACE(theta->space));
  dims = theta->space->num_cont;
  if (self->x == NULL || !oscats_point_space_compatible(theta, self->x))
  {
    if (self->x) g_object_unref(self->x);
    self->x = oscats_point_new_from_space(theta->space);
    if (dims > 0)
    {
      oscats_integrate_set_c_function(self->integrator, dims, eap_integrand);
      oscats_integrate_set_c_function(self->normalizer, dims, norm_integrand);
      oscats_integrate_link_point(self->integrator, self->x);
      oscats_integrate_link_point(self->normalizer, self->x);
      if (self->tmp == NULL || self->tmp->size != dims)
      {
        if (self->tmp) gsl_vector_free(self->tmp);
        if (self->tmp2) gsl_vector_free(self->tmp2);
        self->tmp = gsl_vector_alloc(dims);
        self->tmp2 = gsl_vector_alloc(dims);
      }
    }
  }

  if (e->items->len == 0) return;  // First item wasn't recorded
  self->e = e;

  if (self->eap || e->items->len <= self->Nposterior)
  {
    MAP(e, theta, self);
    return;
  }

/*
  if (e->items->len == 1)
  {
    if (resp == 0) self->flag = -1;
    else if (resp == oscats_cont_model_get_max(item->cont_model))
      self->flag = 1;
    else self->flag = 0;
  } else if (self->flag &&
      (resp != (self->flag == 1 ? oscats_cont_model_get_max(item->cont_model)
                                 : 0) ))
      self->flag = 0;
  if (self->flag)
    MAP(e, theta, self);
  else
*/
    if (MLE(e, theta, self))
      MAP(e, theta, self);
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
//  OscatsAlgEstimate *self = OSCATS_ALG_ESTIMATE(alg_data);

  g_signal_connect_data(test, "administered", G_CALLBACK(administered),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}
