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

#ifndef _LIBOSCATS_ALGORITHM_ESTIMATE_H_
#define _LIBOSCATS_ALGORITHM_ESTIMATE_H_
#include <glib-object.h>
#include <algorithm.h>
#include <integrate.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_ESTIMATE	(oscats_alg_estimate_get_type())
#define OSCATS_ALG_ESTIMATE(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_ESTIMATE, OscatsAlgEstimate))
#define OSCATS_IS_ALG_ESTIMATE(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_ESTIMATE))
#define OSCATS_ALG_ESTIMATE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_ESTIMATE, OscatsAlgEstimateClass))
#define OSCATS_IS_ALG_ESTIMATE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_ESTIMATE))
#define OSCATS_ALG_ESTIMATE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_ESTIMATE, OscatsAlgEstimateClass))

typedef struct _OscatsAlgEstimate OscatsAlgEstimate;
typedef struct _OscatsAlgEstimateClass OscatsAlgEstimateClass;

/**
 * OscatsAlgEstimate
 *
 * Statistics algorithm (#OscatsTest::administered).
 * Update the examinee's latent IRT ability estimate.
 */
struct _OscatsAlgEstimate {
  OscatsAlgorithm parent_instance;
  /*< private >*/
  gboolean eap, independent;
  guint Nposterior;
  GQuark modelKey, thetaKey;
  gdouble tol;
  GGslVector *Dprior;
  gsl_vector *mu;		// population parameters for eap
  gsl_matrix *Sigma_half;
  // Temporary integration slots, held without reference
  OscatsExaminee *e;
  // Working space
  OscatsIntegrate *integrator, *normalizer;
  OscatsPoint *x;
  gsl_vector *tmp, *tmp2;
  gint flag;
  guint dim;
};

struct _OscatsAlgEstimateClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_estimate_get_type();

G_END_DECLS
#endif
