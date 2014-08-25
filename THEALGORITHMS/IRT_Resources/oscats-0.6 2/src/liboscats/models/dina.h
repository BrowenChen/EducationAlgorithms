/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Deterministic Inputs Noisy And Gate (DINA) Classification Model
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

#ifndef _LIBOSCATS_MODEL_DINA_H_
#define _LIBOSCATS_MODEL_DINA_H_
#include <glib.h>
#include <model.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_MODEL_DINA		(oscats_model_dina_get_type())
#define OSCATS_MODEL_DINA(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_MODEL_DINA, OscatsModelDina))
#define OSCATS_IS_MODEL_DINA(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_MODEL_DINA))
#define OSCATS_MODEL_DINA_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_MODEL_DINA, OscatsModelDinaClass))
#define OSCATS_IS_MODEL_DINA_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_MODEL_DINA))
#define OSCATS_MODEL_DINA_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_MODEL_DINA, OscatsModelDinaClass))

typedef struct _OscatsModelDina OscatsModelDina;
typedef struct _OscatsModelDinaClass OscatsModelDinaClass;

/**
 * OscatsModelDina:
 *
 * Deterministic Inputs Noisy And Gate (DINA) Classification model:
 * P(X=1|alpha) = (1-s)^(prod_i alpha_i) g^(1-prod_i alpha_i)
 * where s is the slipping parameter and g is the guessing parameter.
 *
 * Parameter names: Guess, Slip.
 *
 * Note: This model does not support covariates, oscats_model_distance(), or
 * oscats_model_logLik_dtheta().
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <author><personname><firstname>Jimmy</firstname> <surname>de la Torre</surname></personname></author>
 *    (<pubdate>2008</pubdate>).
 *    "<title>DINA Model and Parameter Estimation: A Didactic</title>."
 *    <biblioset><title>Journal of Educational and Behavioral Statistics</title>,
 *               <volumenum>34</volumenum>,</biblioset>
 *    <artpagenums>115-130</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Brian</firstname> <surname>Junker</surname></personname></author> and
 *    <author><personname><firstname>Klaas</firstname> <surname>Sijtsma</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>2001</pubdate>).
 *    "<title>Cognitive Assessment Models with Few Assumptions, and Connections with Nonparametric Item Response Theory</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>25</volumenum>,</biblioset>
 *    <artpagenums>258-272</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>Edward</firstname> <surname>Haertel</surname></personname></author>
 *    (<pubdate>1989</pubdate>).
 *    "<title>Using Restricted Latent Class Models to Map the Skill Structure of Achievement Items</title>."
 *    <biblioset><title>Journal of Educational Measurement</title>,
 *               <volumenum>26</volumenum>,</biblioset>
 *    <artpagenums>301-321</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>George</firstname> <surname>Macready</surname></personname></author> and
 *    <author><personname><firstname>C. Mitchell</firstname> <surname>Dayton</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>1977</pubdate>).
 *    "<title>The Use of Probabilistic Modes in the Assessment of Mastery</title>."
 *    <biblioset><title>Journal of Educational Statistics</title>,
 *               <volumenum>2</volumenum>,</biblioset>
 *    <artpagenums>99-120</artpagenums>.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsModelDina {
  OscatsModel parent_instance;
};

struct _OscatsModelDinaClass {
  OscatsModelClass parent_class;
};

GType oscats_model_dina_get_type();

G_END_DECLS
#endif
