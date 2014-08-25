/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Examinee Java Wrapper Class
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

public final class Examinee extends Object
{
    protected Examinee(long pointer) { super(pointer); }

    public Examinee() {
      this(OscatsExaminee.new_with_params());
    }
    
    public Examinee(String id) {
      this(OscatsExaminee.new_with_params("id", id));
    }
    
    public Examinee(Covariates covariates) {
      this(OscatsExaminee.new_with_params("covariates", covariates));
    }
    
    public Examinee(String id, Covariates covariates) {
      this(OscatsExaminee.new_with_params("id", id, "covariates", covariates));
    }
    
    public void setSimKey(String name) {
      OscatsExaminee.setSimKey(this, GObject.quarkFromString(name));
    }
    
    public String getSimKey() {
      return GObject.quarkToString(OscatsExaminee.getSimKey(this));
    }
    
    public void setEstKey(String name) {
      OscatsExaminee.setEstKey(this, GObject.quarkFromString(name));
    }
    
    public String getEstKey() {
      return GObject.quarkToString(OscatsExaminee.getEstKey(this));
    }
    
    public void setSimTheta(Point theta) {
      OscatsExamineeOverride.setSimTheta(this, theta);
    }
    
    public Point getSimTheta() {
      return OscatsExaminee.getSimTheta(this);
    }
    
    public void setEstTheta(Point theta) {
      OscatsExamineeOverride.setEstTheta(this, theta);
    }
    
    public Point getEstTheta() {
      return OscatsExaminee.getEstTheta(this);
    }
    
    public void setTheta(String name, Point theta) {
      OscatsExamineeOverride.setTheta(this, GObject.quarkFromString(name), theta);
    }
    
    public Point getTheta(String name) {
      return OscatsExaminee.getTheta(this, GObject.quarkFromString(name));
    }
    
    public void initSimTheta(Space space) {
      OscatsExaminee.initSimTheta(this, space);
    }
    
    public void initEstTheta(Space space) {
      OscatsExaminee.initEstTheta(this, space);
    }
    
    public void initTheta(String name, Space space) {
      OscatsExaminee.initTheta(this, GObject.quarkFromString(name), space);
    }
    
    public int numItems() { return OscatsExaminee.numItems(this); }

    public void addItem(Item item, byte resp) {
      OscatsExaminee.addItem(this, item, resp);
    }
    
    public Item getItem(int i) {
      return OscatsExaminee.getItem(this, i);
    }
    
    public byte getResp(int i) {
      return OscatsExaminee.getResp(this, i);
    }
    
    public double logLik(Point theta, String model) {
      return OscatsExaminee.logLik(this, theta, GObject.quarkFromString(model));
    }

    public double logLik(Point theta) {
      return OscatsExaminee.logLik(this, theta, 0);
    }

}

