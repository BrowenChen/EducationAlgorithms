/* OSCATS: Open-Source Computerized Adaptive Testing System
 * a Stratification (with optional b blocking) Algorithm Java Wrapper Class
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

import oscats.bindings.BlacklistedMethodError;
import oscats.bindings.FIXME;
import oscats.glib.Object;

public final class AlgAstrat extends Algorithm
{
    protected AlgAstrat(long pointer) { super(pointer); }

    public AlgAstrat() { this(OscatsAlgAstrat.new_with_params()); }

    // Equal sized strata
    public static AlgAstrat createAlgAstrat(int Nstrata, int NperStratum, int num)
    {
      return new AlgAstrat(OscatsAlgAstrat.new_with_params("Nstrata", Nstrata, "Nequal", NperStratum, "num", num));
    }

    public static AlgAstrat createAlgAstrat(int Nstrata, int NperStratum)
    {
      return new AlgAstrat(OscatsAlgAstrat.new_with_params("Nstrata", Nstrata, "Nequal", NperStratum));
    }

    public static AlgAstrat createAlgAstratWithBlocking(int Nstrata, int Nblocks, int NperStratum, int num)
    {
      return new AlgAstrat(OscatsAlgAstrat.new_with_params("Nstrata", Nstrata, "Nblocks", Nblocks, "Nequal", NperStratum, "num", num));
    }

    public static AlgAstrat createAlgAstratWithBlocking(int Nstrata, int Nblocks, int NperStratum)
    {
      return new AlgAstrat(OscatsAlgAstrat.new_with_params("Nstrata", Nstrata, "Nblocks", Nblocks, "Nequal", NperStratum));
    }

    // Inequally sized strata
    public static AlgAstrat createAlgAstrat(int[] NperStratum, int num)
    {
      return new AlgAstrat(OscatsAlgAstrat.new_with_params("equal", false, "Nitems", NperStratum, "num", num));
    }

    public static AlgAstrat createAlgAstrat(int[] NperStratum)
    {
      return new AlgAstrat(OscatsAlgAstrat.new_with_params("equal", false, "Nitems", NperStratum));
    }

    public static AlgAstrat createAlgAstratWithBlocking(int[] NperStratum, int Nblocks, int num)
    {
      return new AlgAstrat(OscatsAlgAstrat.new_with_params("equal", false, "Nitems", NperStratum, "Nblocks", Nblocks, "num", num));
    }

    public static AlgAstrat createAlgAstratWithBlocking(int[] NperStratum, int Nblocks)
    {
      return new AlgAstrat(OscatsAlgAstrat.new_with_params("equal", false, "Nitems", NperStratum, "Nblocks", Nblocks));
    }

    public void restratify() { OscatsAlgAstrat.restratify(this); }
}

