/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Point in Latent Space
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

#ifndef _LIBOSCATS_POINT_H_
#define _LIBOSCATS_POINT_H_
#include <glib-object.h>
#include "bitarray.h"
#include "space.h"
#include "gsl.h"
G_BEGIN_DECLS

#define OSCATS_EPS 2.220446e-16

#define OSCATS_TYPE_POINT		(oscats_point_get_type())
#define OSCATS_POINT(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_POINT, OscatsPoint))
#define OSCATS_IS_POINT(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_POINT))
#define OSCATS_POINT_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_POINT, OscatsPointClass))
#define OSCATS_IS_POINT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_POINT))
#define OSCATS_POINT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_POINT, OscatsPointClass))

typedef struct _OscatsPoint OscatsPoint;
typedef struct _OscatsPointClass OscatsPointClass;

struct _OscatsPoint {
  GObject parent_instance;
  OscatsSpace *space;
  gdouble *cont;
  GBitArray *bin;
  OscatsNatural *nat;
  GGslVector *link;
};

struct _OscatsPointClass {
  GObjectClass parent_class;
};

GType oscats_point_get_type();

OscatsPoint *oscats_point_new_from_space(OscatsSpace *space);
gboolean oscats_point_same_space(const OscatsPoint *lhs, const OscatsPoint *rhs);
gboolean oscats_point_space_compatible(const OscatsPoint *lhs, const OscatsPoint *rhs);
gboolean oscats_point_equal(const OscatsPoint *lhs, const OscatsPoint *rhs, gdouble tol);
void oscats_point_copy(OscatsPoint *lhs, const OscatsPoint *rhs);
gdouble oscats_point_get_double(const OscatsPoint *point, OscatsDim dim);
gdouble oscats_point_get_cont(const OscatsPoint *point, OscatsDim dim);
gboolean oscats_point_get_bin(const OscatsPoint *point, OscatsDim dim);
OscatsNatural oscats_point_get_nat(const OscatsPoint *point, OscatsDim dim);
void oscats_point_set_cont(OscatsPoint *point, OscatsDim dim, gdouble value);
void oscats_point_set_bin(OscatsPoint *point, OscatsDim dim, gboolean value);
void oscats_point_set_nat(OscatsPoint *point, OscatsDim dim, OscatsNatural value);
GGslVector * oscats_point_cont_as_vector(OscatsPoint *point);

G_END_DECLS
#endif
