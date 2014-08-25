/* OSCATS: Open-Source Computerized Adaptive Testing System
 * One-Parameter Logistic IRT Model
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
 * SECTION:l1p
 * @title:OscatsModelL1p
 * @short_description: One-Parameter Logistic (1PL) Model
 */

#include <math.h>
#include <gsl/gsl_vector.h>
#include "models/l1p.h"

G_DEFINE_TYPE(OscatsModelL1p, oscats_model_l1p, OSCATS_TYPE_MODEL);

enum
{
  PARAM_B,
  NUM_PARAMS,
};

static void model_constructed (GObject *object);
static OscatsResponse get_max (const OscatsModel *model);
static gdouble P(const OscatsModel *model, OscatsResponse resp, const OscatsPoint *theta, const OscatsCovariates *covariates);
static gdouble distance(const OscatsModel *model, const OscatsPoint *theta, const OscatsCovariates *covariates);
static void logLik_dtheta(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes, gboolean Inf);
static void logLik_dparam(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes);

static void oscats_model_l1p_class_init (OscatsModelL1pClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsModelClass *model_class = OSCATS_MODEL_CLASS(klass);

  gobject_class->constructed = model_constructed;

  model_class->get_max = get_max;
  model_class->P = P;
  model_class->distance = distance;
  model_class->logLik_dtheta = logLik_dtheta;
  model_class->logLik_dparam = logLik_dparam;
  
}

static void oscats_model_l1p_init (OscatsModelL1p *self)
{
}

static void model_constructed(GObject *object)
{
  OscatsModel *model = OSCATS_MODEL(object);
  guint i;
  G_OBJECT_CLASS(oscats_model_l1p_parent_class)->constructed(object);

  // Set up parameters
  model->Np = NUM_PARAMS + model->Ncov;
  model->params = g_new0(gdouble, model->Np);

  // Set up parameter names
  model->names = g_new(GQuark, model->Np);
  model->names[PARAM_B] = g_quark_from_string("Diff");
  for (i=0; i < model->Ncov; i++)
    model->names[NUM_PARAMS+i] = model->covariates[i];
  
  // Check subspace type
  if (model->dimType != OSCATS_DIM_CONT)
  {
    model->Ndims = 0;
    g_critical("OscatsModelL1p requires a continuous latent space.");
  }

}

static OscatsResponse get_max (const OscatsModel *model)
{
  return 1;
}

static gdouble P(const OscatsModel *model, OscatsResponse resp,
                 const OscatsPoint *theta, const OscatsCovariates *covariates)
{
  guint *dims = model->shortDims;
  guint i;
  gdouble z = 0;
  g_return_val_if_fail(resp <= 1, 0);
  switch (model->Ndims)
  {
    case 2:
      z = -theta->cont[dims[1]];
    case 1:
      z -= theta->cont[dims[0]];
      break;
    
    default:
      for (i=0; i < model->Ndims; i++)
        z -= theta->cont[dims[i]];
  }
  for (i=0; i < model->Ncov; i++)
    z -= oscats_covariates_get(covariates, model->covariates[i]) *
         model->params[NUM_PARAMS+i];
  z += model->params[PARAM_B];
  return 1/(1+exp(resp ? z : -z));
}

static gdouble distance(const OscatsModel *model, const OscatsPoint *theta,
                        const OscatsCovariates *covariates)
{
  guint *dims = model->shortDims;
  guint i;
  gdouble z = 0;
  switch (model->Ndims)
  {
    case 2:
      z = theta->cont[dims[1]];
    case 1:
      z += theta->cont[dims[0]];
      break;
    
    default:
      for (i=0; i < model->Ndims; i++)
        z += theta->cont[dims[i]];
  }
  for (i=0; i < model->Ncov; i++)
    z += oscats_covariates_get(covariates, model->covariates[i]) *
         model->params[NUM_PARAMS+i];
  z -= model->params[PARAM_B];
  return fabs(z);
}

/* d[log(P), theta_i] = Q
 * d[log(Q), theta_i] = -P
 * d[log(P), theta_i, theta_j] = -PQ
 * d[log(Q), theta_i, theta_j] = -PQ
 */
static void logLik_dtheta(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes, gboolean Inf)
{
  gsl_vector *grad_v = (grad ? grad->v : NULL);
  gsl_matrix *hes_v = (hes ? hes->v : NULL);
  guint dim1, dim2;
  guint i, j, I, J;
  guint hes_stride = (hes ? hes_v->tda : 0);
  gdouble p, grad_val, hes_val;
  g_return_if_fail(resp <= 1);
  p = P(model, 1, theta, covariates);
  if (resp) grad_val = (1-p);
  else      grad_val = -p;
  hes_val = -p*(1-p);
  if (Inf) hes_val *= -(resp ? p : 1-p);
  dim1 = model->shortDims[0];
  switch (model->Ndims)
  {
    case 2:
      dim2 = model->shortDims[1];
      if (grad) grad_v->data[dim2 * grad_v->stride] += grad_val;
      if (hes)
      {
        hes_v->data[dim2 * hes_stride + dim2] += hes_val;
        hes_v->data[dim1 * hes_stride + dim2] += hes_val;
        hes_v->data[dim2 * hes_stride + dim1] += hes_val;
      }
    case 1:
      if (grad) grad_v->data[dim1 * grad_v->stride] += grad_val;
      if (hes)
        hes_v->data[dim1 * hes_stride + dim1] += hes_val;
      break;
    
    default:
      for (i=0; i < model->Ndims; i++)
      {
        I = model->shortDims[i];
        if (grad) grad_v->data[I*grad_v->stride] += grad_val;
        if (hes)
        {
          hes_v->data[I*hes_stride + I] += hes_val;
          for (j=i+1; j < model->Ndims; j++)
          {
            J = model->shortDims[j];
            hes_v->data[I*hes_stride + J] += hes_val;
            hes_v->data[J*hes_stride + I] += hes_val;
          }
        }
      }
  }
}

/* d[log(P), b] = -Q = P-1
 * d[log(Q), b] = P
 * d[log(P), b^2] = -PQ
 * d[log(Q), b^2] = -PQ
 */
static void logLik_dparam(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes)
{
  gdouble p, grad_val, hes_val;
  g_return_if_fail(resp <= 1);
  p = P(model, 1, theta, covariates);
  if (resp) grad_val = p-1;
  else      grad_val = p;
  hes_val = -p*(1-p);
  if (grad) grad->v->data[PARAM_B] += grad_val;
  if (hes) hes->v->data[PARAM_B] += hes_val;
  if (model->Ncov > 0)
  {
    gdouble val, val2;
    gsl_vector *grad_v = (grad ? grad->v : NULL);
    gsl_matrix *hes_v = (hes ? hes->v : NULL);
    guint i, j;
    guint hes_stride = (hes ? hes_v->tda : 0);
    grad_val = -grad_val;
    for (i=NUM_PARAMS; i < model->Np; i++)
    {
      val = oscats_covariates_get(covariates, model->covariates[i-NUM_PARAMS]);
      if (grad_v) grad_v->data[i*grad_v->stride] += val * grad_val;
      if (hes_v)
      {
        hes_v->data[i*hes_stride + i] += val * val * hes_val;
        hes_v->data[i*hes_stride + PARAM_B] += -val * hes_val;
        hes_v->data[PARAM_B*hes_stride + i] += -val * hes_val;
        for (j=i+1; j < model->Np; j++)
        {
          val2 = oscats_covariates_get(covariates, model->covariates[j-NUM_PARAMS]);
          hes_v->data[i*hes_stride + j] += val * val2 * hes_val;
          hes_v->data[j*hes_stride + i] += val * val2 * hes_val;
        }
      }
    }
  } // covariates
}

