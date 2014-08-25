/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Probability Distribution wrappers for GSL
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

/**
 * SECTION:random
 * @title:Random
 * @short_description: Wrapper for GSL random number generators and
 * distribution functions
 */

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_blas.h>
#include "gsl.h"
#include "random.h"

static gsl_rng *global_rng = NULL;

#define CHECK_INIT { if (!global_rng) {					\
  global_rng = gsl_rng_alloc(gsl_rng_mt19937);				\
  gsl_rng_set(global_rng, g_random_int()); }}

/**
 * oscats_rnd_uniform_int:
 *
 * Returns: a uniformly random integer on [0, 2^32)
 */
guint32 oscats_rnd_uniform_int()
{
  CHECK_INIT;
  return gsl_rng_get(global_rng);
}

/**
 * oscats_rnd_uniform_int_range:
 * @min: the minimum
 * @max: the maximum
 *
 * Returns: a uniformly random integer on [@min, @max]
 */
gint oscats_rnd_uniform_int_range(gint min, gint max)
{
  guint range = max-min;
  if (range == 0) return min;
  g_return_val_if_fail(range >= 0, 0);
  CHECK_INIT;
  return (gint)((range+1)*gsl_rng_uniform(global_rng)) + min;
}

/**
 * oscats_rnd_uniform:
 *
 * Returns: a uniformly random number on [0,1).
 */
gdouble oscats_rnd_uniform()
{
  CHECK_INIT;
  return gsl_rng_uniform(global_rng);
}

/**
 * oscats_rnd_uniform_range:
 * @min: minimum
 * @max: maximum
 *
 * Returns: a uniformly random number on [@min,@max).
 */
gdouble oscats_rnd_uniform_range(gdouble min, gdouble max)
{
  g_return_val_if_fail(min < max, 0);
  CHECK_INIT;
  return gsl_ran_flat(global_rng, min, max);
}

/**
 * oscats_rnd_normal:
 * @sd: standard deviation
 *
 * The mean-zero normal distribution has pdf
 * f_norm(x|sd) = exp(-x^2 / (2 sd^2)) / (sqrt(2 pi) sd).
 *
 * Must have: @sd > 0.
 *
 * Returns: a Normally distributed random number
 */
gdouble oscats_rnd_normal(gdouble sd)
{
  g_return_val_if_fail(sd > 0, 0);
  CHECK_INIT;
  return gsl_ran_gaussian_ratio_method(global_rng, sd);
}

/**
 * oscats_rnd_normal_p:
 * @x: sampled value
 * @sd: standard deviation
 *
 * Computes the upper tail of the Normal cdf: 
 * p_norm(x|sd) = int_{-\infty}^x f_norm(t|sd) dt.
 *
 * Must have: @sd > 0.
 *
 * Returns: the upper-tail p-value
 */
gdouble oscats_rnd_normal_p(gdouble x, gdouble sd)
{
  g_return_val_if_fail(sd > 0, 0);
  return gsl_cdf_gaussian_Q(x, sd);
}

/**
 * oscats_rnd_binorm:
 * @sdx : standard deviation for first variable
 * @sdy : standard deviation for second variable
 * @rho : correlation coefficient
 * @X : pointer to return first variable
 * @Y : pointer to return second variable
 *
 * The mean-zero bivariate normal distribution has pdf
 * f_binorm(x,y|sdx,sdy) = exp(-(x^2/sdx^2 + y^2/sdy^2 
 * - 2 rho x y / (sdx sdy)) / 2(1-rho^2)) / (2 pi sdx sdy).
 * Note that cov(X,Y) = rho * sdx * sdy.
 *
 * Must have: @sdx > 0, @sdy > 0, -1 <= @rho <= 1.
 */
void oscats_rnd_binorm(gdouble sdx, gdouble sdy, gdouble rho,
                       gdouble *X, gdouble *Y)
{
  g_return_if_fail(sdx > 0 && sdy > 0 && -1 <= rho && rho <= 1);
  g_return_if_fail(X && Y);
  CHECK_INIT;
  gsl_ran_bivariate_gaussian(global_rng, sdx, sdy, rho, X, Y);
}

/**
 * oscats_rnd_multinorm_prep:
 * @sigma : covariance matrix for multivariate normal distribution
 * @sigma_half : return matrix
 *
 * Prepares a lower-triangular matrix A such that AA'=Sigma for
 * multivariate-normal random-number generation.  This is achieved by
 * Cholesky decomposition.
 *
 * Must have: @sigma symmetric and positive definite,
 * @sigma and @sigma_half with same size > 2.
 */
void oscats_rnd_multinorm_prep(const GGslMatrix *sigma, GGslMatrix *sigma_half)
{
  g_return_if_fail(G_GSL_IS_MATRIX(sigma) && G_GSL_IS_MATRIX(sigma_half) &&
                   sigma->v && sigma_half->v && sigma->v->size1 > 2);
  gsl_matrix_memcpy(sigma_half->v, sigma->v);
  gsl_linalg_cholesky_decomp(sigma_half->v);
}

/**
 * oscats_rnd_multinorm:
 * @mu: mean
 * @sigma_half : half of covariance matrix
 * @x : return for random vector
 *
 * A random vector @x of length @n is multivariate normal if
 * @x = @mu + @A @z, where AA' = Sigma is the covariance matrix of the
 * random vector, and @z is a vector of indepenedent standard normal 
 * random variables.
 *
 * Must have: @sigma_half lower-triangular with same size as @mu and @x.
 */
void oscats_rnd_multinorm(const GGslVector *mu, const GGslMatrix *sigma_half,
                          GGslVector *x)
{
  int i, n;
  g_return_if_fail(G_GSL_IS_VECTOR(mu) && G_GSL_IS_MATRIX(sigma_half) &&
                   G_GSL_IS_VECTOR(x) && mu->v && sigma_half->v && x->v);
  CHECK_INIT;
  n = mu->v->size;
  for (i=0; i < n; i++)
    gsl_vector_set(x->v, i, gsl_ran_gaussian_ratio_method(global_rng, 1));
  gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit,
                 sigma_half->v, x->v);  // x = Ax, A is lower triangular
  gsl_vector_add(x->v, mu->v);
}
                          
/**
 * oscats_ran_exp:
 * @mu: mean
 *
 * The mean-mu exponential distribution has pdf
 * f_exp(x|mu) = exp(-x/mu) / mu, for x >= 0.
 *
 * Must have: @mu > 0.
 *
 * Returns: an exponentially distributed random number
 */
gdouble oscats_rnd_exp(gdouble mu)
{
  g_return_val_if_fail(mu > 0, 0);
  CHECK_INIT;
  return gsl_ran_exponential(global_rng, mu);
}

/**
 * oscats_ran_gamma:
 * @a: shape parameter
 * @b: scale parameter
 *
 * The Gamma distribution has pdf
 * f_gamma(x|a,b) = x^(a-1) e^(-x/b) / (Gamma(a) b^a), for x > 0.
 *
 * Returns: a Gamma-distributed random number
 */
gdouble oscats_rnd_gamma(gdouble a, gdouble b)
{
  CHECK_INIT;
  return gsl_ran_gamma(global_rng, a, b);
}

/**
 * oscats_rnd_chisq_p:
 * @x: sampled value
 * @nu: degrees of freedom
 *
 * If y_1, ..., y_nu are standard normally distributed random numbers,
 * then sum_1^nu y_i^2 is distributed Chi^2 with nu degrees of freedom.
 * Computes the upper tail of the Chi^2 cdf with @nu degrees of freedom:
 * p_chisq(x|nu) = int_{-\infty}^x (t/2)^(nu^2-1) exp(-t/2) / 2 Gamma(nu/2) dt.
 *
 * Must have: @x >= 0.
 *
 * Returns: the upper-tail p-value
 */
gdouble oscats_rnd_chisq_p(gdouble x, gdouble nu)
{
  g_return_val_if_fail(x >= 0, 0);
  return gsl_cdf_chisq_Q(x, nu);
}

/**
 * oscats_rnd_F_p:
 * @x: sampled value
 * @nu1: degrees of freedom for numerator
 * @nu2: degrees of freedom for denominator
 *
 * If y_1 is distributed Chi^2 with nu_1 degrees of freedom, and
 * y_2 is distributed Chi^2 with nu_2 degrees of freedom,
 * w = (y_1/nu_1) / (y_2/nu_2) is distributed F(nu_1, nu_2).
 * Computes the upper tail of the F cdf with @nu1 and @nu2 degrees of freedom.
 *
 * Must have: @x >= 0.
 *
 * Returns: the upper-tail p-value
 */
gdouble oscats_rnd_F_p(gdouble x, gdouble nu1, gdouble nu2)
{
  g_return_val_if_fail(x >= 0, 0);
  return gsl_cdf_fdist_Q(x, nu1, nu2);
}

/**
 * oscats_rnd_t_p:
 * @x: sampled value
 * @nu: degrees of freedom
 *
 * If y_1 is distributed standard normal, and
 * y_2 is distributed Chi^2 with nu degrees of freedom,
 * w = y_1 / (y_2/nu) is distributed t(nu).
 * Computes the upper tail of the t cdf with @nu degrees of freedom.
 *
 * Returns: the upper-tail p-value
 */
gdouble oscats_rnd_t_p(gdouble x, gdouble nu)
{
  return gsl_cdf_tdist_Q(x, nu);
}

/**
 * oscats_ran_beta:
 * @a: shape parameter
 * @b: scale parameter
 *
 * The Beta distribution has pdf
 * f_beta(x|a,b) = x^(a-1) (1-x)^(b-1) Gamma(a+b) / (Gamma(a) Gamma(b)),
 * for 0 <= x <= 1.
 *
 * Returns: a Beta-distributed random number
 */
gdouble oscats_rnd_beta(gdouble a, gdouble b)
{
  CHECK_INIT;
  return gsl_ran_beta(global_rng, a, b);
}

/**
 * oscats_ran_dirichlet:
 * @alpha: parameter vector
 * @x: return vector for Dirichlet-distributed random vector
 *
 * The Dirichlet distribution has pdf
 * f_dirichlet(x|alpha) proportional to
 * prod_1^K x_i^(alpha_i-1), where x and alpha are K-dimensional vectors.
 * The values x_i sum to 1.
 */
void oscats_rnd_dirichlet(const GGslVector *alpha, GGslVector *x)
{
  g_return_if_fail(G_GSL_IS_VECTOR(alpha) && G_GSL_IS_VECTOR(x) &&
                   alpha->v && x->v && alpha->v->size == x->v->size);
  CHECK_INIT;
  gsl_ran_dirichlet(global_rng, alpha->v->size, alpha->v->data, x->v->data);
}

/**
 * oscats_ran_poisson:
 * @mu: mean
 *
 * The Poisson distribution has pdf
 * f_pois(x|mu) = mu^x exp(-mu) / x!, for x a non-negative integer.
 *
 * Must have @mu > 0.
 *
 * Returns: a Poisson-distributed integer
 */
guint oscats_rnd_poisson(gdouble mu)
{
  g_return_val_if_fail(mu > 0, 0);
  CHECK_INIT;
  return gsl_ran_poisson(global_rng, mu);
}

/**
 * oscats_ran_binomial:
 * @n: number of trials
 * @p: success probability of each trial
 *
 * The binomial distribution has pdf
 * f_binomial(x|n,p) = (n!/x!(n-x)!) p^x (1-p)^x, for 0 <= p <= 1,
 * and x an integer between 0 and n, inclusive.
 *
 * Returns: a binomially distributed integer
 */
guint oscats_rnd_binomial(guint n, gdouble p)
{
  g_return_val_if_fail(0 <= p && p <= 1 && n > 0, 0);
  CHECK_INIT;
  return gsl_ran_binomial(global_rng, p, n);
}

/**
 * oscats_ran_multinomial:
 * @n: number of trials
 * @p: success probabilities for each category
 * @x: return vector (#guint) for vector of counts
 *
 * The multinomial distribution has pdf
 * f_binomial(x|n,p) = (n!/x_1!...x_k!) p_1^(x_1) ... p_k^(x_k),
 * for 0 <= p_i <= 1, sum_1^k p_i = 1, and sum_1^k x_i = n.
 */
void oscats_rnd_multinomial(guint n, const GGslVector *p, GArray *x)
{
  g_return_if_fail(G_GSL_IS_VECTOR(p) && p->v && x);
  if (p->v->size != x->len) g_array_set_size(x, p->v->size);
  CHECK_INIT;
  return gsl_ran_multinomial(global_rng, x->len, n, p->v->data,
                             (guint*)(x->data));
}

/**
 * oscats_ran_hypergeometric:
 * @n1: number of elements of type 1
 * @n2: number of elements of type 2
 * @N: numer of samples to draw without replacement
 *
 * The hypergeometric distribution has pdf
 * f_hypergeom(x|n_1,n_2,N) = C(n_1,x) C(n_2, t-x) / C(n_1+n_2, x).
 *
 * Returns: a hypergeometrically distributed integer
 */
guint oscats_rnd_hypergeometric(guint n1, guint n2, guint N)
{
  g_return_val_if_fail(N < n1+n2, 0);
  CHECK_INIT;
  return gsl_ran_hypergeometric(global_rng, n1, n2, N);
}

/**
 * oscats_rnd_sample:
 * @population: objects from which to sample
 * @num: number of objects to sample
 * @sample: return vector of sampled objects
 * @replace: sample with replacement?
 *
 * Pointers are <emphasis>not</emphasis> ref'd when added to @sample.
 * Moreover, @sample should <emphasis>not</emphasis> have a #GDestroyNotify.
 */
void oscats_rnd_sample(const GPtrArray *population, guint num,
                       GPtrArray *sample, gboolean replace)
{
  g_return_if_fail(population && sample);
  g_return_if_fail((!replace && population->len < num) ||
                    (replace && population->len == 0) );
  g_ptr_array_set_size(sample, num);
  CHECK_INIT;
  if (replace)
    gsl_ran_sample(global_rng, sample->pdata, num,
                   population->pdata, population->len, sizeof(gpointer));
  else
    gsl_ran_choose(global_rng, sample->pdata, num,
                   population->pdata, population->len, sizeof(gpointer));
}
