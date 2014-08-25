/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Support Algorithm: Stratify items based on some criterion
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

#ifndef _LIBOSCATS_ALGORITHM_STRATIFY_H_
#define _LIBOSCATS_ALGORITHM_STRATIFY_H_
#include <glib-object.h>
#include <bitarray.h>
#include <itembank.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_STRATIFY	(oscats_alg_stratify_get_type())
#define OSCATS_ALG_STRATIFY(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_STRATIFY, OscatsAlgStratify))
#define OSCATS_IS_ALG_STRATIFY(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_STRATIFY))
#define OSCATS_ALG_STRATIFY_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_STRATIFY, OscatsAlgStratifyClass))
#define OSCATS_IS_ALG_STRATIFY_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_STRATIFY))
#define OSCATS_ALG_STRATIFY_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_STRATIFY, OscatsAlgStratifyClass))

typedef struct _OscatsAlgStratify OscatsAlgStratify;
typedef struct _OscatsAlgStratifyClass OscatsAlgStratifyClass;

typedef gdouble (*OscatsAlgStratifyCriterion) (const OscatsAdministrand *item, gpointer data);

/**
 * OscatsAlgStratify
 *
 * Support algorithm (for item selection):
 * Stratify items based on some criterion.
 */
struct _OscatsAlgStratify {
  GObject parent_instance;
  OscatsItemBank *bank;
  GPtrArray *strata;
  guint next;
};

struct _OscatsAlgStratifyClass {
  GObjectClass parent_class;
};

GType oscats_alg_stratify_get_type();

void oscats_alg_stratify_stratify(OscatsAlgStratify *stratify,
           guint n_strata, OscatsAlgStratifyCriterion f,     gpointer f_data,
           guint n_blocks, OscatsAlgStratifyCriterion block, gpointer b_data);
GBitArray * oscats_alg_stratify_get_stratum(const OscatsAlgStratify *stratify, guint stratum);
void oscats_alg_stratify_reset(OscatsAlgStratify *stratify);
GBitArray * oscats_alg_stratify_next(OscatsAlgStratify *stratify);

G_END_DECLS
#endif
