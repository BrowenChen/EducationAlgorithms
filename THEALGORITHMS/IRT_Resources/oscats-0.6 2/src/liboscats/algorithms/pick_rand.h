/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Select Random Item
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

#ifndef _LIBOSCATS_ALGORITHM_PICK_RAND_H_
#define _LIBOSCATS_ALGORITHM_PICK_RAND_H_
#include <glib-object.h>
#include <algorithm.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_PICK_RAND	(oscats_alg_pick_rand_get_type())
#define OSCATS_ALG_PICK_RAND(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_PICK_RAND, OscatsAlgPickRand))
#define OSCATS_IS_ALG_PICK_RAND(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_PICK_RAND))
#define OSCATS_ALG_PICK_RAND_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_PICK_RAND, OscatsAlgPickRandClass))
#define OSCATS_IS_ALG_PICK_RAND_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_PICK_RAND))
#define OSCATS_ALG_PICK_RAND_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_PICK_RAND, OscatsAlgPickRandClass))

typedef struct _OscatsAlgPickRand OscatsAlgPickRand;
typedef struct _OscatsAlgPickRandClass OscatsAlgPickRandClass;

/**
 * OscatsAlgPickRand
 *
 * Item selection algorithm (#OscatsTest::select).
 * Picks a random item from those that are active.
 */
struct _OscatsAlgPickRand {
  OscatsAlgorithm parent_instance;
};

struct _OscatsAlgPickRandClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_pick_rand_get_type();

G_END_DECLS
#endif
