/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Heterogeneous Logistic Graded Response IRT Model
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
 * SECTION:hlgr
 * @title:OscatsModelHetlgr
 * @short_description: Heterogeneous Logistic Graded Response Model
 */

#include <math.h>
#include <gsl/gsl_vector.h>
#include "models/hlgr.h"

G_DEFINE_TYPE(OscatsModelHetlgr, oscats_model_hetlgr, OSCATS_TYPE_MODEL);

/* There are Ncat+1 response categories, 0, 1, ..., Ncat.
 * P_0 = 1, so there are Ncat sets of discrimination and location parameters,
 * numbered 1, ..., Ncat.
 */
#define PARAM_B(k) ((k)-1)
#define PARAM_A(k,i) (Ncat+((k)-1)*Ncat+(i))
#define PARAM_D(j) (Ncat+Ncat*Ndims+(j))

enum
{
  PROP_0,
  PROP_NUM_CAT,
};

static void model_constructed (GObject *object);
static void model_set_property(GObject *object, guint prop_id,
                               const GValue *value, GParamSpec *pspec);
static void model_get_property(GObject *object, guint prop_id,
                               GValue *value, GParamSpec *pspec);
static OscatsResponse get_max (const OscatsModel *model);
static gdouble P(const OscatsModel *model, OscatsResponse resp, const OscatsPoint *theta, const OscatsCovariates *covariates);
//static gdouble distance(const OscatsModel *model, const OscatsPoint *theta, const OscatsCovariates *covariates);
static void logLik_dtheta(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes, gboolean Inf);
static void logLik_dparam(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes);

static void oscats_model_hetlgr_class_init (OscatsModelHetlgrClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsModelClass *model_class = OSCATS_MODEL_CLASS(klass);
  GParamSpec *pspec;
  
  gobject_class->constructed = model_constructed;
  gobject_class->set_property = model_set_property;
  gobject_class->get_property = model_get_property;

  model_class->get_max = get_max;
  model_class->P = P;
//  model_class->distance = distance;
  model_class->logLik_dtheta = logLik_dtheta;
  model_class->logLik_dparam = logLik_dparam;
  
/**
 * OscatsModelHetlgr:Ncat:
 *
 * The number of response categories in the IRT model,
 * numered 0, 1, ..., Ncat-1.  The default number of categories is 3,
 * which is also the minimum.
 */
  pspec = g_param_spec_uint("Ncat", "Number of Categories", 
                            "Number of response categories in the IRT model",
                            3, G_MAXUINT, 3,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM_CAT, pspec);

}

static void oscats_model_hetlgr_init (OscatsModelHetlgr *self)
{
}

static void model_constructed(GObject *object)
{
  GString *str;
  guint i, k, Ncat, Ndims;
  OscatsModel *model = OSCATS_MODEL(object);
  G_OBJECT_CLASS(oscats_model_hetlgr_parent_class)->constructed(object);
  Ncat = OSCATS_MODEL_HETLGR(model)->Ncat;
  Ndims = OSCATS_MODEL(model)->Ndims;

  // Set up parameters
  model->Np = Ncat + Ndims*Ncat + model->Ncov;
  model->params = g_new0(gdouble, model->Np);

  // Set up parameter names
  str = g_string_sized_new(10);
  model->names = g_new(GQuark, model->Np);
  for (k=0; k < Ncat; k++)
  {
    g_string_printf(str, "Diff.%d", k+1);
    model->names[PARAM_B(k)] = g_quark_from_string(str->str);
    for (i=0; i < model->Ndims; i++)
    {
      g_string_printf(str, "Discr.%d.%s", k+1,
                      oscats_space_dim_get_name(model->space, model->dims[i]));
      model->names[PARAM_A(k,i)] = g_quark_from_string(str->str);
    }
  }
  g_string_free(str, TRUE);
  for (i=0; i < model->Ncov; i++)
    model->names[PARAM_D(i)] = model->covariates[i];

  // Check subspace type
  if (model->dimType != OSCATS_DIM_CONT)
  {
    model->Ndims = 0;
    g_critical("OscatsModelHetlgr requires a continuous latent space.");
  }

}

static void model_set_property(GObject *object, guint prop_id,
                               const GValue *value, GParamSpec *pspec)
{
  OscatsModelHetlgr *self = OSCATS_MODEL_HETLGR(object);
  switch (prop_id)
  {
    case PROP_NUM_CAT:			// construction only
      self->Ncat = g_value_get_uint(value)-1;
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void model_get_property(GObject *object, guint prop_id,
                               GValue *value, GParamSpec *pspec)
{
  OscatsModelHetlgr *self = OSCATS_MODEL_HETLGR(object);
  switch (prop_id)
  {
    case PROP_NUM_CAT:
      g_value_set_uint(value, self->Ncat+1);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static OscatsResponse get_max (const OscatsModel *model)
{
  return ((OscatsModelHetlgr*)model)->Ncat;
}

// Only use this for k in { 1, ..., Ncat } because P_0 = 1, P_{Ncat+1} = 0.
static gdouble P_star(const OscatsModel *model, OscatsResponse k,
                 const OscatsPoint *theta, const OscatsCovariates *covariates)
{
  guint *dims = model->shortDims;
  guint Ndims = model->Ndims;
  guint Ncat = ((OscatsModelHetlgr*)model)->Ncat;
  guint i;
  gdouble z = 0;
  switch (model->Ndims)
  {
    case 2:
      z = -model->params[PARAM_A(k,1)] * theta->cont[dims[1]];
    case 1:
      z -= model->params[PARAM_A(k,0)] * theta->cont[dims[0]];
      break;
    
    default:
      for (i=0; i < model->Ndims; i++)
        z -= model->params[PARAM_A(k,i)] * theta->cont[dims[i]];
  }
  for (i=0; i < model->Ncov; i++)
    z -= model->params[PARAM_D(i)] *
         oscats_covariates_get(covariates, model->covariates[i]);
  z += model->params[PARAM_B(k)];
  return 1/(1+exp(z));
}

static gdouble P(const OscatsModel *model, OscatsResponse resp,
                 const OscatsPoint *theta, const OscatsCovariates *covariates)
{
  guint Ncat = ((OscatsModelHetlgr*)model)->Ncat;
  g_return_val_if_fail(resp <= Ncat, 0);
  return (resp == 0 ? 1 : P_star(model, resp, theta, covariates)) -
         (resp == Ncat ? 0 : P_star(model, resp+1, theta, covariates));
}

/* See below for general derivatives.
 * dz_k/dtheta_i = a_ki
 */
static void logLik_dtheta(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes, gboolean Inf)
{
  gsl_vector *grad_v = (grad ? grad->v : NULL);
  gsl_matrix *hes_v = (hes ? hes->v : NULL);
  guint Ndims = model->Ndims;
  guint Ncat = ((OscatsModelHetlgr*)model)->Ncat;
  guint i, j, I, J;
  guint hes_stride = (hes ? hes_v->tda : 0);
  gdouble p, pq_k, pq_kk, pqqp_k, pqqp_kk, inf_factor;
  gdouble a_ki, a_kki, a_kj, a_kkj;
  g_return_if_fail(resp <= Ncat);

  if (resp == 0)
  {
    pq_k = pqqp_k = 0;
    p = 1;
  }
  else
  {
    p = P_star(model, resp, theta, covariates);
    pq_k = p * (1-p);
    pqqp_k = pq_k * (1-2*p);
  }
  if (resp == Ncat)
    pq_kk = pqqp_kk = 0;
  else
  {
    gdouble pstar = P_star(model, resp+1, theta, covariates);
    pq_kk = pstar * (1-pstar);
    pqqp_kk = pq_kk * (1-2*pstar);
    p -= pstar;
  }
  pq_k /= p;
  pq_kk /= p;
  pqqp_k /= p;
  pqqp_kk /= p;
  inf_factor = (Inf ? -p : 1);

  switch (Ndims)
  {
    case 2:
      I = model->shortDims[1];
      J = model->shortDims[0];
      if (resp == 0) a_ki = a_kj = 0;
      else
      {
        a_ki = model->params[PARAM_A(resp,1)];
        a_kj = model->params[PARAM_A(resp,0)];
      }
      if (resp == Ncat) a_kki = a_kkj = 0;
      else
      {
        a_kki = model->params[PARAM_A(resp+1,1)];
        a_kkj = model->params[PARAM_A(resp+1,0)];
      }
      if (grad) grad_v->data[I] += pq_k * a_ki - pq_kk * a_kki;
      if (hes)
      {
        gdouble tmp;
        hes_v->data[I*hes_stride+I] += inf_factor *
          (a_ki*a_ki*pqqp_k - a_kki*a_kki*pqqp_kk
           - (a_ki*pq_k - a_kki*pq_kk) * (a_ki*pq_k - a_kki*pq_kk));
        tmp = inf_factor *
          (a_ki*a_kj*pqqp_k - a_kki*a_kkj*pqqp_kk
           - (a_ki*pq_k - a_kki*pq_kk) * (a_kj*pq_k - a_kkj*pq_kk));
        hes_v->data[I*hes_stride+J] += tmp;
        hes_v->data[J*hes_stride+I] += tmp;
      }
    case 1:
      J = model->shortDims[0];
      a_kj = (resp == 0 ? 0 : model->params[PARAM_A(resp,0)]);
      a_kkj = (resp == Ncat ? 0 : model->params[PARAM_A(resp+1,0)]);
      if (grad) grad_v->data[J] += pq_k * a_kj - pq_kk * a_kkj;
      if (hes)
        hes_v->data[J*hes_stride+J] += inf_factor *
          (a_kj*a_kj*pqqp_k - a_kkj*a_kkj*pqqp_kk
           - (a_kj*pq_k - a_kkj*pq_kk) * (a_kj*pq_k - a_kkj*pq_kk));
      break;
    
    default:
      for (j=0; j < Ndims; j++)
      {
        J = model->shortDims[j];
        a_kj = (resp == 0 ? 0 : model->params[PARAM_A(resp,j)]);
        a_kkj = (resp == Ncat ? 0 : model->params[PARAM_A(resp+1,j)]);
        if (grad) grad_v->data[J] += pq_k * a_kj - pq_kk * a_kkj;
        if (hes)
        {
          hes_v->data[J*hes_stride+J] += inf_factor *
            (a_kj*a_kj*pqqp_k - a_kkj*a_kkj*pqqp_kk
             - (a_kj*pq_k - a_kkj*pq_kk) * (a_kj*pq_k - a_kkj*pq_kk));
          for (i=j+1; i < Ndims; i++)
          {
            gdouble tmp;
            I = model->shortDims[i];
            a_ki = (resp == 0 ? 0 : model->params[PARAM_A(resp,i)]);
            a_kki = (resp == Ncat ? 0 : model->params[PARAM_A(resp+1,i)]);
            tmp = inf_factor *
              (a_ki*a_kj*pqqp_k - a_kki*a_kkj*pqqp_kk
               - (a_ki*pq_k - a_kki*pq_kk) * (a_kj*pq_k - a_kkj*pq_kk));
            hes_v->data[I*hes_stride+J] += tmp;
            hes_v->data[J*hes_stride+I] += tmp;
          }
        } // if hes
      } // for j
      break;
  } // switch Ndims
}

/* z_k = sum_i a_ki theta_i - b_k + sum_j d_j covariate_j
 * P_k = 1/[1+exp(-z_k)], Q_k = 1-P_k, P_0 = 1, P_Ncat+1 = 0
 * Note, since z_k is linear, dz_k/dAdB = 0
 *
 * dP_k/dA = P_k Q_k dz_k/dA
 * d P_k Q_k / dA = P_k Q_k (Q_k - P_k) dz_k/dA
 * dP_k/dAdB = P_k Q_k (Q_k - P_k) [dz_k/dA] [dz_k/dB]
 * dP/dA = P_k Q_k dz_k/dA - P_k+1 Q_k+1 dz_k+1/dA
 * 
 * d log P / dA = [P_k Q_k dz_k/dA - P_k+1 Q_k+1 dz_k+1/dA] / P
 * d log P / dAdB = 
 *    [P_k Q_k (Q_k - P_k) dz_k/dA dz_k/dB - P_k+1 Q_k+1 (Q_k+1 - P_k+1) dz_k+1/dA dz_k+1/dB] / P
 *  - [P_k Q_k dz_k/dA - P_k+1 Q_k+1 dz_k+1/dA] [P_k Q_k dz_k/dB - P_k+1 Q_k+1 dz_k+1/dB] / P^2
 *
 * dz_k/da_ki = theta_i
 * dz_k/db_k = -1
 * dz_k/dd_j = covariate_j
 */
static void logLik_dparam(const OscatsModel *model, OscatsResponse k,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes)
{
  gsl_vector *grad_v = (grad ? grad->v : NULL);
  gsl_matrix *hes_v = (hes ? hes->v : NULL);
  guint *dims = model->shortDims;
  guint Ndims = model->Ndims;
  guint Ncov = model->Ncov;
  guint Ncat = ((OscatsModelHetlgr*)model)->Ncat;
  guint hes_stride = (hes ? hes_v->tda : 0);
  guint i, j, I, J;
  gdouble p, pq_k, pq_kk, pqqp_k, pqqp_kk;
  gdouble zk_a=0, zkk_a=0, zk_b=0, zkk_b=0, theta_1, theta_2, tmp;
  g_return_if_fail(k <= Ncat);

  if (k == 0)
  {
    pq_k = pqqp_k = 0;
    p = 1;
  }
  else
  {
    p = P_star(model, k, theta, covariates);
    pq_k = p * (1-p);
    pqqp_k = pq_k * (1-2*p);
  }
  if (k == Ncat)
    pq_kk = pqqp_kk = 0;
  else
  {
    gdouble pstar = P_star(model, k+1, theta, covariates);
    pq_kk = pstar * (1-pstar);
    pqqp_kk = pq_kk * (1-2*pstar);
    p -= pstar;
  }
  pq_k /= p;
  pq_kk /= p;
  pqqp_k /= p;
  pqqp_kk /= p;

#define DO_GRAD(i) if(grad) grad_v->data[(i)*grad_v->stride] += \
                              pq_k * zk_a - pq_kk * zkk_a;
#define DO_VAR(i) hes_v->data[(i)*hes_stride+(i)] += \
                   zk_a*zk_a*pqqp_k - zkk_a*zkk_a*pqqp_kk \
                   - (zk_a*pq_k - zkk_a*pq_kk) * (zk_b*pq_k - zkk_b*pq_kk);
#define DO_COV(i,j) { tmp = zk_a*zk_b*pqqp_k - zkk_a*zkk_b*pqqp_kk \
                   - (zk_a*pq_k - zkk_a*pq_kk) * (zk_b*pq_k - zkk_b*pq_kk); \
                  hes_v->data[(i)*hes_stride+(j)] += tmp; \
                  hes_v->data[(j)*hes_stride+(i)] += tmp; }
  
  // Discrimination (a_ki, a_k+1,i)
  for (i=0; i < Ndims; i++)
  {
    theta_1 = theta->cont[dims[i]];
    if (k > 0)
    {
      zk_a = theta_1;  zkk_a = 0;
      DO_GRAD(PARAM_A(k,i));
      if (hes)
      {
        DO_VAR(PARAM_A(k,i));
        zk_b = -1;  zkk_b = 0;
        DO_COV(PARAM_A(k,i), PARAM_B(k));
        if (k < Ncat)
        {
          zk_b = 0;  zkk_b = theta_1;
          DO_COV(PARAM_A(k,i), PARAM_A(k+1,i));
          zk_b = 0;  zkk_b = -1;
          DO_COV(PARAM_A(k,i), PARAM_B(k+1));
        }
      }
    }
    if (k < Ncat)
    {
      zk_a = 0; zkk_a = theta_1;
      DO_GRAD(PARAM_A(k+1,i));
      if (hes)
      {
        DO_VAR(PARAM_A(k+1,i));
        zk_b = 0;  zkk_b = -1;
        DO_COV(PARAM_A(k+1,i), PARAM_B(k+1));
        if (k > 0)
        {
          zk_b = -1;  zkk_b = 0;
          DO_COV(PARAM_A(k+1,i), PARAM_B(k));
        }
      }
    }
    if (hes)
    {
      for (j=i+1; j < Ndims; j++)
      {
        theta_2 = theta->cont[dims[j]];
        if (k > 0)
        {
          zk_a = theta_1;  zkk_a = 0;
          zk_b = theta_2;  zkk_b = 0;
          DO_COV(PARAM_A(k,i), PARAM_A(k,j));
          if (k < Ncat)
          {
            zk_b = 0;  zkk_b = theta_2;
            DO_COV(PARAM_A(k,i), PARAM_A(k+1,j));
          }
        }
        if (k < Ncat)
        {
          zk_a = 0;  zkk_a = theta_1;
          zk_b = 0;  zkk_b = theta_2;
          DO_COV(PARAM_A(k+1,i), PARAM_A(k+1,j));
          if (k > 0)
          {
            zk_b = theta_2;  zkk_b = 0;
            DO_COV(PARAM_A(k+1,i), PARAM_A(k,j));
          }
        }
      }
      for (j=0, J=PARAM_D(0); j < Ncov; j++, J++)
      {
        zk_b = zkk_b = oscats_covariates_get(covariates, model->covariates[j]);
        if (k > 0)
        {
          zk_a = theta_1;  zkk_a = 0;
          DO_COV(PARAM_A(k,i), J);
        }
        if (k < Ncov)
        {
          zk_a = 0;  zkk_a = theta_1;
          DO_COV(PARAM_A(k+1,i), J);
        }
      }
    }
  } // each dimension
    
  // Covariates (d_j)
  for (j=0, J=PARAM_D(0); j < Ncov; j++, J++)
  {
    zk_a = zkk_a = oscats_covariates_get(covariates, model->covariates[j]);
    DO_GRAD(J);
    if (hes)
    {
      DO_VAR(J);
      if (k > 0)
      {
        zk_b = -1;  zkk_b = 0;
        DO_COV(J,PARAM_B(k));
      }
      if (k < Ncov)
      {
        zk_b = 0;  zkk_b = -1;
        DO_COV(J,PARAM_B(k+1));
      }
      for (i=j+1, I=J+1; i < Ncov; i++, I++)
      {
        zk_b = zkk_b = oscats_covariates_get(covariates, model->covariates[i]);
        DO_COV(J,I);
      }
    }  
  } // each covariate

  // Location (b_k, b_k+1)
  if (k > 0)
  {
    zk_a = -1;  zkk_b = 0;
    DO_GRAD(PARAM_B(k));
    if (hes)
    {
      DO_VAR(PARAM_B(k));
      if (k < Ncov)
      {
        zk_b = 0;  zkk_b = -1;
        DO_COV(PARAM_B(k), PARAM_B(k+1));
      }
    }
  }
  if (k < Ncov)
  {
    zk_a = 0;  zkk_a = -1;
    DO_GRAD(PARAM_B(k+1));
    if (hes) DO_VAR(PARAM_B(k+1));
  }

}
