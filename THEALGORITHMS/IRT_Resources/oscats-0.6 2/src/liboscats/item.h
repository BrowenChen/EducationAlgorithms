/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Item Class
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

#ifndef _LIBOSCATS_ITEM_H_
#define _LIBOSCATS_ITEM_H_
#include <glib.h>
#include <administrand.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ITEM		(oscats_item_get_type())
#define OSCATS_ITEM(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ITEM, OscatsItem))
#define OSCATS_IS_ITEM(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ITEM))
#define OSCATS_ITEM_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ITEM, OscatsItemClass))
#define OSCATS_IS_ITEM_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ITEM))
#define OSCATS_ITEM_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ITEM, OscatsItemClass))

typedef struct _OscatsItem OscatsItem;
typedef struct _OscatsItemClass OscatsItemClass;

struct _OscatsItem {
  OscatsAdministrand parent_instance;
  GQuark defaultKey;
  OscatsModel *defaultmodel;
  GData *models;
};

struct _OscatsItemClass {
  OscatsAdministrandClass parent_class;
};

GType oscats_item_get_type();

OscatsItem * oscats_item_new(GQuark name, OscatsModel *model);

G_END_DECLS
#endif
