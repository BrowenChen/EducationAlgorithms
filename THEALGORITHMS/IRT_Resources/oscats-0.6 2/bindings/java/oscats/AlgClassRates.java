/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Classification Rates Tabulation Algorithm Java Wrapper Class
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

public final class AlgClassRates extends Algorithm
{
    protected AlgClassRates(long pointer) { super(pointer); }

    public AlgClassRates() { this(OscatsAlgClassRates.new_with_params()); }

    public int numExaminees() {
      return OscatsAlgClassRates.numExaminees(this);
    }
    
    public double getPatternRate() {
      return OscatsAlgClassRates.getPatternRate(this);
    }

    public double getAttributeRate(int index) {
      return OscatsAlgClassRates.getAttributeRate(this, index);
    }

    public double getMisclassifyFreq(int num) {
      return OscatsAlgClassRates.getMisclassifyFreq(this, num);
    }

    public int numExamineesByPattern(Point attr) {
      return OscatsAlgClassRates.numExamineesByPattern(this, attr);
    }

    public double getRateByPattern(Point attr) {
      return OscatsAlgClassRates.getRateByPattern(this, attr);
    }

}

