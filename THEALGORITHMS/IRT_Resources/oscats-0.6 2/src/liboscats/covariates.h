/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Container Class for Covariates
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

#ifndef _LIBOSCATS_COVARIATES_H_
#define _LIBOSCATS_COVARIATES_H_
#include <glib.h>
#include <glib-object.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_COVARIATES		(oscats_covariates_get_type())
#define OSCATS_COVARIATES(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_COVARIATES, OscatsCovariates))
#define OSCATS_IS_COVARIATES(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_COVARIATES))
#define OSCATS_COVARIATES_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_COVARIATES, OscatsCovariatesClass))
#define OSCATS_IS_COVARIATES_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_COVARIATES))
#define OSCATS_COVARIATES_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_COVARIATES, OscatsCovariatesClass))

typedef struct _OscatsCovariates OscatsCovariates;
typedef struct _OscatsCovariatesClass OscatsCovariatesClass;

struct _OscatsCovariates {
  GObject parent_instance;
  /*< private >*/
  GTree *names;
  GArray *data;
};

struct _OscatsCovariatesClass {
  GObjectClass parent_class;
};

GType oscats_covariates_get_type();

GQuark oscats_covariates_from_string(const gchar *name);
const gchar * oscats_covariates_as_string(GQuark name);

guint oscats_covariates_num(const OscatsCovariates *covariates);
GQuark *oscats_covariates_list(const OscatsCovariates *covariates);

void oscats_covariates_set(OscatsCovariates *covariates,
                           GQuark name, gdouble value);
void oscats_covariates_set_by_name(OscatsCovariates *covariates,
                                   const gchar *name, gdouble value);
gdouble oscats_covariates_get(const OscatsCovariates *covariates, GQuark name);
gdouble oscats_covariates_get_by_name(const OscatsCovariates *covariates,
                                      const gchar *name);

G_END_DECLS
#endif
