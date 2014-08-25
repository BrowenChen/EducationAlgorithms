/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Latent Space Class
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

#ifndef _LIBOSCATS_SPACE_H_
#define _LIBOSCATS_SPACE_H_
#include <glib-object.h>
G_BEGIN_DECLS

/**
 * OscatsDimType:
 * @OSCATS_DIM_CONT: a continuous dimension
 * @OSCATS_DIM_BIN: a binary dimension
 * @OSCATS_DIM_NAT: a natural (0, 1, 2, ...) dimension
 * @OSCATS_DIM_TYPE_MASK: mask to retrieve dimension type from #OscatsDim
 * @OSCATS_DIM_MASK: mask to retrieve dimension number within type from #OscatsDim
 * @OSCATS_DIM_MAX: maximum number of dimensions of any given type (16,383)
 *
 * A latent space is composed of three types of dimensions: continuous-valued
 * dimensions, binary-valued (0, 1) dimensions, and natural-valued
 * (0, 1, 2, ...) dimensions.  An #OscatsDim encodes both the type and the
 * dimension number.
 */
typedef enum
{
  OSCATS_DIM_CONT = (1 << 14),
  OSCATS_DIM_BIN = (2 << 14),
  OSCATS_DIM_NAT = (3 << 14),
  OSCATS_DIM_TYPE_MASK = (3 << 14),
  OSCATS_DIM_MASK = 0x3fff,
  OSCATS_DIM_MAX = 0x3fff		// == 16383
} OscatsDimType;

#define OSCATS_TYPE_DIM_TYPE (oscats_dim_type_get_type())
GType oscats_dim_type_get_type (void);

/**
 * OscatsDim:
 *
 * Index for a dimension in a latent space.  Encodes both the type and
 * dimension number.
 */
typedef guint16 OscatsDim;

/**
 * OscatsNatural:
 *
 * A natural value (0, 1, 2, ...).  This is simply an alias for #guint16.
 */
typedef guint16 OscatsNatural;

#define OSCATS_TYPE_SPACE		(oscats_space_get_type())
#define OSCATS_SPACE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_SPACE, OscatsSpace))
#define OSCATS_IS_SPACE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_SPACE))
#define OSCATS_SPACE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_SPACE, OscatsSpaceClass))
#define OSCATS_IS_SPACE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_SPACE))
#define OSCATS_SPACE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_SPACE, OscatsSpaceClass))

typedef struct _OscatsSpace OscatsSpace;
typedef struct _OscatsSpaceClass OscatsSpaceClass;

struct _OscatsSpace {
  GObject parent_instance;
  gchar *id;
  /*< private >*/
  GHashTable *names; // name -> dim
  GQuark *names_cont, *names_bin, *names_nat;
  guint16 *max;
  guint16 num_cont, num_bin, num_nat;
};

struct _OscatsSpaceClass {
  GObjectClass parent_class;
};

GType oscats_space_get_type();

guint16 oscats_space_size(const OscatsSpace *space);
void oscats_space_set_dim_name(OscatsSpace *space, OscatsDim dim, const gchar *name);
gboolean oscats_space_has_dim(const OscatsSpace *space, GQuark name);
gboolean oscats_space_has_dim_name(const OscatsSpace *space, const gchar *name);
OscatsDim oscats_space_get_dim(const OscatsSpace *space, GQuark name);
OscatsDim oscats_space_get_dim_by_name(const OscatsSpace *space, const gchar *name);
const gchar * oscats_space_dim_get_name(const OscatsSpace *space, OscatsDim dim);
OscatsNatural oscats_space_dim_get_max(const OscatsSpace *space, OscatsDim dim);
gboolean oscats_space_validate(const OscatsSpace *space, OscatsDim dim, OscatsNatural x);
gboolean oscats_space_compatible(const OscatsSpace *lhs, const OscatsSpace *rhs);

G_END_DECLS
#endif
