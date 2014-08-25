/* OSCATS: Open-Source Computerized Adaptive Testing System
 * 2PL IRT Model Java Wrapper Class
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

package oscats;

import oscats.bindings.BlacklistedMethodError;
import oscats.bindings.FIXME;
import oscats.glib.Object;

public final class ModelL2p extends Model
{
    protected ModelL2p(long pointer) { super(pointer); }

    public ModelL2p(Space space) {
      this(OscatsModelL2p.new_with_params("space", space));
    }

    public ModelL2p(Space space, char[] dims) {
      this(OscatsModelL2p.new_with_params("space", space, "dims", dims));
    }

    public ModelL2p(Space space, char[] dims, Covariates covariates) {
      this(OscatsModelL2p.new_with_params("space", space, "dims", dims, "covariates", covariates));
    }

}
