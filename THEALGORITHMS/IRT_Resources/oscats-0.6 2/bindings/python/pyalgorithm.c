/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Abstract CAT Algorithm Class (Python Wrapper)
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

#define NO_IMPORT_PYGOBJECT
#include <pygobject.h>
#include "pyalgorithm.h"

G_DEFINE_TYPE(OscatsPyAlgorithm, oscats_py_algorithm, OSCATS_TYPE_ALGORITHM);

static void py_register (OscatsAlgorithm *alg_data, OscatsTest *test)
{
  PyObject *py_alg_data = NULL, *py_test = NULL, *o, *name;
  
  py_alg_data = pygobject_new((GObject*)alg_data);
  if (!py_alg_data)
  {
    PyErr_SetString(PyExc_RuntimeError, "Could not get python wrapper for alg_data");
    goto bail;
  }
  py_test = pygobject_new((GObject*)test);
  if (!py_test)
  {
    PyErr_SetString(PyExc_RuntimeError, "Could not get python wrapper for test");
    goto bail;
  }

  name = PyString_FromString("__reg__");
  if (!name) goto bail;
  o = PyObject_CallMethodObjArgs(py_alg_data, name, py_test, NULL);
  if (o) Py_DECREF(o);
  Py_DECREF(name);

bail:
  if (py_alg_data) Py_DECREF(py_alg_data);
  if (py_test) Py_DECREF(py_test);
}
                   
static void oscats_py_algorithm_class_init (OscatsPyAlgorithmClass *klass)
{
//  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  OSCATS_ALGORITHM_CLASS(klass)->reg = py_register;
}

static void oscats_py_algorithm_init (OscatsPyAlgorithm *self)
{
}

