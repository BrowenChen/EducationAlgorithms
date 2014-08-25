/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Abstract Measurement Model Class
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
 * SECTION:model
 * @title:OscatsModel
 * @short_description: Abstract Measurement Model Class
 */

#include "model.h"

G_DEFINE_ABSTRACT_TYPE(OscatsModel, oscats_model, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_SPACE,
  PROP_N_DIMS,
  PROP_DIMS,
  PROP_DIM_TYPE,
  PROP_N_PARAMS,
  PROP_PARAM_NAMES,
  PROP_N_COV,
  PROP_COVARIATES,
};

static void oscats_model_dispose (GObject *object);
static void oscats_model_finalize (GObject *object);
static void oscats_model_constructed (GObject *object);
static void oscats_model_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_model_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);
static OscatsResponse null_get_max (const OscatsModel *model)
{
  g_critical("Abstract OscatsModel should have overloaded get_max().");
  return 0;
}
static gdouble null_P (const OscatsModel *model, OscatsResponse resp,
                       const OscatsPoint *theta, const OscatsCovariates *covariates)
{
  g_critical("Abstract OscatsModel should have overloaded P().");
  return 0;
}
static gdouble null_distance (const OscatsModel *model,
                              const OscatsPoint *theta, const OscatsCovariates *covariates)
{
  g_critical("%s does not support distance.", G_OBJECT_TYPE_NAME(model));
  return 0;
}
static void null_logLik_theta (const OscatsModel *model, OscatsResponse resp,
                         const OscatsPoint *theta, const OscatsCovariates *covariates,
                         GGslVector *grad, GGslMatrix *hes, gboolean inf)
{
  g_critical("%s does not support logLik_dtheta", G_OBJECT_TYPE_NAME(model));
}
static void null_logLik_param (const OscatsModel *model, OscatsResponse resp,
                               const OscatsPoint *theta, const OscatsCovariates *covariates,
                               GGslVector *grad, GGslMatrix *hes)
{
  g_critical("%s does not support logLik_dparam", G_OBJECT_TYPE_NAME(model));
}

static void oscats_model_class_init (OscatsModelClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec, *espec;

  gobject_class->constructed = oscats_model_constructed;
  gobject_class->dispose = oscats_model_dispose;
  gobject_class->finalize = oscats_model_finalize;
  gobject_class->set_property = oscats_model_set_property;
  gobject_class->get_property = oscats_model_get_property;
  
  klass->get_max = null_get_max;
  klass->P = null_P;
  klass->distance = null_distance;
  klass->logLik_dtheta = null_logLik_theta;
  klass->logLik_dparam = null_logLik_param;
  
/**
 * OscatsModel:space:
 *
 * The latent space for the test.  This <emphasis>must</emphasis> be
 * supplied at construction.
 */
  pspec = g_param_spec_object("space", "Test Space", 
                            "Latent space for the test",
                            OSCATS_TYPE_SPACE,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_SPACE, pspec);

/**
 * OscatsModel:Ndims:
 *
 * The dimension of the IRT model (<= the dimension of the test).
 * Determined from #OscatsModel:dims.  Default is 1.
 */
  pspec = g_param_spec_uint("Ndims", "Num Dims", 
                            "Dimension of the model's subspace",
                            1, G_MAXUINT, 1,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_DIMS, pspec);

/**
 * OscatsModel:dims:
 *
 * An array of #OscatsDim indicating the subspace used by this model.
 * The default is one dimension, which corresponds to the first dimension of
 * the test space (preference to continuous dimensions, then binary,
 * then natural-valued).
 */
  espec = g_param_spec_uint("dim", "Dimension",
                            "A dimension of the model subspace",
                            0, G_MAXUINT, 0,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  pspec = g_param_spec_value_array("dims", "Dimensions", 
                            "Subspace for the model",
                            espec,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_DIMS, pspec);

/**
 * OscatsModel:dimType:
 *
 * The dimension type of the model's subspace.  If the model is
 * multidimensional with mixed dimension types, #OscatsModel:dimType is 0.
 * Determined from #OscatsModel:dims.
 */
  pspec = g_param_spec_uint("dimType", "Subspace Type", 
                            "Dimension type for the model's subspace",
                            0, G_MAXUINT, 0,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_DIM_TYPE, pspec);

/**
 * OscatsModel:Np:
 *
 * The number of parameters of the measurement model.
 */
  pspec = g_param_spec_uint("Np", "Num Params", 
                            "Number of parameters of the measurement model",
                            0, G_MAXUINT, 1,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_PARAMS, pspec);

/**
 * OscatsModel:paramNames:
 *
 * An array of names for parameters.  The array must have #OscatsModel:Np
 * elements.  The order of the parameters and default names are defined
 * by the particular model implementation.
 */
  espec = g_param_spec_string("paramName", "parameter name",
                            "the name of a model parameter",
                            NULL,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  pspec = g_param_spec_value_array("paramNames", "parameter names", 
                            "the names of the model parameters",
                            espec,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_PARAM_NAMES, pspec);

/**
 * OscatsModel:Ncov:
 *
 * The number of covariates in the measurement model.  Default is 0.
 */
  pspec = g_param_spec_uint("Ncov", "Number of Covariates", 
                            "Number of covariates in measurement model",
                            0, G_MAXUINT, 0,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_COV, pspec);

/**
 * OscatsModel:covariates:
 *
 * An #OscatsCovariates object defining the covariates used in this model. 
 * Note: Models are not required to handle covariates.  Refer to the
 * particular model implementation.
 *
 * Note: The #OscatsCovariates object is not actually stored in the
 * #OscatsModel.  On property read, a new #OscatsCovariates object is
 * created.
 */
  pspec = g_param_spec_object("covariates", "Covariates", 
                            "Covariates used by this model",
                            OSCATS_TYPE_COVARIATES,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_COVARIATES, pspec);

}

static void oscats_model_init (OscatsModel *self)
{
}

static void oscats_model_constructed(GObject *object)
{
  OscatsModel *model = OSCATS_MODEL(object);
//  G_OBJECT_CLASS(oscats_model_parent_class)->constructed(object);

  if (model->space == NULL)
  {
    g_critical("Model must have a latent space.");
    // Provide *something* to prevent memory access errors....
    model->Ndims = 1;
    model->dims = g_new(OscatsDim, 1);
    model->dims[0] = model->dimType = 0;
    return;
  }

  if (model->dims == NULL)
  {
    // No dims specified; default to first dim available.
    model->Ndims = 1;
    model->dims = g_new(OscatsDim, 1);
    if (model->space->num_cont > 0)
      model->dims[0] = model->dimType = OSCATS_DIM_CONT;
    else if (model->space->num_bin > 0)
      model->dims[0] = model->dimType = OSCATS_DIM_BIN;
    else if (model->space->num_nat > 0)
      model->dims[0] = model->dimType = OSCATS_DIM_NAT;
    else
    {
      model->dims[0] = 0;
      g_critical("Latent space has no dimensions!");
    }
  } else {
    // Check that all dims are valid
    OscatsDim type = (model->dims[0] & OSCATS_DIM_TYPE_MASK);
    guint i;
    for (i=0; i < model->Ndims; i++)
      if (!oscats_space_validate(model->space, model->dims[i], 0))
      {
        guint j;
        g_critical("Dimension %x is not valid for space %s.",
                   model->dims[i], model->space->id);
        // Delete the offending dimension
        for (j=i+1; j < model->Ndims; j++)
          model->dims[j-1] = model->dims[j];
        model->Ndims--;
        i--;
      }
      else if (type && type != (model->dims[i] & OSCATS_DIM_TYPE_MASK))
        type = 0;
    model->dimType = type;
    if (model->Ndims == 0)
    {
      g_critical("Model has no valid dimensions!");
      model->Ndims = 1;
      model->dims[0] = model->dimType = 0;
    }
  }

  // If model has single dimension type, set up dimension shortcuts
  if (model->dimType != 0 && model->Ndims > 0)
  {
    guint i;
    model->shortDims = g_new(guint, model->Ndims);
    for (i=0; i < model->Ndims; i++)
      model->shortDims[i] = (model->dims[i] & OSCATS_DIM_MASK);
  }

  // Note: Np, params, and names should be set in the overloaded constructed().
  // Chain up first to check dims and assign dimType.

}

static void oscats_model_dispose (GObject *object)
{
  OscatsModel *self = OSCATS_MODEL(object);
  G_OBJECT_CLASS(oscats_model_parent_class)->dispose(object);
  if (self->space) g_object_unref(self->space);
  self->space = NULL;
}

static void oscats_model_finalize (GObject *object)
{
  OscatsModel *self = OSCATS_MODEL(object);
  g_free(self->dims);
  g_free(self->params);
  g_free(self->names);
  g_free(self->covariates);
  g_free(self->shortDims);
  G_OBJECT_CLASS(oscats_model_parent_class)->finalize(object);
}

static void oscats_model_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec)
{
  OscatsModel *self = OSCATS_MODEL(object);
  OscatsCovariates *covariates;
  GValueArray *array;
  guint i, num;
  switch (prop_id)
  {
    case PROP_SPACE:			// construction only
      self->space = g_value_dup_object(value);
      break;
    
    case PROP_DIMS:			// construction only
      array = g_value_get_boxed(value);
      if (array == NULL) break;		// will default to first dimension
      num = self->Ndims = array->n_values;
      if (num > 0) self->dims = g_new(OscatsDim, num);
      for (i=0; i < num; i++)
        self->dims[i] = g_value_get_uint(array->values+i);
      break;
    
    case PROP_COVARIATES:		// construction only
      covariates = g_value_get_object(value);
      if (covariates == NULL) return;
      self->Ncov = oscats_covariates_num(covariates);
      self->covariates = oscats_covariates_list(covariates);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_model_get_property(GObject *object, guint prop_id,
                                          GValue *value, GParamSpec *pspec)
{
  OscatsModel *self = OSCATS_MODEL(object);
  OscatsCovariates *covariates;
  GValueArray *array;
  GValue v = { 0, };
  guint i;
  
  switch (prop_id)
  {
    case PROP_SPACE:
      g_value_set_object(value, self->space);
      break;
      
    case PROP_N_DIMS:
      g_value_set_uint(value, self->Ndims);
      break;
    
    case PROP_DIMS:
      array = g_value_array_new(self->Ndims);
      g_value_init(&v, G_TYPE_UINT);
      for (i=0; i < self->Ndims; i++)
      {
        g_value_set_uint(&v, self->dims[i]);
        g_value_array_append(array, &v);
      }
      g_value_take_boxed(value, array);
      break;
    
    case PROP_DIM_TYPE:
      g_value_set_uint(value, self->dimType);
      break;
    
    case PROP_N_PARAMS:
      g_value_set_uint(value, self->Np);
      break;
    
    case PROP_PARAM_NAMES:
      // self->names should be set by implementing constructed() method
      g_return_if_fail(self->names != NULL);
      array = g_value_array_new(self->Np);
      g_value_init(&v, G_TYPE_STRING);
      for (i=0; i < self->Np; i++)
      {
        g_value_set_static_string(&v, g_quark_to_string(self->names[i]));
        g_value_array_append(array, &v);
      }
      g_value_take_boxed(value, array);
      break;
    
    case PROP_N_COV:
      g_value_set_uint(value, self->Ncov);
      break;
    
    case PROP_COVARIATES:
      covariates = g_object_newv(OSCATS_TYPE_COVARIATES, 0, NULL);
      for (i=0; i < self->Ncov; i++)
        oscats_covariates_set(covariates, self->covariates[i], 0);
       g_value_take_object(value, covariates);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/**
 * oscats_model_new:
 * @type: the implemented sub-class of #OscatsModel to create
 * @space: the #OscatsSpace in which the model resides
 * @dims: (in) (array length=ndims) (transfer none): the subspace of @space used by this model
 * @ndims: the size of the sub-space used by the model
 * @covariates: (transfer none) (allow-none): optional covariates definition
 *
 * Creates a new model of type @type in the sub-space of @space defined by
 * the @ndims dimensions @dims.  If @covariates is supplied, all of the 
 * covariates present in it will be incorporated into the model, for
 * model implementations that support covariates.  (Refer to specific model
 * implementations for details.)  Note: @type must be a subclass of
 * #OscatsModel.
 *
 * This is a convenience wrapper for g_object_new().
 *
 * Returns: (transfer full): the newly-constructed model
 */
OscatsModel *oscats_model_new(GType type, OscatsSpace *space,
                              const OscatsDim *dims, OscatsDim ndims,
                              OscatsCovariates *covariates)
{
  OscatsModel *model;
  GParameter params[3] = { {"space", { 0, }},
                           {"dims", { 0, }},
                           {"covariates", { 0, }}, };
  GValueArray *array;
  GValue v = { 0, };
  guint i;

  if (type == OSCATS_TYPE_MODEL)
  {
    g_critical("OscatsModel cannot be instantiated directly. Use a sub-class implementation.");
    return NULL;
  }
  g_return_val_if_fail(g_type_is_a(type, OSCATS_TYPE_MODEL), NULL);
  g_return_val_if_fail(OSCATS_IS_SPACE(space), NULL);
  g_return_val_if_fail(ndims > 0 && dims != NULL, NULL);
  if (covariates)
    g_return_val_if_fail(OSCATS_IS_COVARIATES(covariates), NULL);

  g_value_init(&(params[0].value), OSCATS_TYPE_SPACE);
  g_value_take_object(&(params[0].value), space);
  // Use g_value_take_* so that we don't have to bother unsetting value

  array = g_value_array_new(ndims);
  g_value_init(&v, G_TYPE_UINT);
  for (i=0; i < ndims; i++)
  {
    g_value_set_uint(&v, dims[i]);
    g_value_array_append(array, &v);
  }
  g_value_init(&(params[1].value), G_TYPE_VALUE_ARRAY);
  g_value_take_boxed(&(params[1].value), array);
  
  if (covariates)
  {
    g_value_init(&(params[2].value), OSCATS_TYPE_COVARIATES);
    g_value_take_object(&(params[2].value), covariates);
  }
  
  model = g_object_newv(type, (covariates ? 3 : 2), params);
  g_value_array_free(array);
  return model;
}

/**
 * oscats_model_get_max:
 * @model: an #OscatsModel
 *
 * Returns: the maximum valid response category for this model
 */
OscatsResponse oscats_model_get_max(const OscatsModel *model)
{
  g_return_val_if_fail(OSCATS_IS_MODEL(model), 0);
  return OSCATS_MODEL_GET_CLASS(model)->get_max(model);
}

/**
 * oscats_model_P:
 * @model: an #OscatsModel
 * @resp: the examinee response value
 * @theta: the #GGslVector parameter value
 * @covariates: (allow-none): the values for covariates
 * 
 * Calculates the probability of response @resp, given latent ability @theta.
 *
 * Returns: the computed probability
 */
gdouble oscats_model_P(const OscatsModel *model, OscatsResponse resp,
                       const OscatsPoint *theta, const OscatsCovariates *covariates)
{
  g_return_val_if_fail(OSCATS_IS_MODEL(model), 0);
  g_return_val_if_fail(OSCATS_IS_POINT(theta), 0);
  g_return_val_if_fail(oscats_space_compatible(theta->space, model->space), 0);
  if (covariates) g_return_val_if_fail(OSCATS_IS_COVARIATES(covariates), 0);
  return OSCATS_MODEL_GET_CLASS(model)->P(model, resp, theta, covariates);
}

/**
 * oscats_model_distance:
 * @model: an #OscatsModel
 * @theta: the #GGslVector parameter value
 * @covariates: (allow-none): the values of covariates
 *
 * Calculates a distance metric between the model and the given latent
 * ability.  Metric definition is implementation specific.  Note:
 * model implementations are not required to provide oscats_model_distance().
 * Refer to specific implementation documentation for details.
 *
 * Returns: the distance metric
 */
gdouble oscats_model_distance(const OscatsModel *model,
                              const OscatsPoint *theta,
                              const OscatsCovariates *covariates)
{
  g_return_val_if_fail(OSCATS_IS_MODEL(model), 0);
  g_return_val_if_fail(OSCATS_IS_POINT(theta), 0);
  g_return_val_if_fail(oscats_space_compatible(theta->space, model->space), 0);
  if (covariates) g_return_val_if_fail(OSCATS_IS_COVARIATES(covariates), 0);
  return OSCATS_MODEL_GET_CLASS(model)->distance(model, theta, covariates);
}

/**
 * oscats_model_logLik_dtheta:
 * @model: an #OscatsModel
 * @resp: the examinee response value 
 * @theta: the value of the latent variables
 * @covariates: (allow-none): the value of covariates
 * @grad: (inout) (allow-none): a #GGslVector for returning the gradient
 * @hes: (inout) (allow-none): a #GGslMatrix for returning the Hessian
 * 
 * Calculates the derivative of the log-likelihood with respect to the 
 * latent ability @theta, given the examinee response @resp,
 * evaluated at @theta.  If given, the graident is <emphasis>added</emphasis>
 * to @grad, and the Hessian (second derivatives) to @hes.  The vectors and
 * matrix must have dimension @model->dims->size.
 *
 * Not all model implementations support oscats_model_logLik_dtheta().
 * Generally, only models on continuous latent sub-spaces will provide
 * derivatives.  Refer to model implementation documentation for details.
 */
void oscats_model_logLik_dtheta(const OscatsModel *model, OscatsResponse resp,
                                const OscatsPoint *theta, const OscatsCovariates *covariates,
                                GGslVector *grad, GGslMatrix *hes)
{
  g_return_if_fail(OSCATS_IS_MODEL(model));
  g_return_if_fail(OSCATS_IS_POINT(theta));
  g_return_if_fail(oscats_space_compatible(theta->space, model->space));
  if (covariates) g_return_if_fail(OSCATS_IS_COVARIATES(covariates));
  if (grad) g_return_if_fail(G_GSL_IS_VECTOR(grad) && grad->v &&
                             grad->v->size == theta->space->num_cont);
  if (hes) g_return_if_fail(G_GSL_IS_MATRIX(hes) && hes->v &&
                            hes->v->size1 == theta->space->num_cont &&
                            hes->v->size1 == hes->v->size2);
  OSCATS_MODEL_GET_CLASS(model)->logLik_dtheta(model, resp, theta,
                                               covariates, grad, hes, FALSE);
}

/**
 * oscats_model_logLik_dparam:
 * @model: an #OscatsModel
 * @resp: the examinee response value 
 * @theta: the point in latent space
 * @covariates: (allow-none): the values of covariates
 * @grad: (inout) (allow-none): a #GGslVector for returning the gradient
 * @hes: (inout) (allow-none): a #GGslMatrix for returning the Hessian
 * 
 * Calculates the derivative of the log-likelihood with respect to the
 * parameters, given the examinee response @resp, latent variables @theta,
 * and covariates @covariates.  If given, the graident is
 * <emphasis>added</emphasis> to @grad, and the Hessian (second partial
 * derivatives) to @hes.  The gradient and Hessian must have dimension
 * @model->Np, and the latent ability @theta must be compatible with
 * #OscatsModel:space for @model.
 *
 * Note: Not all model implementations support oscats_model_logLik_dparam().
 * Refer to model implementation documentation for details.
 */
void oscats_model_logLik_dparam(const OscatsModel *model, OscatsResponse resp,
                                const OscatsPoint *theta, const OscatsCovariates *covariates,
                                GGslVector *grad, GGslMatrix *hes)
{
  g_return_if_fail(OSCATS_IS_MODEL(model));
  g_return_if_fail(OSCATS_IS_POINT(theta));
  g_return_if_fail(oscats_space_compatible(theta->space, model->space));
  if (covariates) g_return_if_fail(OSCATS_IS_COVARIATES(covariates));
  if (grad) g_return_if_fail(G_GSL_IS_VECTOR(grad) && grad->v &&
                             grad->v->size == model->Np);
  if (hes) g_return_if_fail(G_GSL_IS_MATRIX(hes) && hes->v &&
                            hes->v->size1 == model->Np &&
                            hes->v->size1 == hes->v->size2);
  OSCATS_MODEL_GET_CLASS(model)->logLik_dparam(model, resp, theta,
                                               covariates, grad, hes);
}

/**
 * oscats_model_fisher_inf:
 * @model: an #OscatsModel
 * @theta: the #OscatsPoint latent point
 * @covariates: (allow-none): the value of covariates
 * @I: a #GGslMatrix for returning the Fisher Information
 * 
 * Calculates the Fisher Information at @theta, given @covariates:
 * I = E_{X|theta}[-d^2/dtheta dtheta' log(P(X))].
 * The information is <emphasis>added</emphasis> to @I.
 * The matrix must have dimension @model->space->num_cont.
 */
void oscats_model_fisher_inf(const OscatsModel *model, const OscatsPoint *theta,
                             const OscatsCovariates *covariates, GGslMatrix *I)
{
  OscatsModelClass *klass;
  guint k, max;
  g_return_if_fail(OSCATS_IS_MODEL(model) && OSCATS_IS_POINT(theta));
  g_return_if_fail(oscats_space_compatible(theta->space, model->space));
  g_return_if_fail(G_GSL_IS_MATRIX(I) && I->v &&
                   I->v->size1 == I->v->size2 && 
                   I->v->size2 == model->space->num_cont);
  klass = OSCATS_MODEL_GET_CLASS(model);
  max = klass->get_max(model);
  for (k=0; k <= max; k++)
    klass->logLik_dtheta(model, k, theta, covariates, NULL, I, TRUE);
}

/**
 * oscats_model_get_param_name:
 * @model: an #OscatsModel
 * @index: the parameter index
 * 
 * Returns: (transfer none): the name of parameter @index < #OscatsModel:Np
 */
const gchar* oscats_model_get_param_name(const OscatsModel *model, guint index)
{
  g_return_val_if_fail(OSCATS_IS_MODEL(model) && index < model->Np, NULL);
  g_return_val_if_fail(model->names != NULL, NULL);
  return g_quark_to_string(model->names[index]);
}

/**
 * oscats_model_has_param_name:
 * @model: an #OscatsModel
 * @name: (transfer none): parameter name to check
 *
 * Returns: %TRUE if @model has a parameter called @name.
 */
gboolean oscats_model_has_param_name(const OscatsModel *model,
                                     const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  if (!q) return FALSE;
  return oscats_model_has_param(model, q);
}

/**
 * oscats_model_has_param:
 * @model: an #OscatsModel
 * @name: parameter name (as a #GQuark) to check
 *
 * Returns: %TRUE if @model has a parameter called @name.
 */
gboolean oscats_model_has_param(const OscatsModel *model, GQuark name)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_MODEL(model), FALSE);
  g_return_val_if_fail(model->names != NULL, FALSE);
  for (i=0; i < model->Np; i++)
    if (name == model->names[i]) return TRUE;
  return FALSE;
}

/**
 * oscats_model_get_param_by_index:
 * @model: an #OscatsModel
 * @index: the parameter index
 * 
 * Returns: the parameter @index < #OscatsModel:Np
 */
gdouble oscats_model_get_param_by_index(const OscatsModel *model, guint index)
{
  g_return_val_if_fail(OSCATS_IS_MODEL(model) && index < model->Np, 0);
  g_return_val_if_fail(model->params != NULL, 0);
  return model->params[index];
}

/**
 * oscats_model_get_param:
 * @model: an #OscatsModel
 * @name: the parameter name (as a #GQuark)
 * 
 * Returns: the parameter @name
 */
gdouble oscats_model_get_param(const OscatsModel *model, GQuark name)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_MODEL(model), 0);
  g_return_val_if_fail(model->names != NULL && model->params != NULL, 0);
  for (i=0; i < model->Np; i++)
    if (name == model->names[i]) return model->params[i];
  g_critical("Unknown parameter %s", g_quark_to_string(name));
  return 0;
}

/**
 * oscats_model_get_param_by_name:
 * @model: an #OscatsModel
 * @name: (transfer none): the parameter name
 * 
 * The #GQuark version oscats_model_get_param() is faster.
 *
 * Returns: the parameter called @name
 */
gdouble oscats_model_get_param_by_name(const OscatsModel *model,
                                       const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  g_return_val_if_fail(q != 0, 0);
  return oscats_model_get_param(model, q);
}

/**
 * oscats_model_set_param_by_index:
 * @model: an #OscatsModel
 * @index: the parameter index < #OscatsModel:Np
 * @value: the #gdouble value to set
 */
void oscats_model_set_param_by_index(OscatsModel *model, guint index,
                                     gdouble value)
{
  g_return_if_fail(OSCATS_IS_MODEL(model) && index < model->Np);
  g_return_if_fail(model->params != NULL);
  model->params[index] = value;
}

/**
 * oscats_model_set_param:
 * @model: an #OscatsModel
 * @name: the parameter name (as a #GQuark)
 * @value: the #gdouble to set
 *
 * Faster to set by index.
 */
void oscats_model_set_param(OscatsModel *model, GQuark name, gdouble value)
{
  guint i;
  g_return_if_fail(OSCATS_IS_MODEL(model));
  g_return_if_fail(model->names != NULL && model->params != NULL);
  for (i=0; i < model->Np; i++)
    if (model->names[i] == name)
    {
      model->params[i] = value;
      return;
    }
  g_critical("Unknown parameter %s", g_quark_to_string(name));
}

/**
 * oscats_model_set_param_by_name:
 * @model: an #OscatsModel
 * @name: (transfer none): the parameter name
 * @value: the #gdouble to set
 *
 * The #GQuark version oscats_model_set_param() is faster.
 */
void oscats_model_set_param_by_name(OscatsModel *model, const gchar *name,
                                    gdouble value)
{
  GQuark q = g_quark_try_string(name);
  if (q == 0)
    g_critical("Unknown parameter %s", name);
  else
    oscats_model_set_param(model, q, value);
}

/**
 * oscats_model_has_covariate:
 * @model: an #OscatsModel
 * @name: covariate name (as a #GQuark)
 *
 * Returns: %TRUE if @model incorporates covariate @name
 */
gboolean oscats_model_has_covariate(const OscatsModel *model, GQuark name)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_MODEL(model), FALSE);
  for (i=0; i < model->Ncov; i++)
    if (model->covariates[i] == name) return TRUE;
  return FALSE;
}

/**
 * oscats_model_has_covariate_name:
 * @model: an #OscatsModel
 * @name: (transfer none): covariate name
 *
 * Returns: %TRUE if @model incorporates covariate @name
 */
gboolean oscats_model_has_covariate_name(const OscatsModel *model,
                                         const gchar *name)
{
  GQuark q = g_quark_try_string(name);
  if (!q) return FALSE;
  return oscats_model_has_covariate(model, q);
}

