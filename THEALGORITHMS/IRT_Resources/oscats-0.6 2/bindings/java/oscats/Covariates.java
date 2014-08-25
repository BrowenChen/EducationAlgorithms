/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Covariates Java Wrapper Class
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

public final class Covariates extends Object
{
    protected Covariates(long pointer) { super(pointer); }
    
    public Covariates() { this(OscatsCovariates.new_with_params()); }

    static public long fromString(String name) {
      return OscatsCovariates.fromString(name);
    }
    
    static public String asString(long name) {
      return OscatsCovariates.asString(name);
    }
    
    public void set(int name, double value) {
      OscatsCovariates.set(this, name, value);
    }
    
    public void set(String name, double value) {
      OscatsCovariates.setByName(this, name, value);
    }
    
    public double get(int name) {
      return OscatsCovariates.get(this, name);
    }
    
    public double get(String name) {
      return OscatsCovariates.getByName(this, name);
    }
    
}

