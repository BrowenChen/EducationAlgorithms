/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Wrapper for GNU Scientific Library Data Structures
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
 * SECTION:gsl
 * @title:GSL
 * @short_description: Wrapper for GNU Scientific Library data structures
 */

#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN "GSL"
#include "gsl.h"
#include <gsl/gsl_errno.h>
#include <gsl/gsl_linalg.h>

static void gsl_err_handler(const char * reason, const char * file,
                            int line, int gsl_errno)
{
  g_critical("Error %d: %s.", gsl_errno, reason);
}

G_DEFINE_TYPE(GGslVector, g_gsl_vector, G_TYPE_OBJECT);

static void g_gsl_vector_finalize (GObject *object);

static void g_gsl_vector_class_init (GGslVectorClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = g_gsl_vector_finalize;
  gsl_set_error_handler(gsl_err_handler);
}

static void g_gsl_vector_init (GGslVector *self)
{
}

static void g_gsl_vector_finalize (GObject *object)
{
  GGslVector *self = G_GSL_VECTOR(object);
  if (self->v) gsl_vector_free(self->v);
  G_OBJECT_CLASS(g_gsl_vector_parent_class)->finalize(object);
}

GGslVector* g_gsl_vector_new(guint N)
{
  GGslVector *r = (GGslVector*)g_object_new(G_TYPE_GSL_VECTOR, NULL);
  r->v = gsl_vector_calloc(N);
  return r;
}

void g_gsl_vector_resize(GGslVector *v, guint N)
{
  if (v->v)
  {
    if (v->v->size == N) return;
    gsl_vector_free(v->v);
  }
  v->v = gsl_vector_calloc(N);
}

void g_gsl_vector_copy(GGslVector *lhs, const GGslVector *rhs)
{ gsl_vector_memcpy(lhs->v, rhs->v); }

gdouble g_gsl_vector_get(const GGslVector *v, guint i)
{ return gsl_vector_get(v->v, i); }

void g_gsl_vector_set(GGslVector *v, guint i, gdouble x)
{ gsl_vector_set(v->v, i, x); }

void g_gsl_vector_set_all(GGslVector *v, gdouble X)
{ gsl_vector_set_all(v->v, X); }

guint g_gsl_vector_get_size(const GGslVector *v)
{ return v->v->size; }


G_DEFINE_TYPE(GGslMatrix, g_gsl_matrix, G_TYPE_OBJECT);

static void g_gsl_matrix_finalize (GObject *object);

static void g_gsl_matrix_class_init (GGslMatrixClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = g_gsl_matrix_finalize;
  gsl_set_error_handler(gsl_err_handler);
}

static void g_gsl_matrix_init (GGslMatrix *self)
{
}

static void g_gsl_matrix_finalize (GObject *object)
{
  GGslMatrix *self = G_GSL_MATRIX(object);
  if (self->v) gsl_matrix_free(self->v);
  G_OBJECT_CLASS(g_gsl_matrix_parent_class)->finalize(object);
}

GGslMatrix* g_gsl_matrix_new(guint rows, guint cols)
{
  GGslMatrix *ret = (GGslMatrix*)g_object_new(G_TYPE_GSL_MATRIX, NULL);
  ret->v = gsl_matrix_calloc(rows, cols);
  return ret;
}

void g_gsl_matrix_resize(GGslMatrix *v, guint rows, guint cols)
{
  if (v->v)
  {
    if (v->v->size1 == rows && v->v->size2 == cols) return;
    gsl_matrix_free(v->v);
  }
  v->v = gsl_matrix_calloc(rows, cols);
}

void g_gsl_matrix_copy(GGslMatrix *lhs, const GGslMatrix *rhs)
{ gsl_matrix_memcpy(lhs->v, rhs->v); }

gdouble g_gsl_matrix_get(const GGslMatrix *v, guint row, guint col)
{ return gsl_matrix_get(v->v, row, col); }

void g_gsl_matrix_set(GGslMatrix *v, guint row, guint col, gdouble x)
{ gsl_matrix_set(v->v, row, col, x); }

void g_gsl_matrix_set_all(GGslMatrix *v, gdouble X)
{ gsl_matrix_set_all(v->v, X); }

guint g_gsl_matrix_get_rows(const GGslMatrix *v)
{ return v->v->size1; }

guint g_gsl_matrix_get_cols(const GGslMatrix *v)
{ return v->v->size2; }

/**
 * g_gsl_matrix_solve:
 * @X: a square #GGslMatrix of size @N
 * @y: a #GGslVector of length @N
 * @b: a #GGslVector of length @N to receive the result
 * @p: a #GGslPermutation of length @N for working space
 *
 * Computes @b for the equation @y = @X @b.
 * Note: @X is overwritten with its LU decomposition.
 */
void g_gsl_matrix_solve(GGslMatrix *X, const GGslVector *y,
                        GGslVector *b, GGslPermutation *p)
{
  gdouble det;
  int i;
  g_return_if_fail(G_GSL_IS_MATRIX(X) && G_GSL_IS_VECTOR(y) &&
                   G_GSL_IS_VECTOR(b) && G_GSL_IS_PERMUTATION(p));
  g_return_if_fail(X->v->size1 == X->v->size2 &&
                   X->v->size1 == y->v->size  &&
                   X->v->size1 == b->v->size  &&
                   X->v->size1 == p->v->size);
  switch (X->v->size1)
  {
    case 1:
      b->v->data[0] = y->v->data[0]/X->v->data[0];
      break;
    
    case 2:
      det = X->v->data[0]*X->v->data[3] - X->v->data[1]*X->v->data[2];
      b->v->data[0] =
        (X->v->data[3]*y->v->data[0] -
         X->v->data[2]*y->v->data[y->v->stride]) / det;
      b->v->data[b->v->stride] =
        (X->v->data[0]*y->v->data[y->v->stride] -
         X->v->data[1]*y->v->data[0]) / det;
      break;

    default:
      gsl_linalg_LU_decomp(X->v, p->v, &i);
      gsl_linalg_LU_solve(X->v, p->v, y->v, b->v);
  }
}

/**
 * g_gsl_matrix_invert:
 * @X: a square #GGslMatrix of size @N
 * @X_inv: a square #GGslMatrix of length @N to receive the result
 * @p: a #GGslPermutation of length @N for working space
 *
 * Computes the inverse of @X.
 * Note: @X is overwritten with its LU decomposition.
 */
void g_gsl_matrix_invert(GGslMatrix *X, GGslMatrix *X_inv,
                         GGslPermutation *p)
{
  gdouble det;
  int i;
  g_return_if_fail(G_GSL_IS_MATRIX(X) && G_GSL_IS_MATRIX(X_inv) &&
                   G_GSL_IS_PERMUTATION(p));
  g_return_if_fail(X->v->size1 == X->v->size2 &&
                   X_inv->v->size1 == X_inv->v->size2 &&
                   X->v->size1 == p->v->size);
  switch (X->v->size1)
  {
    case 1:
      X_inv->v->data[0] = 1/X->v->data[0];
      break;

    case 2:
      det = X->v->data[0]*X->v->data[3] - X->v->data[1]*X->v->data[2];
      X_inv->v->data[0] =  X->v->data[3]/det;
      X_inv->v->data[1] = -X->v->data[1]/det;
      X_inv->v->data[2] = -X->v->data[2]/det;
      X_inv->v->data[3] =  X->v->data[0]/det;
      break;

    default:
      gsl_linalg_LU_decomp(X->v, p->v, &i);
      gsl_linalg_LU_invert(X->v, p->v, X_inv->v);
  }
}
                        
/**
 * g_gsl_matrix_det:
 * @X: a square #GGslMatrix of size @N
 * @p: a #GGslPermutation of length @N for working space
 *
 * Computes teh determinant of @X.
 * Note that @X is overwritten with its LU decomposition.
 *
 * Returns: the determinant of @X.
 */
double g_gsl_matrix_det(GGslMatrix *X, GGslPermutation *p)
{
  int i;
  g_return_val_if_fail(G_GSL_IS_MATRIX(X) && G_GSL_IS_PERMUTATION(p), 0);
  g_return_val_if_fail(X->v->size1 == X->v->size2 &&
                       X->v->size1 == p->v->size, 0);
  switch (X->v->size1)
  {
    case 1:
      return X->v->data[0];
      break;

    case 2:
      return X->v->data[0]*X->v->data[3] - X->v->data[1]*X->v->data[2];
      break;

    default:
      gsl_linalg_LU_decomp(X->v, p->v, &i);
      return gsl_linalg_LU_det(X->v, i);
  }
}
                        

G_DEFINE_TYPE(GGslPermutation, g_gsl_permutation, G_TYPE_OBJECT);

static void g_gsl_permutation_finalize (GObject *object);

static void g_gsl_permutation_class_init (GGslPermutationClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = g_gsl_permutation_finalize;
  gsl_set_error_handler(gsl_err_handler);
}

static void g_gsl_permutation_init (GGslPermutation *self)
{
}

static void g_gsl_permutation_finalize (GObject *object)
{
  GGslPermutation *self = G_GSL_PERMUTATION(object);
  if (self->v) gsl_permutation_free(self->v);
  G_OBJECT_CLASS(g_gsl_permutation_parent_class)->finalize(object);
}

GGslPermutation* g_gsl_permutation_new(guint N)
{
  GGslPermutation *ret =
     (GGslPermutation*)g_object_new(G_TYPE_GSL_PERMUTATION, NULL);
  ret->v = gsl_permutation_calloc(N);
  return ret;
}

void g_gsl_permutation_resize(GGslPermutation *v, guint N)
{
  if (v->v)
  {
    if (v->v->size == N) return;
    gsl_permutation_free(v->v);
  }
  v->v = gsl_permutation_calloc(N);
}
