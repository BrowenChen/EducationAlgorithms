/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Track item exposure rate
 * Copyright 2010 Michael Culbertson <culbert1@illinois.edu>
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

#ifndef _LIBOSCATS_ALGORITHM_EXPOSURE_COUNTER_H_
#define _LIBOSCATS_ALGORITHM_EXPOSURE_COUNTER_H_
#include <glib-object.h>
#include <item.h>
#include <algorithm.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_EXPOSURE_COUNTER	(oscats_alg_exposure_counter_get_type())
#define OSCATS_ALG_EXPOSURE_COUNTER(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_EXPOSURE_COUNTER, OscatsAlgExposureCounter))
#define OSCATS_IS_ALG_EXPOSURE_COUNTER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_EXPOSURE_COUNTER))
#define OSCATS_ALG_EXPOSURE_COUNTER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_EXPOSURE_COUNTER, OscatsAlgExposureCounterClass))
#define OSCATS_IS_ALG_EXPOSURE_COUNTER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_EXPOSURE_COUNTER))
#define OSCATS_ALG_EXPOSURE_COUNTER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_EXPOSURE_COUNTER, OscatsAlgExposureCounterClass))

typedef struct _OscatsAlgExposureCounter OscatsAlgExposureCounter;
typedef struct _OscatsAlgExposureCounterClass OscatsAlgExposureCounterClass;

/**
 * OscatsAlgExposureCounter
 *
 * Statistics algorithm (#OscatsTest::administered).
 * Tracks the exposure rate for each item.
 */
struct _OscatsAlgExposureCounter {
  OscatsAlgorithm parent_instance;
  /*< private >*/
  guint num_examinees;
  GHashTable *counts;
};

struct _OscatsAlgExposureCounterClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_exposure_counter_get_type();

guint oscats_alg_exposure_counter_num_examinees(const OscatsAlgExposureCounter *alg_data);
gdouble oscats_alg_exposure_counter_get_rate(const OscatsAlgExposureCounter *alg_data,
                                             const OscatsItem *item);

G_END_DECLS
#endif
