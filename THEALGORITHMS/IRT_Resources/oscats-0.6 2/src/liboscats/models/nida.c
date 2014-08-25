/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Noisy Inputs Deterministic And Gate (NIDA) Classification Model
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
 * SECTION:nida
 * @title:OscatsModelNida
 * @short_description: Noisy Input Determistic And Gate (NIDA) Model
 */

#include <math.h>
#include <gsl/gsl_vector.h>
#include "models/nida.h"

enum
{
  PARAM_GUESS,
  PARAM_SLIP,
};

G_DEFINE_TYPE(OscatsModelNida, oscats_model_nida, OSCATS_TYPE_MODEL);

static void model_constructed (GObject *object);
static OscatsResponse get_max (const OscatsModel *model);
static gdouble P(const OscatsModel *model, OscatsResponse resp,
                 const OscatsPoint *theta, const OscatsCovariates *covariates);
static void logLik_dparam(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes);

static void oscats_model_nida_class_init (OscatsModelNidaClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsModelClass *model_class = OSCATS_MODEL_CLASS(klass);

  gobject_class->constructed = model_constructed;

  model_class->get_max = get_max;
  model_class->P = P;
  model_class->logLik_dparam = logLik_dparam;
  
}

static void oscats_model_nida_init (OscatsModelNida *self)
{
}

static void model_constructed(GObject *object)
{
  OscatsModel *model = OSCATS_MODEL(object);
  GString *str;
  guint i;
  G_OBJECT_CLASS(oscats_model_nida_parent_class)->constructed(object);

  // Set up parameters
  model->Np = 2*model->Ndims;
  model->params = g_new0(gdouble, model->Np);

  // Set up parameter names
  str = g_string_sized_new(10);
  model->names = g_new(GQuark, model->Np);
  for (i=0; i < model->Ndims; i++)
  {
    g_string_printf(str, "Guess.%s",
                    oscats_space_dim_get_name(model->space, model->dims[i]));
    model->names[PARAM_GUESS*model->Ndims+i] = g_quark_from_string(str->str);
    g_string_printf(str, "Diff.%s",
                    oscats_space_dim_get_name(model->space, model->dims[i]));
    model->names[PARAM_SLIP*model->Ndims+i] = g_quark_from_string(str->str);
  }
  g_string_free(str, TRUE);
  
  // Check subspace type
  if (model->dimType != OSCATS_DIM_BIN)
  {
    model->Ndims = 0;
    g_critical("OscatsModelNida requires a binary latent space."); 
  }

}

static OscatsResponse get_max (const OscatsModel *model)
{
  return 1;
}

static gdouble P(const OscatsModel *model, OscatsResponse resp,
                 const OscatsPoint *theta, const OscatsCovariates *covariates)
{
  gdouble p = 1;
  guint *dims = model->shortDims;
  guint i;
  g_return_val_if_fail(resp <= 1, 0);
  for (i=0; i < model->Ndims; i++)
    p *= (g_bit_array_get_bit(theta->bin, dims[i])
          ? 1-model->params[PARAM_SLIP*model->Ndims+i]
          : model->params[PARAM_GUESS*model->Ndims+i]);
  return (resp ? p : 1-p);
}

static void logLik_dparam(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes)
{
  gdouble *g = (grad ? grad->v->data : NULL);
  gdouble *H = (hes ? hes->v->data : NULL);
  gdouble p, logit_p, grad_val, hes_val;
  guint *dims = model->shortDims;
  guint j, k, Ndims = model->Ndims;
  guint grad_stride = (grad ? grad->v->stride : 0);
  guint hes_stride = (hes ? hes->v->tda : 0);
  g_return_if_fail(resp <= 1);

#define GRAD(t, j) (g[((t)*Ndims+(j))*grad_stride])
#define HES(t1, j, t2, k) (H[((t1)*Ndims+(j))*hes_stride + ((t2)*Ndims+(k))])

  if (resp)
    for (j=0; j < Ndims; j++)
      if (g_bit_array_get_bit(theta->bin, dims[j]))
      {
        grad_val = 1 / (model->params[PARAM_SLIP*Ndims+j] - 1);
        if (g) GRAD(PARAM_SLIP, j) += grad_val;
        if (H) HES(PARAM_SLIP, j, PARAM_SLIP, j) -= grad_val*grad_val;
      } else {		// alpha_j == 0
        grad_val = 1 / model->params[PARAM_GUESS*Ndims+j];
        if (g) GRAD(PARAM_GUESS, j) += grad_val;
        if (H) HES(PARAM_GUESS, j, PARAM_GUESS, j) -= grad_val*grad_val;
      }
  else 			// !resp
  {
    p = P(model, 0, theta, covariates);
    logit_p = p/(1-p);
    for (j=0; j < Ndims; j++)
      if (g_bit_array_get_bit(theta->bin, dims[j]))
      {
        grad_val = logit_p/(1-model->params[PARAM_SLIP*Ndims+j]);
        GRAD(PARAM_SLIP, j) += grad_val;
        HES(PARAM_SLIP, j, PARAM_SLIP, j) -= grad_val*grad_val;
        grad_val /= (1-p);
        for (k=j+1; k < Ndims; k++)
          if (g_bit_array_get_bit(theta->bin, dims[k]))
          {
            hes_val = grad_val / (1-model->params[PARAM_SLIP*Ndims+k]);
            HES(PARAM_SLIP, j, PARAM_SLIP, k) -= hes_val;
            HES(PARAM_SLIP, k, PARAM_SLIP, j) -= hes_val;
          } else {	// alpha_k == 0
            hes_val = grad_val / model->params[PARAM_GUESS*Ndims+k];
            HES(PARAM_SLIP, j, PARAM_GUESS, k) += hes_val;
            HES(PARAM_GUESS, k, PARAM_SLIP, j) += hes_val;
          }
      } else {		// alpha_j == 0
        grad_val = logit_p / model->params[PARAM_GUESS*Ndims+j];
        GRAD(PARAM_GUESS, j) -= grad_val;
        HES(PARAM_GUESS, j, PARAM_GUESS, j) -= grad_val*grad_val;
        grad_val /= (1-p);
        for (k=j+1; k < Ndims; k++)
          if (g_bit_array_get_bit(theta->bin, dims[k]))
          {
            hes_val = grad_val / (1-model->params[PARAM_SLIP*Ndims+k]);
            HES(PARAM_GUESS, j, PARAM_SLIP, k) += hes_val;
            HES(PARAM_SLIP, k, PARAM_GUESS, j) += hes_val;
          } else {	// alpha_k == 0
            hes_val = grad_val / model->params[PARAM_GUESS*Ndims+k];
            HES(PARAM_GUESS, j, PARAM_GUESS, k) += hes_val;
            HES(PARAM_GUESS, k, PARAM_GUESS, j) += hes_val;
          }
      }
  }
}
