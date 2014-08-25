/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Space Java Wrapper Class
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

public final class Space extends Object
{
    public static final char OSCATS_DIM_CONT = (1 << 14);
    public static final char OSCATS_DIM_BIN = (2 << 14);
    public static final char OSCATS_DIM_NAT = (3 << 14);
    public static final char OSCATS_DIM_TYPE_MASK = (3 << 14);
    public static final char OSCATS_DIM_MASK = 0x3fff;
    public static final char OSCATS_DIM_MAX = 0x3fff;               // == 16383

    protected Space(long pointer) { super(pointer); }

    public Space(String id, int numCont, int numBin, char[] natMax) {
      this(OscatsSpace.new_with_params("id", id, "numCont", numCont, "numBin", numBin, "numNat", natMax.length, "max", natMax));
    }
    
    public Space(String id, int numCont, int numBin) {
      this(OscatsSpace.new_with_params("id", id, "numCont", numCont, "numBin", numBin));
    }
    
    public Space(int numCont, int numBin, char[] natMax) {
      this(OscatsSpace.new_with_params("numCont", numCont, "numBin", numBin, "numNat", natMax.length, "max", natMax));
    }
    
    public Space(int numCont, int numBin) {
      this(OscatsSpace.new_with_params("numCont", numCont, "numBin", numBin));
    }
    
    public int size() {
      return OscatsSpace.size(this);
    }
    
    public void setDimName(char dim, String name) {
      OscatsSpace.setDimName(this, dim, name);
    }
    
    public boolean hasDimName(String name) {
      return OscatsSpace.hasDimName(this, name);
    }
    
    public char getDim(String name) {
      return OscatsSpace.getDimByName(this, name);
    }
    
    public String dimGetName(char dim) {
      return OscatsSpace.dimGetName(this, dim);
    }
    
    public char dimGetMax(char dim) {
      return OscatsSpace.dimGetMax(this, dim);
    }
    
    public boolean validate(char dim, char x) {
      return OscatsSpace.validate(this, dim, x);
    }
    
    public boolean compatible(Space rhs) {
      return OscatsSpace.compatible(this, rhs);
    }
    
}

