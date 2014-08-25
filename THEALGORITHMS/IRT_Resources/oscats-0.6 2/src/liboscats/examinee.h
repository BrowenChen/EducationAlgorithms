/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Examinee Class
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

#ifndef _LIBOSCATS_EXAMINEE_H_
#define _LIBOSCATS_EXAMINEE_H_
#include <glib-object.h>
#include <item.h>
#include <covariates.h>
#include <point.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_EXAMINEE		(oscats_examinee_get_type())
#define OSCATS_EXAMINEE(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_EXAMINEE, OscatsExaminee))
#define OSCATS_IS_EXAMINEE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_EXAMINEE))
#define OSCATS_EXAMINEE_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_EXAMINEE, OscatsExamineeClass))
#define OSCATS_IS_EXAMINEE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_EXAMINEE))
#define OSCATS_EXAMINEE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_EXAMINEE, OscatsExamineeClass))

typedef struct _OscatsExaminee OscatsExaminee;
typedef struct _OscatsExamineeClass OscatsExamineeClass;

struct _OscatsExaminee {
  GObject parent_instance;
  gchar *id;
  GQuark simKey, estKey;
  OscatsPoint *simTheta, *estTheta;
  GData *theta;
  OscatsCovariates *covariates;
  GPtrArray *items;
  GArray *resp;
};

struct _OscatsExamineeClass {
  GObjectClass parent_class;
};

GType oscats_examinee_get_type();

void oscats_examinee_set_sim_key(OscatsExaminee *e, GQuark name);
GQuark oscats_examinee_get_sim_key(const OscatsExaminee *e);
void oscats_examinee_set_est_key(OscatsExaminee *e, GQuark name);
GQuark oscats_examinee_get_est_key(const OscatsExaminee *e);
void oscats_examinee_set_sim_theta(OscatsExaminee *e, OscatsPoint *theta);
OscatsPoint * oscats_examinee_get_sim_theta(const OscatsExaminee *e);
void oscats_examinee_set_est_theta(OscatsExaminee *e, OscatsPoint *theta);
OscatsPoint * oscats_examinee_get_est_theta(const OscatsExaminee *e);
void oscats_examinee_set_theta(OscatsExaminee *e, GQuark name, OscatsPoint *theta);
void oscats_examinee_set_theta_by_name(OscatsExaminee *e, const gchar *name, OscatsPoint *theta);
OscatsPoint * oscats_examinee_get_theta(OscatsExaminee *e, GQuark name);
OscatsPoint * oscats_examinee_get_theta_by_name(OscatsExaminee *e, const gchar *name);
OscatsPoint * oscats_examinee_init_sim_theta(OscatsExaminee *e, OscatsSpace *space);
OscatsPoint * oscats_examinee_init_est_theta(OscatsExaminee *e, OscatsSpace *space);
OscatsPoint * oscats_examinee_init_theta(OscatsExaminee *e, GQuark name, OscatsSpace *space);

void oscats_examinee_prep(OscatsExaminee *e, guint length_hint);
void oscats_examinee_add_item(OscatsExaminee *e, OscatsItem *item, OscatsResponse resp);
guint oscats_examinee_num_items(const OscatsExaminee *e);
OscatsItem * oscats_examinee_get_item(OscatsExaminee *e, guint i);
OscatsResponse oscats_examinee_get_resp(OscatsExaminee *e, guint i);
gdouble oscats_examinee_logLik(const OscatsExaminee *e, const OscatsPoint *theta, GQuark modelKey);

G_END_DECLS
#endif
