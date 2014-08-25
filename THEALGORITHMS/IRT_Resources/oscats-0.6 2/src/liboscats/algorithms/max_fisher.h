/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Select Item based on Fisher Information
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

#ifndef _LIBOSCATS_ALGORITHM_MAX_FISHER_H_
#define _LIBOSCATS_ALGORITHM_MAX_FISHER_H_
#include <glib-object.h>
#include <algorithm.h>
#include <algorithms/chooser.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_MAX_FISHER	(oscats_alg_max_fisher_get_type())
#define OSCATS_ALG_MAX_FISHER(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_MAX_FISHER, OscatsAlgMaxFisher))
#define OSCATS_IS_ALG_MAX_FISHER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_MAX_FISHER))
#define OSCATS_ALG_MAX_FISHER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_MAX_FISHER, OscatsAlgMaxFisherClass))
#define OSCATS_IS_ALG_MAX_FISHER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_MAX_FISHER))
#define OSCATS_ALG_MAX_FISHER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_MAX_FISHER, OscatsAlgMaxFisherClass))

typedef struct _OscatsAlgMaxFisher OscatsAlgMaxFisher;
typedef struct _OscatsAlgMaxFisherClass OscatsAlgMaxFisherClass;

/**
 * OscatsAlgMaxFisher:
 *
 * Item selection algorithm (#OscatsTest::select).
 * Picks the item with optimal Fisher information.
 * Note: This algorithm may not work correctly if there are
 * multiple items with exactly the same optimality metric.
 *
 * Fisher Information is:
 * I_j(theta) = -E_{X|theta}[ d^2/dtheta dtheta' log P_j(x|theta) ]
 *
 * D-optimal Fisher Information minimizes the confidence ellipsoid around
 * theta.hat by maximizing the determinant of the Fisher Information:
 * arg max_j det[ I_j(theta.hat) + sum_k I_k(theta.hat) ],
 * where I_k are the previously administered items.
 * This is equivalent to the traditional Maximum Information Criterion
 * in the unidimensional case.
 *
 * A-optimal Fisher Information minimizes the trace of the covariance
 * matrix for theta:
 * arg min_j tr{ [ I_j(theta.hat) + sum_k I_k(theta.hat) ]^-1 }
 * This is equivalent to the D-optimal condition in the unidimensional
 * case, but the D-optimal calculation will be slightly faster.
 *
 * Note for multidimensional tests: These algorithms will not work correctly
 * unless the test information matrix for the items already administered is
 * full rank.  If not all of the items in the item bank load onto every
 * dimension, a different item selection mechanism may be needed until the
 * test information achieves full rank.
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Joris</firstname> <surname>Mulder</surname></personname></author> and
 *    <author><personname><firstname>Wim</firstname> <surname>van der Linden</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>2009</pubdate>).
 *    "<title>Multidimensional Adaptive Testing with Optimal Design Criteria for Item Selection</title>."
 *    <biblioset><title>Psychometrika</title>,
 *               <volumenum>74</volumenum>,</biblioset>
 *    <artpagenums>273-296</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>D.O.</firstname> <surname>Segall</surname></personname></author>
 *    (<pubdate>1996</pubdate>).
 *    "<title>Multidimensional Adaptive Testing</title>."
 *    <biblioset><title>Psychometrika</title>,
 *               <volumenum>61</volumenum>,</biblioset>
 *    <artpagenums>331-354</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <author><personname><firstname>Frederick</firstname> <surname>Lord</surname></personname></author>
 *    (<pubdate>1980</pubdate>).
 *    <title>Applications of Item Response Theory to Practical Testing Problems</title>.
 *    p <pagenums>151</pagenums>.
 *  </bibliomixed>
 * </bibliolist>
 */
struct _OscatsAlgMaxFisher {
  OscatsAlgorithm parent_instance;
  gboolean A_opt;
  /*< private >*/
  OscatsAlgChooser *chooser;
  guint base_num, dim;
  GQuark modelKey, thetaKey;
  OscatsPoint *theta;			// temporary value for criterion
  GGslMatrix *base, *work, *inv;
  GGslPermutation *perm;
};

struct _OscatsAlgMaxFisherClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_max_fisher_get_type();
void oscats_alg_max_fisher_resize(OscatsAlgMaxFisher *alg_data, guint num);

G_END_DECLS
#endif
