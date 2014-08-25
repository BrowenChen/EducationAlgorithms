/* OSCATS: Open-Source Computerized Adaptive Testing System
 * OscatsAdministrand Java Overrides
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

#include <jni.h>
#include <oscats.h>
#include "bindings_java.h"
#include "oscats_OscatsAdministrandOverride.h"

JNIEXPORT void JNICALL
Java_oscats_OscatsAdministrandOverride_oscats_1administrand_1set_1model
(
	JNIEnv* env,
	jclass cls,
	jlong _self,
	jlong _name,
	jlong _model
)
{
	OscatsAdministrand* self;
	GQuark name;
	OscatsModel* model;

	// convert parameter self
	self = (OscatsAdministrand*) _self;

	// convert parameter name
	name = (GQuark) _name;

	// convert parameter model
	model = (OscatsModel*) _model;

	// call function
	g_object_ref(model);
	oscats_administrand_set_model(self, name, model);

	// cleanup parameter self

	// cleanup parameter name

	// cleanup parameter model
}

