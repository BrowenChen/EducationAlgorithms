/* OSCATS: Open-Source Computerized Adaptive Testing System
 * One-Parameter Logistic IRT Model
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

#ifndef _LIBOSCATS_MODEL_L1P_H_
#define _LIBOSCATS_MODEL_L1P_H_
#include <glib.h>
#include <model.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_MODEL_L1P		(oscats_model_l1p_get_type())
#define OSCATS_MODEL_L1P(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_MODEL_L1P, OscatsModelL1p))
#define OSCATS_IS_MODEL_L1P(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_MODEL_L1P))
#define OSCATS_MODEL_L1P_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_MODEL_L1P, OscatsModelL1pClass))
#define OSCATS_IS_MODEL_L1P_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_MODEL_L1P))
#define OSCATS_MODEL_L1P_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_MODEL_L1P, OscatsModelL1pClass))

typedef struct _OscatsModelL1p OscatsModelL1p;
typedef struct _OscatsModelL1pClass OscatsModelL1pClass;

/**
 * OscatsModelL1p:
 *
 * The One-Paramter Logistic (Rasch) IRT model:
 * P(X=1|theta) = 1/1+exp[-(sum_i theta_i -b + sum_j d_j covariate_j)],
 * where b is the item difficulty.
 * Distance is defined as: abs(sum_i theta_i - b + sum_j d_j covariate_j).
 *
 * Parameter names: Diff.
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <author><personname><firstname>R. J.</firstname> <surname>de Ayala</surname></personname></author>
 *    (<pubdate>2009</pubdate>).
 *    <title>The Theory and Practice of Item Response Theory</title>.
 *    p <pagenums>15</pagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>G.</firstname> <surname>Rasch</surname></personname></author>
 *    (<pubdate>1980</pubdate>).
 *    <title>Probabilistic Models for Some Intelligence and Attainment Tests</title>.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsModelL1p {
  OscatsModel parent_instance;
};

struct _OscatsModelL1pClass {
  OscatsModelClass parent_class;
};

GType oscats_model_l1p_get_type();

G_END_DECLS
#endif
