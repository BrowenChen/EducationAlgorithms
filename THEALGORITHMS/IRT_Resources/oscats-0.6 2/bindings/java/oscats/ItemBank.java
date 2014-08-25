/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Item Bank Java Wrapper Class
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

public final class ItemBank extends Administrand
{
    protected ItemBank(long pointer) { super(pointer); }

    public ItemBank(String id, int sizeHint) {
      this(OscatsItemBank.new_with_params("id", id, "sizeHint", sizeHint));
    }
    
    public ItemBank(String id) {
      this(OscatsItemBank.new_with_params("id", id));
    }
    
    public ItemBank(int sizeHint) {
      this(OscatsItemBank.new_with_params("sizeHint", sizeHint));
    }
    
    public ItemBank() {
      this(OscatsItemBank.new_with_params());
    }
    
    public void addItem(Administrand item) {
      OscatsItemBank.addItem(this, item);
    }
    
    public Administrand getItem(int i) {
      return OscatsItemBank.getItem(this, i);
    }

    public int numItems() {
      return OscatsItemBank.numItems(this);
    }

}

