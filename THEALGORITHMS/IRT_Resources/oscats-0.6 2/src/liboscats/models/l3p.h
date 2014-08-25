/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Three-Parameter Logistic IRT Model
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

#ifndef _LIBOSCATS_MODEL_L3P_H_
#define _LIBOSCATS_MODEL_L3P_H_
#include <glib.h>
#include <model.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_MODEL_L3P		(oscats_model_l3p_get_type())
#define OSCATS_MODEL_L3P(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_MODEL_L3P, OscatsModelL3p))
#define OSCATS_IS_MODEL_L3P(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_MODEL_L3P))
#define OSCATS_MODEL_L3P_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_MODEL_L3P, OscatsModelL3pClass))
#define OSCATS_IS_MODEL_L3P_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_MODEL_L3P))
#define OSCATS_MODEL_L3P_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_MODEL_L3P, OscatsModelL3pClass))

typedef struct _OscatsModelL3p OscatsModelL3p;
typedef struct _OscatsModelL3pClass OscatsModelL3pClass;

/**
 * OscatsModelL3p:
 *
 * The Three-Paramter Logistic IRT model:
 * P(X=1|theta) = c + (1-c)/1+exp[-(sum_i a_i theta_i -b + sum_j d_j covariate_j)],
 * where a_i are the item discrimination for each dimension 
 * and b is the scaled item difficulty.  Note that this differs from the
 * usual IRT parameterization c + (1-c)/1+exp[-a(theta-b)].
 * Distance is defined as: abs(sum_i a_i theta_i - b + sum_j d_j covariate_j).
 *
 * Parameter names: Diff, Guess, Discr.DimName
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <author><personname><firstname>R. J.</firstname> <surname>de Ayala</surname></personname></author>
 *    (<pubdate>2009</pubdate>).
 *    <title>The Theory and Practice of Item Response Theory</title>.
 *    p <pagenums>124, 288</pagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>A.</firstname> <surname>Birnbaum</surname></personname></author>
 *    (<pubdate>1968</pubdate>).
 *    "<title>Some Latent Trait Models and Their Use in Inferring an Examinee's Ability</title>."
 *    In <biblioset><authorgroup><editor><personname><firstname>F. M.</firstname> <surname>Lord</surname></personname></editor> and
 *                               <editor><personname><firstname>M. R.</firstname> <surname>Novick</surname></personname></editor>. </authorgroup>
 *                  <title>Statistical Theories of Mental Test Scores</title></biblioset>
 *    p <pagenums>405</pagenums>.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsModelL3p {
  OscatsModel parent_instance;
};

struct _OscatsModelL3pClass {
  OscatsModelClass parent_class;
};

GType oscats_model_l3p_get_type();

G_END_DECLS
#endif
