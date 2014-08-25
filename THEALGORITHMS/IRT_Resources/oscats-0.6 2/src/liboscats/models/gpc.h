/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Generalized Partial Credit IRT Model
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

#ifndef _LIBOSCATS_MODEL_GPC_H_
#define _LIBOSCATS_MODEL_GPC_H_
#include <glib.h>
#include <model.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_MODEL_GPC		(oscats_model_gpc_get_type())
#define OSCATS_MODEL_GPC(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_MODEL_GPC, OscatsModelGpc))
#define OSCATS_IS_MODEL_GPC(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_MODEL_GPC))
#define OSCATS_MODEL_GPC_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_MODEL_GPC, OscatsModelGpcClass))
#define OSCATS_IS_MODEL_GPC_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_MODEL_GPC))
#define OSCATS_MODEL_GPC_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_MODEL_GPC, OscatsModelGpcClass))

typedef struct _OscatsModelGpc OscatsModelGpc;
typedef struct _OscatsModelGpcClass OscatsModelGpcClass;

/**
 * OscatsModelGpc:
 *
 * The Generalized Partial Credit Model is a special case of Bock's
 * Nominal Response Model (#OscatsModelNominal):
 * P(X=k|theta) = f_k(theta)/[1+sum_j f_j(theta)],
 * where f_j(theta) = sum_i a_i theta_i - b_j + sum_l d_l covariate_l,
 * a_i is the discrimination for dimension i,
 * b_j is the scaled option difficulty,
 * and f_0(theta) = 0.
 * Responses are numbered 0, 1, ..., Ncat-1.
 * Note that all options share the same parameters for the covariates.
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
 *    p <pagenums>209-210</pagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>Eiji</firstname> <surname>Muraki</surname></personname></author>
 *    (<pubdate>1992</pubdate>).
 *    "<title>A Generalized Partial Credit Model: Application of an EM Algorithm</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>16</volumenum>,</biblioset>
 *    <artpagenums>159-176</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>Usama</firstname> Sayed Ahmed <surname>Ali</surname></personname></author>
 *    (<pubdate>2011</pubdate>).
 *    <title>Item Selection Methods in Polytomous Computerized Adaptive Testing</title>.
 *    Dissertation, University of Illinois.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsModelGpc {
  OscatsModel parent_instance;
  guint Ncat;		// number of non-zero categories
};

struct _OscatsModelGpcClass {
  OscatsModelClass parent_class;
};

GType oscats_model_gpc_get_type();

G_END_DECLS
#endif
