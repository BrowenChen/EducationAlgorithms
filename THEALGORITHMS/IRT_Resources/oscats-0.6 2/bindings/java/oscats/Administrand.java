/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Administrand Java Wrapper Class
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
import oscats.glib.GObject;

public class Administrand extends Object
{
    protected Administrand(long pointer) { super(pointer); }

    public Administrand() {
      this(OscatsAdministrand.new_with_params());
    }
    
    public Administrand(String id) {
      this(OscatsAdministrand.new_with_params("id", id));
    }
    
    static public void resetCharacteristics() {
      OscatsAdministrand.resetCharacteristics();
    }
    
    static public void registerCharacteristic (int characteristic) {
      OscatsAdministrand.registerCharacteristic(characteristic);
    }
    
    static public long characteristicFromString(String name) {
      return OscatsAdministrand.characteristicFromString(name);
    }
    
    static public String characteristicAsString(long characteristic) {
      return OscatsAdministrand.characteristicAsString(characteristic);
    }
    
    public void setCharacteristic(int characteristic) {
      OscatsAdministrand.setCharacteristic(this, characteristic);
    }
    
    public void clearCharacteristic(int characteristic) {
      OscatsAdministrand.clearCharacteristic(this, characteristic);
    }
    
    public void clearCharacteristics() {
      OscatsAdministrand.clearCharacteristics(this);
    }
    
    public boolean hasCharacteristic(int characteristic) {
      return OscatsAdministrand.hasCharacteristic(this, characteristic);
    }
    
    public void characteristicsIterReset() {
      OscatsAdministrand.characteristicsIterReset(this);
    }
    
    public long characteristicsIterNext() {
      return OscatsAdministrand.characteristicsIterNext(this);
    }

    public void setDefaultModel(String name) {
      OscatsAdministrand.setDefaultModel(this, GObject.quarkFromString(name));
    }

    public String getDefaultModel() {
      return GObject.quarkToString(OscatsAdministrand.getDefaultModel(this));
    }

    public void setModel(String name, Model model) {
      OscatsAdministrandOverride.setModel(this, GObject.quarkFromString(name), model);
    }

    public void setModel(Model model) {  // set default model
      OscatsAdministrand.setModel(this, 0, model);
    }

    public Model getModel(String name) {
      return OscatsAdministrand.getModel(this, GObject.quarkFromString(name));
    }

    public Model getModel() {  // get default model
      return OscatsAdministrand.getModel(this, 0);
    }

}

