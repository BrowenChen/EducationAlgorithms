/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Test Java Wrapper Class
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

public final class Test extends Object
{
    protected Test(long pointer) { super(pointer); }

    public Test(ItemBank bank) {
      this(OscatsTest.new_with_params("itembank", bank));
    }
    
    public Test(ItemBank bank, int length_hint) {
      this(OscatsTest.new_with_params("itembank", bank, "length_hint", length_hint));
    }
    
    public Test(String id, ItemBank bank) {
      this(OscatsTest.new_with_params("id", id, "itembank", bank));
    }
    
    public Test(String id, ItemBank bank, int length_hint) {
      this(OscatsTest.new_with_params("id", id, "itembank", bank, "length_hint", length_hint));
    }
    
    public Test(ItemBank bank, int itermax_items, int itermax_select) {
      this(OscatsTest.new_with_params("itembank", bank, "itermax-items", itermax_items, "itermax_select", itermax_select));
    }

    public Test(ItemBank bank, int length_hint, int itermax_items, int itermax_select) {
      this(OscatsTest.new_with_params("itembank", bank, "length_hint", length_hint, "itermax-items", itermax_items, "itermax_select", itermax_select));
    }

    public Test(String id, ItemBank bank, int itermax_items, int itermax_select) {
      this(OscatsTest.new_with_params("id", id, "itembank", bank, "itermax-items", itermax_items, "itermax_select", itermax_select));
    }

    public Test(String id, ItemBank bank, int length_hint, int itermax_items, int itermax_select) {
      this(OscatsTest.new_with_params("id", id, "itembank", bank, "length_hint", length_hint, "itermax-items", itermax_items, "itermax_select", itermax_select));
    }

    public void administer(Examinee e) {
      OscatsTest.administer(this, e);
    }
    
    public void setHint(BitArray hint) {
      OscatsTest.setHint(this, hint);
    }

}

