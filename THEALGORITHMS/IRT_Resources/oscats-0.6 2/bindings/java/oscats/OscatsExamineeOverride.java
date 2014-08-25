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
package oscats;

import oscats.Examinee;
import oscats.Item;
import oscats.Point;
import oscats.Space;

final class OscatsExamineeOverride extends Plumbing
{
    private OscatsExamineeOverride() {}

    static final void setSimTheta(Examinee self, Point theta) {
        if (self == null) {
            throw new IllegalArgumentException("self can't be null");
        }

        if (theta == null) {
            throw new IllegalArgumentException("theta can't be null");
        }

        {
            oscats_examinee_set_sim_theta(pointerOf(self), pointerOf(theta));
        }
    }

    private static native final void oscats_examinee_set_sim_theta(long self, long theta);

    static final void setEstTheta(Examinee self, Point theta) {
        if (self == null) {
            throw new IllegalArgumentException("self can't be null");
        }

        if (theta == null) {
            throw new IllegalArgumentException("theta can't be null");
        }

        {
            oscats_examinee_set_est_theta(pointerOf(self), pointerOf(theta));
        }
    }

    private static native final void oscats_examinee_set_est_theta(long self, long theta);

    static final void setTheta(Examinee self, long name, Point theta) {
        if (self == null) {
            throw new IllegalArgumentException("self can't be null");
        }

        if (theta == null) {
            throw new IllegalArgumentException("theta can't be null");
        }

        {
            oscats_examinee_set_theta(pointerOf(self), name, pointerOf(theta));
        }
    }

    private static native final void oscats_examinee_set_theta(long self, long name, long theta);

}

