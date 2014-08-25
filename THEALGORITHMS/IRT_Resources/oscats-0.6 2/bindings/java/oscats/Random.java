/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Random Function Wrappers
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

public final class Random
{
    private Random() {}

    public static final int uniformInt() { return OscatsRandom.uniformInt(); }
    public static final int uniformIntRange(int min, int max) {
      return OscatsRandom.uniformIntRange(min, max);
    }
    public static final int uniformInt(int min, int max) {
      return OscatsRandom.uniformIntRange(min, max);
    }
    public static final double uniform() { return OscatsRandom.uniform(); }
    public static final double uniformRange(double min, double max) {
      return OscatsRandom.uniformRange(min, max);
    }
    public static final double uniform(double min, double max) {
      return OscatsRandom.uniformRange(min, max);
    }
    public static final double normal(double sd) { return OscatsRandom.normal(sd); }
    public static final void binorm(double sdx, double sdy, double rho, double[] X, double[] Y) {
      OscatsRandom.binorm(sdx, sdy, rho, X, Y);
    }
    public static final void multinormPrep(GslMatrix sigma, oscats.GslMatrix sigmaHalf) {
      OscatsRandom.multinormPrep(sigma, sigmaHalf);
    }
    public static final void multinorm(GslVector mu, GslMatrix sigmaHalf, oscats.GslVector x) {
      OscatsRandom.multinorm(mu, sigmaHalf, x);
    }
    public static final double exp(double mu) { return OscatsRandom.exp(mu); }
    public static final double gamma(double a, double b) { return OscatsRandom.gamma(a, b); }
    public static final double beta(double a, double b) { return OscatsRandom.beta(a, b); }
    public static final void dirichlet(GslVector alpha, oscats.GslVector x) {
      OscatsRandom.dirichlet(alpha, x);
    }
    public static final int poisson(double mu) { return OscatsRandom.poisson(mu); }
    public static final int binomial(int n, double p) { return OscatsRandom.binomial(n, p); }

//    public static final void multinomial(int n, GslVector p, FIXME x);

    public static final int hypergeometric(int n1, int n2, int N) {
      return OscatsRandom.hypergeometric(n1, n2, N);
    }
    public static final double normalP(double x, double sd) { return OscatsRandom.normalP(x, sd); }
    public static final double chisqP(double x, double nu) { return OscatsRandom.chisqP(x, nu); }
    public static final double FP(double x, double nu1, double nu2) {
      return OscatsRandom.FP(x, nu1, nu2);
    }
    public static final double tP(double x, double nu) {
      return OscatsRandom.tP(x, nu);
    }

//    public static final void sample(FIXME population, int num, FIXME sample, boolean replace);

}

