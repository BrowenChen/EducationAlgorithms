/* OSCATS: Open-Source Computerized Adaptive Testing System
 * GslVector Java Wrapper Class
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

package oscats;

import oscats.bindings.BlacklistedMethodError;
import oscats.bindings.FIXME;
import oscats.glib.Object;

public final class GslVector extends Object
{
    protected GslVector(long pointer) { super(pointer); }

    public static GslVector createGslVector(int n) {
      return new GslVector(GGslVector.createGslVector(n));
    }
    
    public void resize(int n) { GGslVector.resize(this, n); }
    
    public void copy(GslVector rhs) { GGslVector.copy(this, rhs); }
    
    public double get(int i) { return GGslVector.get(this, i); }
    
    public void set(int i, double x) { GGslVector.set(this, i, x); }
    
    public void setAll(double x) { GGslVector.setAll(this, x); }
    
    public int getSize() { return GGslVector.getSize(this); }
}

