/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Nominal Response IRT Model
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

#ifndef _LIBOSCATS_MODEL_NOMINAL_H_
#define _LIBOSCATS_MODEL_NOMINAL_H_
#include <glib.h>
#include <model.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_MODEL_NOMINAL		(oscats_model_nominal_get_type())
#define OSCATS_MODEL_NOMINAL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_MODEL_NOMINAL, OscatsModelNominal))
#define OSCATS_IS_MODEL_NOMINAL(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_MODEL_NOMINAL))
#define OSCATS_MODEL_NOMINAL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_MODEL_NOMINAL, OscatsModelNominalClass))
#define OSCATS_IS_MODEL_NOMINAL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_MODEL_NOMINAL))
#define OSCATS_MODEL_NOMINAL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_MODEL_NOMINAL, OscatsModelNominalClass))

typedef struct _OscatsModelNominal OscatsModelNominal;
typedef struct _OscatsModelNominalClass OscatsModelNominalClass;

/**
 * OscatsModelNominal:
 *
 * The Nominal Response IRT model:
 * P(X=k|theta) = f_k(theta)/[1+sum_j f_j(theta)],
 * where f_j(theta) = sum_i a_ij theta_i - b_j + sum_l d_l covariate_l,
 * a_ij are the option discriminations for each dimension,
 * b_j is the scaled option difficulty,
 * and a_i0 = b_0 = 0.
 * Responses are numbered 0, 1, ..., Ncat-1.
 * Note that all options share the same parameters for the covariates.
 * Distance is defined as: min_j abs(f_j(theta)).
 *
 * Parameter names: Diff.j, Discr.j.DimName
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <author><personname><firstname>R. J.</firstname> <surname>de Ayala</surname></personname></author>
 *    (<pubdate>2009</pubdate>).
 *    <title>The Theory and Practice of Item Response Theory</title>.
 *    p <pagenums>240</pagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>R. J.</firstname> <surname>de Ayala</surname></personname></author>
 *    (<pubdate>1992</pubdate>).
 *    "<title>The Nominal Response Model in Computerized Adaptive Testing</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>16</volumenum>,</biblioset>
 *    <artpagenums>327-343</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>R. D.</firstname> <surname>Bock</surname></personname></author>
 *    (<pubdate>1972</pubdate>).
 *    "<title>Estimating item parameters and latent ability when responses are scored in two or more nominal categories</title>."
 *    <biblioset><title>Psychometrika</title>,
 *               <volumenum>37</volumenum>,</biblioset>
 *    <artpagenums>29-51</artpagenums>.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsModelNominal {
  OscatsModel parent_instance;
  guint Ncat;		// number of non-zero categories
};

struct _OscatsModelNominalClass {
  OscatsModelClass parent_class;
};

GType oscats_model_nominal_get_type();

G_END_DECLS
#endif
