/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Examinee Java Overrides
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
#include "oscats_OscatsExamineeOverride.h"

JNIEXPORT void JNICALL
Java_oscats_OscatsExamineeOverride_oscats_1examinee_1set_1sim_1theta
(
	JNIEnv* env,
	jclass cls,
	jlong _self,
	jlong _theta
)
{
	OscatsExaminee* self;
	OscatsPoint* theta;

	// convert parameter self
	self = (OscatsExaminee*) _self;

	// convert parameter theta
	theta = (OscatsPoint*) _theta;

	// call function
	g_object_ref(theta);
	oscats_examinee_set_sim_theta(self, theta);

	// cleanup parameter self

	// cleanup parameter theta
}

JNIEXPORT void JNICALL
Java_oscats_OscatsExamineeOverride_oscats_1examinee_1set_1est_1theta
(
	JNIEnv* env,
	jclass cls,
	jlong _self,
	jlong _theta
)
{
	OscatsExaminee* self;
	OscatsPoint* theta;

	// convert parameter self
	self = (OscatsExaminee*) _self;

	// convert parameter theta
	theta = (OscatsPoint*) _theta;

	// call function
	g_object_ref(theta);
	oscats_examinee_set_est_theta(self, theta);

	// cleanup parameter self

	// cleanup parameter theta
}

JNIEXPORT void JNICALL
Java_oscats_OscatsExamineeOverride_oscats_1examinee_1set_1theta
(
	JNIEnv* env,
	jclass cls,
	jlong _self,
	jlong _name,
	jlong _theta
)
{
	OscatsExaminee* self;
	GQuark name;
	OscatsPoint* theta;

	// convert parameter self
	self = (OscatsExaminee*) _self;

	// convert parameter name
	name = (GQuark) _name;

	// convert parameter theta
	theta = (OscatsPoint*) _theta;

	// call function
	g_object_ref(theta);
	oscats_examinee_set_theta(self, name, theta);

	// cleanup parameter self

	// cleanup parameter name

	// cleanup parameter theta
}

