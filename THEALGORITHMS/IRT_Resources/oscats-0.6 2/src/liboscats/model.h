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

#ifndef _LIBOSCATS_MODEL_H_
#define _LIBOSCATS_MODEL_H_
#include <glib.h>
#include <gsl.h>
#include <point.h>
#include <covariates.h>
G_BEGIN_DECLS

#define OSCATS_DEFAULT_KEY 0

/**
 * OscatsResponse:
 *
 * Type representing the response to an item.  Simply an alias for #guint8.
 */
typedef guint8 OscatsResponse;
#define OSCATS_MAX_RESPONSE G_MAXUINT8

#define OSCATS_TYPE_MODEL		(oscats_model_get_type())
#define OSCATS_MODEL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_MODEL, OscatsModel))
#define OSCATS_IS_MODEL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_MODEL))
#define OSCATS_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_MODEL, OscatsModelClass))
#define OSCATS_IS_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_MODEL))
#define OSCATS_MODEL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_MODEL, OscatsModelClass))

typedef struct _OscatsModel OscatsModel;
typedef struct _OscatsModelClass OscatsModelClass;

struct _OscatsModel {
  /*< private >*/
  GObject parent_instance;
  /*< public >*/
  OscatsSpace *space;
  guint Ndims;			// dimensions of test and this model
  guint Np, Ncov;		// number of parameters, covariates
  OscatsDim *dims, dimType;
  gdouble *params;
  GQuark *names, *covariates;	// parameter and covariate names
  guint *shortDims;		// shortcut for model implementations
};

/**
 * OscatsModelClass:
 * @get_max: get maximum response category supported by model
 * @P: get the probability of a response, given a point in latent space
 * @distance: get a distance metric between the item model and a given point
 *            in latent space
 * @logLik_dtheta: get the derivative of the log-likelihood of the given
 *                 response with respect to continuous dimensions of the
 *                 model's latent subspace
 * @logLik_dparam: get the derivative of the log-likelihood of the given
 *                 response with respect to model parameters
 *
 * #OscatsModel implementations <emphasis>must</emphasis> overload @get_max
 * and @P.  They <emphasis>may</emphasis> overload the remaining functions.
 * Implementations should make clear in their documentation which optional
 * functions they provide.
 *
 * Note: The accessors for virtual functions of #OscatsModel check the type
 * validity of arguments, so implementations need not do so.  However,
 * implementations should still check the substantive valididity of
 * arguments (including size of @grad and @Hes), as necessary.
 */
struct _OscatsModelClass {
  /*< private >*/
  GObjectClass parent_class;
  /*< public >*/
  OscatsResponse (*get_max) (const OscatsModel *model);
  gdouble (*P) (const OscatsModel *model, OscatsResponse resp, const OscatsPoint *theta, const OscatsCovariates *covariates);
  gdouble (*distance) (const OscatsModel *model, const OscatsPoint *theta, const OscatsCovariates *covariates);
  void (*logLik_dtheta) (const OscatsModel *model, OscatsResponse resp,
                         const OscatsPoint *theta, const OscatsCovariates *covariates,
                         GGslVector *grad, GGslMatrix *hes, gboolean inf);
  void (*logLik_dparam) (const OscatsModel *model, OscatsResponse resp,
                         const OscatsPoint *theta, const OscatsCovariates *covariates,
                         GGslVector *grad, GGslMatrix *hes);
};

GType oscats_model_get_type();

OscatsModel * oscats_model_new(GType type, OscatsSpace *space,
                               const OscatsDim *dims, OscatsDim ndims,
                               OscatsCovariates *covariates);

OscatsResponse oscats_model_get_max(const OscatsModel *model);
gdouble oscats_model_P(const OscatsModel *model, OscatsResponse resp,
                       const OscatsPoint *theta, const OscatsCovariates *covariates);
gdouble oscats_model_distance(const OscatsModel *model,
                              const OscatsPoint *theta, const OscatsCovariates *covariates);
void oscats_model_logLik_dtheta(const OscatsModel *model, OscatsResponse resp,
                                const OscatsPoint *theta, const OscatsCovariates *covariates,
                                GGslVector *grad, GGslMatrix *hes);
void oscats_model_logLik_dparam(const OscatsModel *model, OscatsResponse resp,
                                const OscatsPoint *theta, const OscatsCovariates *covariates,
                                GGslVector *grad, GGslMatrix *hes);
void oscats_model_fisher_inf(const OscatsModel *model,
                                const OscatsPoint *theta, const OscatsCovariates *covariates,
                                GGslMatrix *I);

const gchar* oscats_model_get_param_name(const OscatsModel *model, guint index);
gboolean oscats_model_has_param_name(const OscatsModel *model, const gchar *name);
gboolean oscats_model_has_param(const OscatsModel *model, GQuark name);
gdouble oscats_model_get_param(const OscatsModel *model, GQuark name);
gdouble oscats_model_get_param_by_index(const OscatsModel *model, guint index);
gdouble oscats_model_get_param_by_name(const OscatsModel *model, const gchar *name);
void oscats_model_set_param(OscatsModel *model, GQuark name, gdouble value);
void oscats_model_set_param_by_index(OscatsModel *model, guint index, gdouble value);
void oscats_model_set_param_by_name(OscatsModel *model, const gchar *name, gdouble value);

gboolean oscats_model_has_covariate(const OscatsModel *model, GQuark name);
gboolean oscats_model_has_covariate_name(const OscatsModel *model, const gchar *name);

G_END_DECLS
#endif
