/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Nominal IRT Model Java Wrapper Class
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

public final class ModelNominal extends Model
{
    protected ModelNominal(long pointer) { super(pointer); }

    public ModelNominal(Space space, int Ncat) {
      this(OscatsModelNominal.new_with_params("space", space, "Ncat", Ncat));
    }

    public ModelNominal(Space space, char[] dims, int Ncat) {
      this(OscatsModelNominal.new_with_params("space", space, "dims", dims, "Ncat", Ncat));
    }

    public ModelNominal(Space space, char[] dims, int Ncat, Covariates covariates) {
      this(OscatsModelNominal.new_with_params("space", space, "dims", dims, "Ncat", Ncat, "covariates", covariates));
    }

}
