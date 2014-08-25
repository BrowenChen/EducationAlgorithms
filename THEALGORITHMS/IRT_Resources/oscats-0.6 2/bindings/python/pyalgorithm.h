/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Abstract CAT Algorithm Class (Python wrapper)
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

#ifndef _LIBOSCATS_PYALGORITHM_H_
#define _LIBOSCATS_PYALGORITHM_H_
#include <glib.h>
#include <algorithm.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_PY_ALGORITHM		(oscats_py_algorithm_get_type())
#define OSCATS_PY_ALGORITHM(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_PY_ALGORITHM, OscatsPyAlgorithm))
#define OSCATS_IS_PY_ALGORITHM(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_PY_ALGORITHM))
#define OSCATS_PY_ALGORITHM_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_PY_ALGORITHM, OscatsPyAlgorithmClass))
#define OSCATS_IS_PY_ALGORITHM_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_PY_ALGORITHM))
#define OSCATS_PY_ALGORITHM_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_PY_ALGORITHM, OscatsPyAlgorithmClass))

typedef struct _OscatsPyAlgorithm OscatsPyAlgorithm;
typedef struct _OscatsPyAlgorithmClass OscatsPyAlgorithmClass;

struct _OscatsPyAlgorithm {
  OscatsAlgorithm parent_instance;
};

struct _OscatsPyAlgorithmClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_py_algorithm_get_type();

G_END_DECLS
#endif
