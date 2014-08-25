/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Model Java Wrapper Class
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

public abstract class Model extends Object
{
    protected Model(long pointer) { super(pointer); }

    public byte getMax() { return OscatsModel.getMax(this); }
    
    public double P(byte resp, Point theta, Covariates covariates) {
      return OscatsModel.P(this, resp, theta, covariates);
    }
    
    public double distance(Point theta, Covariates covariates) {
      return OscatsModel.distance(this, theta, covariates);
    }
    
    public void logLikDtheta(byte resp, Point theta, Covariates covariates, GslVector grad, GslMatrix hes) {
      OscatsModel.logLikDtheta(this, resp, theta, covariates, grad, hes);
    }
    
    public void logLikDparam(byte resp, Point theta, Covariates covariates, GslVector grad, GslMatrix hes) {
      OscatsModel.logLikDparam(this, resp, theta, covariates, grad, hes);
    }
    
    public void fisherInf(Point theta, Covariates covariates, GslMatrix I) {
      OscatsModel.fisherInf(this, theta, covariates, I);
    }
    
    public String getParamName(int i) { return OscatsModel.getParamName(this, i); }
    
    public boolean hasParamName(String name) {
      return OscatsModel.hasParamName(this, name);
    }
    
    public boolean hasParam(int param) { return OscatsModel.hasParam(this, param); }
    
    public double getParam(int param) { return OscatsModel.getParam(this, param); }
    
    public double getParamByIndex(int i) { return OscatsModel.getParamByIndex(this, i); }
    
    public double getParam(String name) {
      return OscatsModel.getParamByName(this, name);
    }
    
    public double getParamByName(String name) {
      return OscatsModel.getParamByName(this, name);
    }
    
    public void setParam(int param, double x) {
      OscatsModel.setParam(this, param, x);
    }
    
    public void setParamByIndex(int i, double x) {
      OscatsModel.setParamByIndex(this, i, x);
    }
    
    public void setParam(String name, double x) {
      OscatsModel.setParamByName(this, name, x);
    }
    
    public void setParamByName(String name, double x) {
      OscatsModel.setParamByName(this, name, x);
    }

    public boolean hasCovariate(int name) { return OscatsModel.hasCovariate(this, name); }
    
    public boolean hasCovariate(String name) { return OscatsModel.hasCovariateName(this, name); }
    
    public boolean hasCovariateName(String name) { return OscatsModel.hasCovariateName(this, name); }
    
}

