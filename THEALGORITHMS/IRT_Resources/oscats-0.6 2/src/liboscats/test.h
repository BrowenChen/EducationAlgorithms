/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Test Administration Class
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

#ifndef _LIBOSCATS_TEST_H_
#define _LIBOSCATS_TEST_H_
#include <glib.h>
#include "examinee.h"
#include "item.h"
#include "itembank.h"
G_BEGIN_DECLS

#define OSCATS_TYPE_TEST		(oscats_test_get_type())
#define OSCATS_TEST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_TEST, OscatsTest))
#define OSCATS_IS_TEST(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_TEST))
#define OSCATS_TEST_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_TEST, OscatsTestClass))
#define OSCATS_IS_TEST_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_TEST))
#define OSCATS_TEST_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_TEST, OscatsTestClass))

typedef struct _OscatsTest OscatsTest;
typedef struct _OscatsTestClass OscatsTestClass;

struct _OscatsTest {
  GObject parent_instance;
  gchar *id;
  OscatsItemBank *itembank;
  GBitArray *hint;
  guint length_hint;
  guint itermax_select, itermax_items;
};

struct _OscatsTestClass {
  GObjectClass parent_class;
  // Signals
  guint initialize;
  guint filter;
  guint select;
  guint approve;
  guint administer;
  guint administered;
  guint stopcrit;
  guint finalize;
};

/*
typedef void (*OscatsTestInitializeFunc) (OscatsTest*, OscatsExaminee*, gpointer);
typedef void (*OscatsTestFilterFunc) (OscatsTest*, OscatsExaminee*, GBitArray*, gpointer);
typedef gint (*OscatsTestSelectFunc) (OscatsTest*, OscatsExaminee*, GBitArray*, gpointer);
typedef gboolean (*OscatsTestApproveFunc) (OscatsTest*, OscatsExaminee*, OscatsItem*, gpointer);
typedef guint (*OscatsTestAdministerFunc) (OscatsTest*, OscatsExaminee*, OscatsItem*, gpointer);
typedef void (*OscatsTestAdministeredFunc) (OscatsTest*, OscatsExaminee*, OscatsItem*, guint, gpointer);
typedef gboolean (*OscatsTestStopcritFunc) (OscatsTest*, OscatsExaminee*, gpointer);
typedef void (*OscatsTestFinalizeFunc) (OscatsTest*, OscatsExamine*, gpointer);
*/

GType oscats_test_get_type();

void oscats_test_administer(OscatsTest *test, OscatsExaminee *e);
void oscats_test_set_hint(OscatsTest *test, GBitArray *hint);

G_END_DECLS
#endif
