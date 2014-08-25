/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Homogenous Logistic Graded Response IRT Model
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

#ifndef _LIBOSCATS_MODEL_GR_H_
#define _LIBOSCATS_MODEL_GR_H_
#include <glib.h>
#include <model.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_MODEL_GR		(oscats_model_gr_get_type())
#define OSCATS_MODEL_GR(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_MODEL_GR, OscatsModelGr))
#define OSCATS_IS_MODEL_GR(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_MODEL_GR))
#define OSCATS_MODEL_GR_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_MODEL_GR, OscatsModelGrClass))
#define OSCATS_IS_MODEL_GR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_MODEL_GR))
#define OSCATS_MODEL_GR_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_MODEL_GR, OscatsModelGrClass))

typedef struct _OscatsModelGr OscatsModelGr;
typedef struct _OscatsModelGrClass OscatsModelGrClass;

/**
 * OscatsModelGr:
 *
 * The Homogenous Logistic Graded Response IRT model (commonly called simply
 * the Graded Response Model):
 * P(X=k|theta) = P*_k(theta) - P*_{k+1}(theta),
 * P*_k(theta) = 1/1+exp[-(sum_i a_i theta_i - b_k + sum_j d_j covariate_j)]
 * where P*_k(theta) = P(X>=k|theta) is the probability of scoring at least k,
 * P*_0(theta) = 1, P*_{Ncat} = 0,
 * a_i is the discrimination on dimension i,
 * and b_k is the scaled response boundary location.
 * Responses are numbered 0, 1, ..., Ncat-1.
 * Note that all respose options share the same parameters for the covariates.
 * No distance metric is currently provided, but see Ali (2011) for possibilities.
 *
 * Parameter names: Diff.k, Discr.DimName
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <author><personname><firstname>R. J.</firstname> <surname>de Ayala</surname></personname></author>
 *    (<pubdate>2009</pubdate>).
 *    <title>The Theory and Practice of Item Response Theory</title>.
 *    p <pagenums>217-222</pagenums>.
 *  </bibliomixed>
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
struct _OscatsModelGr {
  OscatsModel parent_instance;
  guint Ncat;		// number of non-zero categories
};

struct _OscatsModelGrClass {
  OscatsModelClass parent_class;
};

GType oscats_model_gr_get_type();

G_END_DECLS
#endif
