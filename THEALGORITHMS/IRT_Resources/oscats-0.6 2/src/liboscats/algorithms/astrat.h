/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Perform a Stratification (with optional b blocking)
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

#ifndef _LIBOSCATS_ALGORITHM_ASTRAT_H_
#define _LIBOSCATS_ALGORITHM_ASTRAT_H_
#include <glib-object.h>
#include <algorithm.h>
#include <algorithms/stratify.h>
#include <model.h>
#include <examinee.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_ASTRAT	(oscats_alg_astrat_get_type())
#define OSCATS_ALG_ASTRAT(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_ASTRAT, OscatsAlgAstrat))
#define OSCATS_IS_ALG_ASTRAT(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_ASTRAT))
#define OSCATS_ALG_ASTRAT_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_ASTRAT, OscatsAlgAstratClass))
#define OSCATS_IS_ALG_ASTRAT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_ASTRAT))
#define OSCATS_ALG_ASTRAT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_ASTRAT, OscatsAlgAstratClass))

typedef struct _OscatsAlgAstrat OscatsAlgAstrat;
typedef struct _OscatsAlgAstratClass OscatsAlgAstratClass;

/**
 * OscatsAlgAstrat:
 *
 * Item selection support algorithm.  This algorithm implements (part of)
 * the a-Stratified item selection method.  On registration, the algorithm
 * will stratify the item bank into the given number of strata.  Then, the
 * algorithm uses the test hint to indicate the current stratum [see
 * oscats_test_set_hint()].  The test hint is updated on the
 * #OscatsTest::administered signal, after the given number of items for
 * each stratum has been <emphasis>recorded</emphasis> for the examinee.
 *
 * Note: This algorithm only implements item stratification.  It must be
 * used in conjunction with an algorithm to pick an item from within the
 * current stratum (e.g. #OscatsAlgClosestDiff or #OscatsAlgMaxFisher).
 *
 * Note: This algorithm does not terminate the test.  It must be used in
 * conjunction with an algorithm that signals completion of the CAT
 * (e.g. #OscatsAlgFixedLength).  If the CAT proceeds beyond that last
 * stratum, a warning will be issued.
 *
 * #OscatsAlgAstrat needs some way of knowing how to find the a-parameter
 * for each model type.  This is accomplished by registering a callback
 * using oscats_alg_astrat_register_model().  Default callbacks are
 * automatically registered for #OscatsModelL1p, #OscatsModelL2p, and
 * #OscatsModelL3p.  These define discrimination as:
 *   [sum_i a_k^2]^{1/2}
 * for each dimension i.
 *
 * #OscatsAlgAstrat emits the #OscatsAlgAstrat::stratum signal when the
 * test moves into a new stratum (including at the start of the test), for
 * coordination with other algorithms that may need this information.
 *
 * References:
 * <bibliolist>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Hua-Hua</firstname> <surname>Chang</surname></personname></author> and
 *    <author><personname><firstname>Zhiliang</firstname> <surname>Ying</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>1999</pubdate>).
 *    "<title>a-Stratified Multistage Computerized Adaptive Testing</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>23</volumenum>,</biblioset>
 *    <artpagenums>211-222</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Hua-Hua</firstname> <surname>Chang</surname></personname></author>,
 *    <author><personname><firstname>Jiahe</firstname> <surname>Qian</surname></personname></author>, and
 *    <author><personname><firstname>Zhiliang</firstname> <surname>Ying</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>2001</pubdate>).
 *    "<title>a-Stratified Multistage Computerized Adaptive Testing Adaptive Testing With b Blocking</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>25</volumenum>,</biblioset>
 *    <artpagenums>333-341</artpagenums>.
 *  </bibliomixed>
 *  <bibliomixed>
 *    <authorgroup>
 *    <author><personname><firstname>Mark</firstname> D. <surname>Reckase</surname></personname></author> and
 *    <author><personname><firstname>Robert</firstname> L. <surname>McKinley</surname></personname></author>
 *    </authorgroup>
 *    (<pubdate>1991</pubdate>).
 *    "<title>The Discriminating Power of Items That Measure More Than One Dimension</title>."
 *    <biblioset><title>Applied Psychological Measurement</title>,
 *               <volumenum>15</volumenum>,</biblioset>
 *    <artpagenums>361-373</artpagenums>.
 *  </bibliomixed>
 * </bibliolist>
 */

struct _OscatsAlgAstrat {
  OscatsAlgorithm parent_instance;
  OscatsAlgStratify *stratify;
  // Properties
  gboolean equalSize;
  guint n_strata, n_blocks, n_equal, *n_items;
  GQuark modelKey;
  // Working space
  guint cur, rem;
  gboolean flag;		// Reached end of last stratum
};

struct _OscatsAlgAstratClass {
  OscatsAlgorithmClass parent_class;
  // Signals
  guint stratum;
};

typedef gdouble (*OscatsAlgAstratCriterion) (const OscatsModel *model);
typedef void (*OscatsAlgAstratNotify) (OscatsAlgAstrat*, OscatsExaminee*, GBitArray*, gpointer);

GType oscats_alg_astrat_get_type();

void oscats_alg_astrat_register_model(GType model,
                                      OscatsAlgAstratCriterion a,
                                      OscatsAlgAstratCriterion b);
void oscats_alg_astrat_restratify(OscatsAlgAstrat *alg_data);

G_END_DECLS
#endif
