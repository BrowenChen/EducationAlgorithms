/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Fixed Length Stopping Criterion
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

#ifndef _LIBOSCATS_ALGORITHM_FIXED_LENGTH_H_
#define _LIBOSCATS_ALGORITHM_FIXED_LENGTH_H_
#include <glib-object.h>
#include <algorithm.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_FIXED_LENGTH	(oscats_alg_fixed_length_get_type())
#define OSCATS_ALG_FIXED_LENGTH(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_FIXED_LENGTH, OscatsAlgFixedLength))
#define OSCATS_IS_ALG_FIXED_LENGTH(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_FIXED_LENGTH))
#define OSCATS_ALG_FIXED_LENGTH_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_FIXED_LENGTH, OscatsAlgFixedLengthClass))
#define OSCATS_IS_ALG_FIXED_LENGTH_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_FIXED_LENGTH))
#define OSCATS_ALG_FIXED_LENGTH_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_FIXED_LENGTH, OscatsAlgFixedLengthClass))

typedef struct _OscatsAlgFixedLength OscatsAlgFixedLength;
typedef struct _OscatsAlgFixedLengthClass OscatsAlgFixedLengthClass;

/**
 * OscatsAlgFixedLength
 *
 * Statistics algorithm (#OscatsTest::administered).
 * Tracks the exposure rate for each item.
 */
struct _OscatsAlgFixedLength {
  OscatsAlgorithm parent_instance;
  /*< private >*/
  guint len;
};

struct _OscatsAlgFixedLengthClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_fixed_length_get_type();

G_END_DECLS
#endif
