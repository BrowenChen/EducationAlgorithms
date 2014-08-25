/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Select Item based on Kullback-Leibler Divergence
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

#ifndef _LIBOSCATS_ALGORITHM_MAX_KL_H_
#define _LIBOSCATS_ALGORITHM_MAX_KL_H_
#include <glib-object.h>
#include <algorithm.h>
#include <algorithms/chooser.h>
#include <integrate.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_MAX_KL	(oscats_alg_max_kl_get_type())
#define OSCATS_ALG_MAX_KL(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_MAX_KL, OscatsAlgMaxKl))
#define OSCATS_IS_ALG_MAX_KL(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_MAX_KL))
#define OSCATS_ALG_MAX_KL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_MAX_KL, OscatsAlgMaxKlClass))
#define OSCATS_IS_ALG_MAX_KL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_MAX_KL))
#define OSCATS_ALG_MAX_KL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_MAX_KL, OscatsAlgMaxKlClass))

typedef struct _OscatsAlgMaxKl OscatsAlgMaxKl;
typedef struct _OscatsAlgMaxKlClass OscatsAlgMaxKlClass;

/**
 * OscatsAlgMaxKl:
 *
 * Item selection algorithm (#OscatsTest::select).
 * Picks the item with greatest Kullback-Leibler index.
 * Note: This algorithm may not work correctly if there are
 * multiple items with exactly the same optimality metric.
 *
 * Kullback-Leibler Divergence is:
 * KL(theta.hat || theta) = E_{X|theta.hat}[log{P(X|theta.hat)/P(X|theta)}]
 *
 * The KL Index is:
 * KLI(theta.hat) = Int KL(theta.hat||theta) dtheta
 * with integration over continuous dimensions and summation over discrete
 * dimensions.
 *
 * Integration is either over the box theta.hat +/- c/sqrt(n) or
 * over the ellipsoid (x-theta.hat)' I_n(theta.hat)^-1 (x-theta.hat) <= c,
 * where n is the number of items already administered and I_n(theta.hat) is
 * the Fisher Information for the previously administered items.
 *
 * Note that for discrete dimensions the sum is over all Prod_i n_i patterns
 * (where n_i is the number of response categories for discrete dimension i;
 * for K binary dimensions, this would be 2^K patterns).  Consequently, this
 * algorithm may be very slow in high dimensional latent spaces.
 *
 * Alternatively, the KL Index may be posteriorly weighted:
 * PWKLI(theta.hat) = Int KL(theta.hat||theta) { prod_i P_i(x_i|theta) } g(theta) dtheta
 * where g(theta) is the prior for theta (multivariate normal for continuous
 * dimensions and an arbitrary discrete distribution for discrete dimensions).
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Hua-Hua</firstname> <surname>Chang</surname></personname></author> and
 *    <author><personname><firstname>Zhiliang</firstname> <surname>Ying</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>1996</pubdate>).
 *    "<title>A Global Information Appraoch to Computerized Adaptive Testing</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>20</volumenum>,</biblioset>
 *    <artpagenums>213-229</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Bernard</firstname> <surname>Veldkamp</surname></personname></author> and
 *    <author><personname><firstname>Wim</firstname> <surname>van der Linden</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>2002</pubdate>).
 *    "<title>Multidimensional Adaptive Testing with Constraints on Test Content</title>."
 *    <biblioset><title>Psychometrika</title>,
 *               <volumenum>67</volumenum>,</biblioset>
 *    <artpagenums>575-588</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>Ying</firstname> <surname>Cheng</surname></personname></author>
 *    (<pubdate>2009</pubdate>).
 *    "<title>When Cognitive Diagnosis Meets Computerized Adaptive Testing: CD-CAT</title>."
 *    <biblioset><title>Psychometrika</title>,
 *               <volumenum>74</volumenum>,</biblioset>
 *    <artpagenums>619-632</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Xueli</firstname> <surname>Xu</surname></personname></author>,
 *    <author><personname><firstname>Hua-Hua</firstname> <surname>Chang</surname></personname></author>, and
 *    <author><personname><firstname>Jeff</firstname> <surname>Douglas</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>2003</pubdate>).
 *    "<title>A Simulation Study to Compare CAT Strategies for Cognitive Diagnosis</title>."
 *    Paper presented at the annual meeting of the National Council on Measurement in Education, Montreal, Canada.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>T.M.</firstname> <surname>Cover</surname></personname></author> and
 *    <author><personname><firstname>J.A.</firstname> <surname>Thomas</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>1991</pubdate>).
 *    <title>Elements of Information Theory</title>.
 *    p <pagenums>18</pagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>T.M.</firstname> <surname>Cover</surname></personname></author> and
 *    <author><personname><firstname>J.A.</firstname> <surname>Thomas</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>1991</pubdate>).
 *    <title>Elements of Information Theory</title>.
 *    p <pagenums>18</pagenums>.
 *  </bibliomixed>
 * </bibliolist>
 */

struct _OscatsAlgMaxKl {
  OscatsAlgorithm parent_instance;
  gdouble c;
  /*< private >*/
  OscatsAlgChooser *chooser;
  OscatsSpace *space;
  guint numPatterns;
  // Properties
  GQuark modelKey, thetaKey;
  gboolean inf_bounds, posterior;
  GGslVector *Dprior;
  gsl_vector *mu;
  gsl_matrix *Sigma_half;
  // Temporary holding slots (held without reference)
  OscatsExaminee *e;
  OscatsPoint *theta_hat;
  OscatsModel *model;
  OscatsResponse max;
  // Integration working space
  OscatsPoint *theta;
  gdouble p_sum, *p;
  guint p_num;
  OscatsIntegrate *integrator;
  gsl_vector *tmp, *tmp2;	// for posterior
  GGslMatrix *Inf, *Inf_inv;	// for ellipse
  guint base_num;
};

struct _OscatsAlgMaxKlClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_max_kl_get_type();

G_END_DECLS
#endif
