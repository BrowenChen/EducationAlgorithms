/* OSCATS: Open-Source Computerized Adaptive Testing System
 * BitArray Java Wrapper Class
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

public final class BitArray extends Object
{
    protected BitArray(long pointer) { super(pointer); }

    public static final BitArray createBitArray(int bitLength) {
      return new BitArray(GBitArray.createBitArray(bitLength));
    }

    public void copy(BitArray rhs) { GBitArray.copy(this, rhs); }

    public BitArray resize(int bitLength) {
        return GBitArray.resize(this, bitLength);
    }
    
    public BitArray extend(int num) {
        return GBitArray.extend(this, num);
    }

    public int getLen() { return GBitArray.getLen(this); }
    
    public int getNumSet() { return GBitArray.getNumSet(this); }

    public boolean getBit(int pos) {
        return GBitArray.getBit(this, pos);
    }

    public BitArray setBit(int pos) {
        return GBitArray.setBit(this, pos);
    }

    public BitArray clearBit(int pos) {
        return GBitArray.clearBit(this, pos);
    }

    public boolean flipBit(int pos) {
        return GBitArray.flipBit(this, pos);
    }

    public BitArray setBitVal(int pos, boolean val) {
        return GBitArray.setBitVal(this, pos, val);
    }

    public BitArray setRange(int start, int stop, boolean val) {
        return GBitArray.setRange(this, start, stop, val);
    }

    public BitArray reset(boolean val) {
        return GBitArray.reset(this, val);
    }

    public void iterReset() {
        GBitArray.iterReset(this);
    }

    public int iterNext() {
        return GBitArray.iterNext(this);
    }

}

