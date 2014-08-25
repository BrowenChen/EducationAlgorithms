/* OSCATS: Open-Source Computerized Adaptive Testing System
 * GslMatrix Java Wrapper Class
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

public final class GslMatrix extends Object
{
    protected GslMatrix(long pointer) { super(pointer); }

    public static GslMatrix createGslMatrix(int rows, int cols) {
      return new GslMatrix(GGslMatrix.createGslMatrix(rows, cols));
    }
    
    public void resize(int rows, int cols) {
      GGslMatrix.resize(this, rows, cols);
    }
    
    public void copy(GslMatrix rhs) { GGslMatrix.copy(this, rhs); }
    
    public double get(int i, int j) { return GGslMatrix.get(this, i, j); }
    
    public void set(int i, int j, double x) { GGslMatrix.set(this, i, j, x); }
    
    public void setAll(double x) { GGslMatrix.setAll(this, x); }
    
    public int getRows() { return GGslMatrix.getRows(this); }

    public int getCols() { return GGslMatrix.getCols(this); }

    public static void solve(GslMatrix X, GslVector y, GslVector b, GslPermutation perm) {
      GGslMatrix.solve(X, y, b, perm);
    }
    
    public static void invert(GslMatrix X, GslMatrix X_inv, GslPermutation perm) {
      GGslMatrix.invert(X, X_inv, perm);
    }
    
    public double det(GslPermutation perm) { return GGslMatrix.det(this, perm); }

}

