/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Partial Credit IRT Model
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

#ifndef _LIBOSCATS_MODEL_PC_H_
#define _LIBOSCATS_MODEL_PC_H_
#include <glib.h>
#include <model.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_MODEL_PC		(oscats_model_pc_get_type())
#define OSCATS_MODEL_PC(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_MODEL_PC, OscatsModelPc))
#define OSCATS_IS_MODEL_PC(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_MODEL_PC))
#define OSCATS_MODEL_PC_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_MODEL_PC, OscatsModelPcClass))
#define OSCATS_IS_MODEL_PC_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_MODEL_PC))
#define OSCATS_MODEL_PC_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_MODEL_PC, OscatsModelPcClass))

typedef struct _OscatsModelPc OscatsModelPc;
typedef struct _OscatsModelPcClass OscatsModelPcClass;

/**
 * OscatsModelPc:
 *
 * The Partial Credit Model is the Rasch case of the Generalized Partial
 * Credit Model (#OscatsModelGpc):
 * P(X=k|theta) = f_k(theta)/[1+sum_j f_j(theta)],
 * where f_j(theta) = sum_i theta_i - b_j + sum_l d_l covariate_l,
 * b_j is the scaled option difficulty,
 * and f_0(theta) = 0.
 * Responses are numbered 0, 1, ..., Ncat-1.
 * Note that all options share the same parameters for the covariates.
 * No distance metric is currently provided, but see Ali (2011) for possibilities.
 *
 * Parameter names: Diff.k
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <author><personname><firstname>R. J.</firstname> <surname>de Ayala</surname></personname></author>
 *    (<pubdate>2009</pubdate>).
 *    <title>The Theory and Practice of Item Response Theory</title>.
 *    p <pagenums>166</pagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>Henk</firstname> <surname>Kelderman</surname></personname></author>
 *    (<pubdate>1996</pubdate>).
 *    "<title>Multidimensional Rasch Models for Partial-Credit Scoring</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>20</volumenum>,</biblioset>
 *    <artpagenums>155-168</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>G. N.</firstname> <surname>Masters</surname></personname></author>
 *    (<pubdate>1982</pubdate>).
 *    "<title>A Rasch model for partial credit scoring</title>."
 *    <biblioset><title>Psychometrika</title>,
 *               <volumenum>47</volumenum>,</biblioset>
 *    <artpagenums>149-174</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>Usama</firstname> Sayed Ahmed <surname>Ali</surname></personname></author>
 *    (<pubdate>2011</pubdate>).
 *    <title>Item Selection Methods in Polytomous Computerized Adaptive Testing</title>.
 *    Dissertation, University of Illinois.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsModelPc {
  OscatsModel parent_instance;
  guint Ncat;		// number of non-zero categories
};

struct _OscatsModelPcClass {
  OscatsModelClass parent_class;
};

GType oscats_model_pc_get_type();

G_END_DECLS
#endif
