/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Multivariate Integration
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

#ifndef _LIBOSCATS_INTEGRATE_H_
#define _LIBOSCATS_INTEGRATE_H_
#include <glib.h>
#include <gsl/gsl_integration.h>
#include "gsl.h"
#include <point.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_INTEGRATE		(oscats_integrate_get_type())
#define OSCATS_INTEGRATE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_INTEGRATE, OscatsIntegrate))
#define OSCATS_IS_INTEGRATE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_INTEGRATE))
#define OSCATS_INTEGRATE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_INTEGRATE, OscatsIntegrateClass))
#define OSCATS_IS_INTEGRATE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_INTEGRATE))
#define OSCATS_INTEGRATE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_INTEGRATE, OscatsIntegrateClass))

typedef struct _OscatsIntegrate OscatsIntegrate;
typedef struct _OscatsIntegrateClass OscatsIntegrateClass;

typedef gdouble (*OscatsIntegrateFunction) (const GGslVector *x, gpointer data);

struct _OscatsIntegrate {
  GObject parent_instance;
  OscatsIntegrateFunction f;
  guint dims;
  gdouble tol;
  
  /*< private >*/
  guint level;
  GGslVector *x;
  gdouble *min, *max, rem;
  gsl_vector *z, *mu;
  gsl_matrix *B;
  gsl_integration_workspace **ws;
  gpointer data;
  gsl_function F;
};

struct _OscatsIntegrateClass {
  GObjectClass parent_class;
};

GType oscats_integrate_get_type();

void oscats_integrate_set_tol(OscatsIntegrate *integrator, gdouble tol);
void oscats_integrate_set_c_function(OscatsIntegrate *integrator, guint dims, OscatsIntegrateFunction f);
gdouble oscats_integrate_cube(OscatsIntegrate *integrator, GGslVector *mu, gdouble delta, gpointer data);
gdouble oscats_integrate_box(OscatsIntegrate *integrator, GGslVector *min, GGslVector *max, gpointer data);
gdouble oscats_integrate_ellipse(OscatsIntegrate *integrator, GGslVector *mu, GGslMatrix *Sigma, gdouble c, gpointer data);
gdouble oscats_integrate_space(OscatsIntegrate *integrator, gpointer data);
void oscats_integrate_link_point(OscatsIntegrate *integrator, OscatsPoint *point);

G_END_DECLS
#endif
