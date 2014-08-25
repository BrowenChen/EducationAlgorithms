/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Multivariate Integration
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

/**
 * SECTION:integrate
 * @title:OscatsIntegrate
 * @short_description: Multivariate Integration
 */

#include "integrate.h"
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>

#define WS_SIZE 32

G_DEFINE_TYPE(OscatsIntegrate, oscats_integrate, G_TYPE_OBJECT);

static void oscats_integrate_finalize (GObject *object);
                   
static void oscats_integrate_class_init (OscatsIntegrateClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
//  GParamSpec *pspec;

  gobject_class->finalize = oscats_integrate_finalize;

}

static void oscats_integrate_init (OscatsIntegrate *self)
{
  self->tol = 1e-6;
  self->rem = 1;
  self->F.params = self;
}

static void oscats_integrate_clear (OscatsIntegrate *self)
{
  if (self->x) g_object_unref(self->x);
  self->x = NULL;
  if (self->min) g_free(self->min);
  if (self->max) g_free(self->max);
  if (self->z) gsl_vector_free(self->z);
  if (self->mu) gsl_vector_free(self->mu);
  if (self->B) gsl_matrix_free(self->B);
  if (self->ws)
  {
    guint i;
    for (i=0; i < self->dims; i++)
      gsl_integration_workspace_free(self->ws[i]);
    g_free(self->ws);
  }
  self->min = NULL;
  self->max = NULL;
  self->z = NULL;
  self->mu = NULL;
  self->B = NULL;
  self->ws = NULL;
  self->f = NULL;
}

static void oscats_integrate_finalize (GObject *object)
{
  OscatsIntegrate *self = OSCATS_INTEGRATE(object);
  oscats_integrate_clear(self);
  G_OBJECT_CLASS(oscats_integrate_parent_class)->finalize(object);
}

static double integrate_box(double x, void *data)
{
  OscatsIntegrate *self = (OscatsIntegrate*)data;
  guint level = self->level;
  if (level > 0) self->x->v->data[level-1] = x;
  if (level < self->dims)		// Recurse
  {
    gdouble I, err;
    self->level++;
    gsl_integration_qag(&(self->F), self->min[level], self->max[level],
                        self->tol, self->tol, WS_SIZE, GSL_INTEG_GAUSS15,
                        self->ws[level], &I, &err);
    self->level--;
    return I;
  } else				// Integrand
    return (*(self->f))(self->x, self->data);
}

// Note x must be 0 on first call!
static double integrate_ellipse(double x, void *data)
{
  OscatsIntegrate *self = (OscatsIntegrate*)data;
  guint level = self->level;
  if (level > 0) self->z->data[level-1] = x;
  if (level < self->dims)		// Recurse
  {
    gdouble I, err, delta, rem = self->rem;
    self->rem -= x*x;
    delta = sqrt(self->rem);
    self->level++;
    gsl_integration_qag(&(self->F), -delta, delta, self->tol, self->tol,
                        WS_SIZE, GSL_INTEG_GAUSS15, self->ws[level],
                        &I, &err);
    self->level--;
    self->rem = rem;
    return I;
  } else {				// Integrand
    gsl_vector_memcpy(self->x->v, self->z);
    gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, self->B, self->x->v);
    gsl_vector_add(self->x->v, self->mu);
    return (*(self->f))(self->x, self->data);
  }
}

static double integrate_space(double x, void *data)
{
  OscatsIntegrate *self = (OscatsIntegrate*)data;
  if (self->level > 0) self->x->v->data[self->level-1] = x;
  if (self->level < self->dims)		// Recurse
  {
    gdouble I, err;
    self->level++;
    gsl_integration_qagi(&(self->F), self->tol, self->tol, WS_SIZE,
                         self->ws[self->level-1], &I, &err);
    self->level--;
    return I;
  } else				// Integrand
    return (*(self->f))(self->x, self->data);
}

/**
 * oscats_integrate_set_tol:
 * @integrator: an #OscatsIntegrate
 * @tol: a tolerance level
 *
 * Sets the integration tolerance.  The default is 1e-6.
 */
void oscats_integrate_set_tol(OscatsIntegrate *integrator, gdouble tol)
{
  g_return_if_fail(OSCATS_IS_INTEGRATE(integrator) && tol > 0);
  integrator->tol = tol;
}

/**
 * oscats_integrate_set_c_function:
 * @integrator: an #OscatsIntegrate
 * @dims: the dimension of the function
 * @f: the function to integrate
 *
 * Sets the function to integrate.
 */
void oscats_integrate_set_c_function(OscatsIntegrate *integrator, guint dims, OscatsIntegrateFunction f)
{
  guint i;
  g_return_if_fail(OSCATS_IS_INTEGRATE(integrator) && dims > 0 && f != NULL);
  if (integrator->dims != dims)
  {
    oscats_integrate_clear(integrator);
    integrator->dims = dims;
    integrator->x = g_gsl_vector_new(dims);
    integrator->min = g_new(gdouble, dims);
    integrator->max = g_new(gdouble, dims);
    integrator->z = gsl_vector_calloc(dims);
    integrator->mu = gsl_vector_calloc(dims);
    integrator->B = gsl_matrix_calloc(dims, dims);
    integrator->ws = g_new(gsl_integration_workspace*, dims);
    for (i=0; i < dims; i++)
      integrator->ws[i] = gsl_integration_workspace_alloc(WS_SIZE);
  }
  integrator->f = f;
}

/**
 * oscats_integrate_cube:
 * @integrator: an #OscatsIntegrate object with function set
 * @mu: a vector indicating the center of the cube (or %NULL for the origin)
 * @delta: the half-width of the cube
 * @data: parameters for the function (or %NULL)
 *
 * Integrates the set function over [mu-delta, mu+delta].
 *
 * Returns: the value of the integral
 */
gdouble oscats_integrate_cube(OscatsIntegrate *integrator, GGslVector *mu, gdouble delta, gpointer data)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_INTEGRATE(integrator) && integrator->f != NULL, 0);
  if (mu) g_return_val_if_fail(G_GSL_IS_VECTOR(mu) && mu->v->size == integrator->dims, 0);
  for (i=0; i < integrator->dims; i++)
  {
    integrator->min[i] = (mu ? mu->v->data[i*mu->v->stride] : 0) - delta;
    integrator->max[i] = (mu ? mu->v->data[i*mu->v->stride] : 0) + delta;
  }
  integrator->data = data;
  integrator->F.function = integrate_box;
  return integrate_box(0, integrator);
}

/**
 * oscats_integrate_box:
 * @integrator: an #OscatsIntegrate object with function set
 * @min: the lower bounds
 * @max: the upper bounds
 * @data: parameters for the function (or %NULL)
 *
 * Integrates the set function over [min, max]
 *
 * Returns: the value of the integral
 */
gdouble oscats_integrate_box(OscatsIntegrate *integrator, GGslVector *min, GGslVector *max, gpointer data)
{
  guint i;
  g_return_val_if_fail(OSCATS_IS_INTEGRATE(integrator) && integrator->f != NULL, 0);
  g_return_val_if_fail(G_GSL_IS_VECTOR(min) && min->v->size == integrator->dims, 0);
  g_return_val_if_fail(G_GSL_IS_VECTOR(max) && max->v->size == integrator->dims, 0);
  for (i=0; i < integrator->dims; i++)
  {
    integrator->min[i] = min->v->data[i*min->v->stride];
    integrator->max[i] = max->v->data[i*max->v->stride];
    g_return_val_if_fail(integrator->min[i] < integrator->max[i], 0);
  }
  integrator->data = data;
  integrator->F.function = integrate_box;
  return integrate_box(0, integrator);
}

/**
 * oscats_integrate_ellipse:
 * @integrator: an #OscatsIntegrate object with function set
 * @mu: the center of the ellipse (or %NULL for the origin)
 * @Sigma: the orientation and shape of the ellipse (or %NULL for a sphere)
 * @c: the dilation size of the ellipse (> 0)
 * @data: parameters for the function (or %NULL)
 *
 * Integrates the set function over the ellipse defined by:
 * (x-mu)' Sigma^-1 (x-mu) <= c.
 *
 * This is accomplished by transformation of variables:
 * x = Bz + mu,
 * where B is the Cholesky decomposition of Sigma: c Sigma = BB'.
 * The integration is then performed over the sphere ||z||^2 <= 1.
 * Note that @Sigma must be symmetric and positive definite.
 *
 * Returns: the value of the integral
 */
gdouble oscats_integrate_ellipse(OscatsIntegrate *integrator, GGslVector *mu, GGslMatrix *Sigma, gdouble c, gpointer data)
{
  gdouble det = 1;
  guint i;
  g_return_val_if_fail(OSCATS_IS_INTEGRATE(integrator) &&
                       integrator->f != NULL, 0);
  if (mu) g_return_val_if_fail(G_GSL_IS_VECTOR(mu) &&
                               mu->v->size == integrator->dims, 0);
  if (Sigma) g_return_val_if_fail(G_GSL_IS_MATRIX(Sigma) &&
                                  Sigma->v->size1 == integrator->dims &&
                                  Sigma->v->size2 == integrator->dims, 0);
  g_return_val_if_fail(c > 0, 0);
  if (mu)
    gsl_vector_memcpy(integrator->mu, mu->v);
  else
    gsl_vector_set_zero(integrator->mu);
  if (Sigma)
  {
    gsl_matrix_memcpy(integrator->B, Sigma->v);
    gsl_matrix_scale(integrator->B, c);
    gsl_linalg_cholesky_decomp(integrator->B);
  }
  else
  {
    gsl_matrix_set_identity(integrator->B);
    gsl_matrix_scale(integrator->B, c);
  }
  for (i=0; i < integrator->dims; i++)
    det *= integrator->B->data[i*integrator->B->tda+i];
  integrator->data = data;
  integrator->F.function = integrate_ellipse;
  return integrate_ellipse(0, integrator) * det;
}

/**
 * oscats_integrate_space:
 * @integrator: an #OscatsIntegrate object with function set
 * @data: parameters for the function (or %NULL)
 *
 * Integrates the set function over the whole space R^n.
 *
 * Returns: the value of the integral
 */
gdouble oscats_integrate_space(OscatsIntegrate *integrator, gpointer data)
{
  g_return_val_if_fail(OSCATS_IS_INTEGRATE(integrator) && integrator->f != NULL, 0);
  integrator->data = data;
  integrator->F.function = integrate_space;
  return integrate_space(0, integrator);
}

/**
 * oscats_integrate_link_point:
 * @integrator: an #OscatsIntegrate object with function already set
 * @point: an #OscatsPoint to link
 *
 * Links the internal integration variable used by @integrator with the
 * continuous dimensions of @point so that when the integration variable is
 * changed, @point is moved as well.  The continuous dimension of @point must
 * be the same as the integration dimensions.
 */
void oscats_integrate_link_point(OscatsIntegrate *integrator, OscatsPoint *point)
{
  g_return_if_fail(OSCATS_IS_INTEGRATE(integrator));
  g_return_if_fail(OSCATS_IS_POINT(point) && OSCATS_IS_SPACE(point->space));
  g_return_if_fail(integrator->dims == point->space->num_cont);
  if (integrator->x) g_object_unref(integrator->x);
  integrator->x = oscats_point_cont_as_vector(point);
  g_object_ref(integrator->x);
}
