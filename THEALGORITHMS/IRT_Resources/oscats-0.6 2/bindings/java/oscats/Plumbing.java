/* OSCATS: Open-Source Computerized Adaptive Testing System
 * OSCATS Plumbing Class
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

import java.io.InputStream;

public abstract class Plumbing extends oscats.glib.Plumbing
{
  protected Plumbing() {}
  
  static
  {
    try { System.loadLibrary("oscatsjni"); }
    catch (java.lang.UnsatisfiedLinkError e1) {
      try { System.loadLibrary("liboscatsjni-0"); }
      catch (java.lang.UnsatisfiedLinkError e2) {
        throw new java.lang.UnsatisfiedLinkError(e1.toString() + "\n" + e2.toString());
      }
    }
    registerTypes("oscats.typeMapping");
  }
  
}
