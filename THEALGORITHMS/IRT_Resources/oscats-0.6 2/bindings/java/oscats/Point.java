/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Point Java Wrapper Class
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

public final class Point extends Object
{
    protected Point(long pointer) { super(pointer); }

    public Point(Space space) {
      this(OscatsPoint.new_with_params("space", space));
    }
    
    public boolean sameSpace(Point rhs) {
      return OscatsPoint.sameSpace(this, rhs);
    }
    
    public boolean spaceCompatible(Point rhs) {
      return OscatsPoint.spaceCompatible(this, rhs);
    }
    
    public void copy(Point rhs) {
      OscatsPoint.copy(this, rhs);
    }
    
    public double getDouble(char dim) {
      return OscatsPoint.getDouble(this, dim);
    }
    
    public double getCont(char dim) {
      return OscatsPoint.getCont(this, dim);
    }
    
    public boolean getBin(char dim) {
      return OscatsPoint.getBin(this, dim);
    }
    
    public char getNat(char dim) {
      return OscatsPoint.getNat(this, dim);
    }
    
    public void setCont(char dim, double x) {
      OscatsPoint.setCont(this, dim, x);
    }
    
    public void setBin(char dim, boolean x) {
      OscatsPoint.setBin(this, dim, x);
    }
    
    public void setNat(char dim, char x) {
      OscatsPoint.setNat(this, dim, x);
    }
    
/*
    public GslVector asVector() {
      return OscatsPoint.asVector(this);
    }
*/

}
