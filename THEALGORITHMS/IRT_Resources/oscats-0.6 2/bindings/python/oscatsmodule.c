/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Python Bindings for liboscats
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
 *
 *  This file is based on atkmodule.c from the PyGTK package:
 *  http://www.pygtk.org/
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* include this first, before NO_IMPORT_PYGOBJECT is defined */
#include <pygobject.h>
#include <oscats.h>

void pyoscats_register_classes (PyObject *d);
void pyoscats_add_constants(PyObject *module, const gchar *strip_prefix);
void _pyoscats_register_boxed_types(void);	

extern PyMethodDef pyoscats_functions[];

DL_EXPORT(void)
initoscats(void)
{
    PyObject *m, *d;
	
    init_pygobject ();

    m = Py_InitModule ("oscats", pyoscats_functions);
    d = PyModule_GetDict (m);
//    _pyoscats_register_boxed_types();	
    pyoscats_register_classes (d);
//    pyoscats_add_constants(m, "OSCATS_");
}

