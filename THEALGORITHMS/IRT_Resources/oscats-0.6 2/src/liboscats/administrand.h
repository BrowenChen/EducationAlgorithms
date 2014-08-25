/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Abstract Administrand Class
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

#ifndef _LIBOSCATS_ADMINISTRAND_H_
#define _LIBOSCATS_ADMINISTRAND_H_
#include <glib-object.h>
#include <bitarray.h>
#include <model.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ADMINISTRAND		(oscats_administrand_get_type())
#define OSCATS_ADMINISTRAND(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ADMINISTRAND, OscatsAdministrand))
#define OSCATS_IS_ADMINISTRAND(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ADMINISTRAND))
#define OSCATS_ADMINISTRAND_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ADMINISTRAND, OscatsAdministrandClass))
#define OSCATS_IS_ADMINISTRAND_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ADMINISTRAND))
#define OSCATS_ADMINISTRAND_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ADMINISTRAND, OscatsAdministrandClass))

typedef struct _OscatsAdministrand OscatsAdministrand;
typedef struct _OscatsAdministrandClass OscatsAdministrandClass;

struct _OscatsAdministrand {
  GObject parent_instance;
  gchar *id;
  GBitArray *characteristics;
  guint freeze_count;
};

/**
 * OscatsAdministrandClass:
 * @freeze: Mark all sub-administrands as frozen
 * @unfreeze: Unmark all sub-administrands as frozen
 * @check_type: Check that all of the administrand's sub-administrands conform to @type
 * @check_model: Check that administrand has a model named @name of sub-classed from type @model
 * @check_dim_type: Check that administrand's sub-space is of @type
 * @check_space: Check that the space in which administrand's @model is compatible with @space
 * @set_default_model: Set the default model name for administrand
 * @get_default_model: Get the current default model name for administrand
 * @set_model: Set a model for the administrand
 * @get_model: Fetch the named model from administrand
 *
 * Note: Implementations of #OscatsAdministrand may not provide all functions.
 * Refer to implementation documentation for details.  If a function is not
 * provided, the default fails silently (returning %FALSE or %NULL).
 *
 * The accessor functions will check type conformity of arguments, so 
 * implementation functions need not do so.
 */
struct _OscatsAdministrandClass {
  /*< private >*/
  GObjectClass parent_class;
  /*< public >*/
  void (*freeze) (OscatsAdministrand *item);
  void (*unfreeze) (OscatsAdministrand *item);
  gboolean (*check_type) (const OscatsAdministrand *item, GType type);
  gboolean (*check_model) (const OscatsAdministrand *item, GQuark model, GType type);
  gboolean (*check_dim_type) (const OscatsAdministrand *item, GQuark model, OscatsDim type);
  gboolean (*check_space) (const OscatsAdministrand *item, GQuark model, const OscatsSpace *space);
  void (*set_default_model) (OscatsAdministrand *item, GQuark name);
  GQuark (*get_default_model) (const OscatsAdministrand *item);
  void (*set_model) (OscatsAdministrand *item, GQuark name, OscatsModel *model);
  OscatsModel * (*get_model) (const OscatsAdministrand *item, GQuark name);
};

GType oscats_administrand_get_type();

void oscats_administrand_freeze(OscatsAdministrand *item);
void oscats_administrand_unfreeze(OscatsAdministrand *item);

void oscats_administrand_reset_characteristics();
void oscats_administrand_register_characteristic(GQuark characteristic);
GQuark oscats_administrand_characteristic_from_string(const gchar *name);
const gchar * oscats_administrand_characteristic_as_string(GQuark characteristic);

void oscats_administrand_set_characteristic(OscatsAdministrand *administrand, GQuark characteristic);
void oscats_administrand_clear_characteristic(OscatsAdministrand *administrand, GQuark characteristic);
void oscats_administrand_clear_characteristics(OscatsAdministrand *administrand);
gboolean oscats_administrand_has_characteristic(OscatsAdministrand *administrand, GQuark characteristic);

void oscats_administrand_characteristics_iter_reset(OscatsAdministrand *administrand);
GQuark oscats_administrand_characteristics_iter_next(OscatsAdministrand *administrand);

gboolean oscats_administrand_check_type (const OscatsAdministrand *item, GType type);
gboolean oscats_administrand_check_model (const OscatsAdministrand *item, GQuark model, GType type);
gboolean oscats_administrand_check_dim_type (const OscatsAdministrand *item, GQuark model, OscatsDim type);
gboolean oscats_administrand_check_space (const OscatsAdministrand *item, GQuark model, const OscatsSpace *space);
void oscats_administrand_set_default_model (OscatsAdministrand *item, GQuark name);
GQuark oscats_administrand_get_default_model (const OscatsAdministrand *item);
void oscats_administrand_set_model (OscatsAdministrand *item, GQuark name, OscatsModel *model);
void oscats_administrand_set_model_by_name (OscatsAdministrand *item, const gchar *name, OscatsModel *model);
OscatsModel * oscats_administrand_get_model (const OscatsAdministrand *item, GQuark name);
OscatsModel * oscats_administrand_get_model_by_name (const OscatsAdministrand *item, const gchar *name);

G_END_DECLS
#endif
