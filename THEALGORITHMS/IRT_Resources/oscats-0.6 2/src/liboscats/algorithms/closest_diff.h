/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Select Item with Closest Difficulty
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

#ifndef _LIBOSCATS_ALGORITHM_CLOSEST_DIFF_H_
#define _LIBOSCATS_ALGORITHM_CLOSEST_DIFF_H_
#include <glib-object.h>
#include <algorithm.h>
#include <algorithms/chooser.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_CLOSEST_DIFF	(oscats_alg_closest_diff_get_type())
#define OSCATS_ALG_CLOSEST_DIFF(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_CLOSEST_DIFF, OscatsAlgClosestDiff))
#define OSCATS_IS_ALG_CLOSEST_DIFF(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_CLOSEST_DIFF))
#define OSCATS_ALG_CLOSEST_DIFF_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_CLOSEST_DIFF, OscatsAlgClosestDiffClass))
#define OSCATS_IS_ALG_CLOSEST_DIFF_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_CLOSEST_DIFF))
#define OSCATS_ALG_CLOSEST_DIFF_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_CLOSEST_DIFF, OscatsAlgClosestDiffClass))

typedef struct _OscatsAlgClosestDiff OscatsAlgClosestDiff;
typedef struct _OscatsAlgClosestDiffClass OscatsAlgClosestDiffClass;

/**
 * OscatsAlgClosestDiff
 *
 * Item selection algorithm (#OscatsTest::select).
 * Picks the item whose difficulty parameter is closest to the current
 * estimate of theta.  Note: This algorithm won't work correctly if there
 * are multiple items with exactly the same location metric.
 */
struct _OscatsAlgClosestDiff {
  OscatsAlgorithm parent_instance;
  /*< private >*/
  OscatsAlgChooser *chooser;
  GQuark modelKey, thetaKey;
};

struct _OscatsAlgClosestDiffClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_closest_diff_get_type();

G_END_DECLS
#endif
