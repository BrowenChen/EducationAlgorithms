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

package oscats;

import oscats.Administrand;
import oscats.Model;
import oscats.Space;
import oscats.bindings.BlacklistedMethodError;
import oscats.bindings.FIXME;

final class OscatsAdministrandOverride extends Plumbing
{
    private OscatsAdministrandOverride() {}

    static final void setModel(Administrand self, long name, Model model) {
        if (self == null) {
            throw new IllegalArgumentException("self can't be null");
        }

        if (model == null) {
            throw new IllegalArgumentException("model can't be null");
        }

        {
            oscats_administrand_set_model(pointerOf(self), name, pointerOf(model));
        }
    }

    private static native final void oscats_administrand_set_model(long self, long name, long model);

}
