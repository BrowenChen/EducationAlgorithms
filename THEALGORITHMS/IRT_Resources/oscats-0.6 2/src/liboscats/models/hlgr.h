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

#ifndef _LIBOSCATS_MODEL_HLGR_H_
#define _LIBOSCATS_MODEL_HLGR_H_
#include <glib.h>
#include <model.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_MODEL_HETLGR		(oscats_model_hetlgr_get_type())
#define OSCATS_MODEL_HETLGR(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_MODEL_HETLGR, OscatsModelHetlgr))
#define OSCATS_IS_MODEL_HETLGR(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_MODEL_HETLGR))
#define OSCATS_MODEL_HETLGR_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_MODEL_HETLGR, OscatsModelHetlgrClass))
#define OSCATS_IS_MODEL_HETLGR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_MODEL_HETLGR))
#define OSCATS_MODEL_HETLGR_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_MODEL_HETLGR, OscatsModelHetlgrClass))

typedef struct _OscatsModelHetlgr OscatsModelHetlgr;
typedef struct _OscatsModelHetlgrClass OscatsModelHetlgrClass;

/**
 * OscatsModelHetlgr:
 *
 * The Heterogeneous Logistic Graded Response IRT model is a generalization
 * of the Homogeneous Logistic Graded Response model (#OscatsModelGr) that
 * allows the discrimination of each response category to be different:
 * P(X=k|theta) = P*_k(theta) - P*_{k+1}(theta),
 * P*_k(theta) = 1/1+exp[-(sum_i a_ki theta_i - b_k + sum_j d_j covariate_j)]
 * where P*_k(theta) = P(X>=k|theta) is the probability of scoring at least k,
 * P*_0(theta) = 1, P*_{Ncat} = 0,
 * a_ki is the discrimination on dimension i for response k,
 * and b_k is the scaled response boundary location.
 * Responses are numbered 0, 1, ..., Ncat-1.
 * Note that all respose options share the same parameters for the covariates.
 * No distance metric is currently provided, but see Ali (2011) for possibilities.
 *
 * Parameter names: Diff.k, Discr.k.DimName
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <author><personname><firstname>F.</firstname> <surname>Samejima</surname></personname></author>
 *    (<pubdate>1976</pubdate>).
 *    "<title>The graded response model of latent trait theory and tailored testing</title>."
 *    <biblioset><title>Proceedings of the First Conference on Computerized Adaptive Testing</title></biblioset>,
 *    <artpagenums>5-15</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>F.</firstname> <surname>Samejima</surname></personname></author>
 *    (<pubdate>1969</pubdate>).
 *    "<title>Estimation of latent ability using a response pattern of graded scores</title>."
 *    <biblioset><title>Psychometrika Monograph Supplement</title>,
 *               <volumenum>17</volumenum></biblioset>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>Usama</firstname> Sayed Ahmed <surname>Ali</surname></personname></author>
 *    (<pubdate>2011</pubdate>).
 *    <title>Item Selection Methods in Polytomous Computerized Adaptive Testing</title>.
 *    Dissertation, University of Illinois.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsModelHetlgr {
  OscatsModel parent_instance;
  guint Ncat;		// number of non-zero categories
};

struct _OscatsModelHetlgrClass {
  OscatsModelClass parent_class;
};

GType oscats_model_hetlgr_get_type();

G_END_DECLS
#endif
