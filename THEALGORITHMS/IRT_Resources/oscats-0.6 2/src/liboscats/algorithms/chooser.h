/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Support Algorithm: Select Item based on arbitrary Criterion
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

#ifndef _LIBOSCATS_ALGORITHM_CHOOSER_H_
#define _LIBOSCATS_ALGORITHM_CHOOSER_H_
#include <glib-object.h>
#include <itembank.h>
#include <examinee.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_CHOOSER	(oscats_alg_chooser_get_type())
#define OSCATS_ALG_CHOOSER(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_CHOOSER, OscatsAlgChooser))
#define OSCATS_IS_ALG_CHOOSER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_CHOOSER))
#define OSCATS_ALG_CHOOSER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_CHOOSER, OscatsAlgChooserClass))
#define OSCATS_IS_ALG_CHOOSER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_CHOOSER))
#define OSCATS_ALG_CHOOSER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_CHOOSER, OscatsAlgChooserClass))

typedef struct _OscatsAlgChooser OscatsAlgChooser;
typedef struct _OscatsAlgChooserClass OscatsAlgChooserClass;

typedef gdouble (*OscatsAlgChooserCriterion) (const OscatsItem *item,
                                              const OscatsExaminee *e,
                                              gpointer data);

/**
 * OscatsAlgChooser
 *
 * Support algorithm (for item selection):
 * Picks an optimal item based on a supplied criterion function.
 * Note: This algorithm won't work correctly if there are
 * multiple items with exactly the optimality metric.
 */
struct _OscatsAlgChooser {
  GObject parent_instance;
  /*< private >*/
  OscatsItemBank *bank;
  OscatsAlgChooserCriterion criterion;
  guint num;
  GArray *dists, *items;
};

struct _OscatsAlgChooserClass {
  GObjectClass parent_class;
};

GType oscats_alg_chooser_get_type();

void oscats_alg_chooser_set_c_criterion(OscatsAlgChooser *chooser,
                                        OscatsAlgChooserCriterion f);
gint oscats_alg_chooser_choose(OscatsAlgChooser *chooser,
                               const OscatsExaminee *e,
                               GBitArray *eligible,
                               gpointer data);

G_END_DECLS
#endif
