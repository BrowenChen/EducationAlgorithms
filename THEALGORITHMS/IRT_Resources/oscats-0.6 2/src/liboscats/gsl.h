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

#ifndef _LIBOSCATS_GSL_H_
#define _LIBOSCATS_GSL_H_
#include <glib-object.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
G_BEGIN_DECLS

#define G_TYPE_GSL_VECTOR		(g_gsl_vector_get_type())
#define G_GSL_VECTOR(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_GSL_VECTOR, GGslVector))
#define G_GSL_IS_VECTOR(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_GSL_VECTOR))
#define G_GSL_VECTOR_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_GSL_VECTOR, GGslVectorClass))
#define G_GSL_IS_VECTOR_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_GSL_VECTOR))
#define G_GSL_VECTOR_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_GSL_VECTOR, GGslVectorClass))

typedef struct _GGslVector GGslVector;
typedef struct _GGslVectorClass GGslVectorClass;
typedef struct _GGslMatrix GGslMatrix;
typedef struct _GGslMatrixClass GGslMatrixClass;
typedef struct _GGslPermutation GGslPermutation;
typedef struct _GGslPermutationClass GGslPermutationClass;

struct _GGslVector {
  GObject parent_instance;
  gsl_vector *v;
};

struct _GGslVectorClass {
  GObjectClass parent_class;
};

GType g_gsl_vector_get_type();

GGslVector* g_gsl_vector_new(guint N);
void g_gsl_vector_resize(GGslVector *v, guint N);
void g_gsl_vector_copy(GGslVector *lhs, const GGslVector *rhs);
gdouble g_gsl_vector_get(const GGslVector *v, guint i);
void g_gsl_vector_set(GGslVector *v, guint i, gdouble x);
void g_gsl_vector_set_all(GGslVector *v, gdouble X);
guint g_gsl_vector_get_size(const GGslVector *v);

#define G_TYPE_GSL_MATRIX		(g_gsl_matrix_get_type())
#define G_GSL_MATRIX(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_GSL_MATRIX, GGslMatrix))
#define G_GSL_IS_MATRIX(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_GSL_MATRIX))
#define G_GSL_MATRIX_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_GSL_MATRIX, GGslMatrixClass))
#define G_GSL_IS_MATRIX_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_GSL_MATRIX))
#define G_GSL_MATRIX_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_GSL_MATRIX, GGslMatrixClass))

struct _GGslMatrix {
  GObject parent_instance;
  gsl_matrix *v;
};

struct _GGslMatrixClass {
  GObjectClass parent_class;
};

GType g_gsl_matrix_get_type();

GGslMatrix* g_gsl_matrix_new(guint rows, guint cols);
void g_gsl_matrix_resize(GGslMatrix *v, guint rows, guint cols);
void g_gsl_matrix_copy(GGslMatrix *lhs, const GGslMatrix *rhs);
gdouble g_gsl_matrix_get(const GGslMatrix *v, guint row, guint col);
void g_gsl_matrix_set(GGslMatrix *v, guint row, guint col, gdouble x);
void g_gsl_matrix_set_all(GGslMatrix *v, gdouble X);
guint g_gsl_matrix_get_rows(const GGslMatrix *v);
guint g_gsl_matrix_get_cols(const GGslMatrix *v);
void g_gsl_matrix_solve(GGslMatrix *X, const GGslVector *y,
                        GGslVector *b, GGslPermutation *p);
void g_gsl_matrix_invert(GGslMatrix *X, GGslMatrix *X_inv, GGslPermutation *p);
double g_gsl_matrix_det(GGslMatrix *X, GGslPermutation *p);

#define G_TYPE_GSL_PERMUTATION		(g_gsl_permutation_get_type())
#define G_GSL_PERMUTATION(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_GSL_PERMUTATION, GGslPermutation))
#define G_GSL_IS_PERMUTATION(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_GSL_PERMUTATION))
#define G_GSL_PERMUTATION_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_GSL_PERMUTATION, GGslPermutationClass))
#define G_GSL_IS_PERMUTATION_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_GSL_PERMUTATION))
#define G_GSL_PERMUTATION_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_GSL_PERMUTATION, GGslPermutationClass))

struct _GGslPermutation {
  GObject parent_instance;
  gsl_permutation *v;
};

struct _GGslPermutationClass {
  GObjectClass parent_class;
};

GType g_gsl_permutation_get_type();

GGslPermutation* g_gsl_permutation_new(guint N);
void g_gsl_permutation_resize(GGslPermutation *v, guint N);

G_END_DECLS
#endif
