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

#ifndef _LIBOSCATS_RANDOM_H_
#define _LIBOSCATS_RANDOM_H_
#include <glib.h>
#include "gsl.h"
G_BEGIN_DECLS

guint32 oscats_rnd_uniform_int();
gint oscats_rnd_uniform_int_range(gint min, gint max);
gdouble oscats_rnd_uniform();
gdouble oscats_rnd_uniform_range(gdouble min, gdouble max);

gdouble oscats_rnd_normal(gdouble sd);
void oscats_rnd_binorm(gdouble sdx, gdouble sdy, gdouble rho,
                       gdouble *X, gdouble *Y);
void oscats_rnd_multinorm_prep(const GGslMatrix *sigma, GGslMatrix *sigma_half);
void oscats_rnd_multinorm(const GGslVector *mu, const GGslMatrix *sigma_half,
                          GGslVector *x);
gdouble oscats_rnd_exp(gdouble mu);
gdouble oscats_rnd_gamma(gdouble a, gdouble b);
gdouble oscats_rnd_beta(gdouble a, gdouble b);

void oscats_rnd_dirichlet(const GGslVector *alpha, GGslVector *x);
guint oscats_rnd_poisson(gdouble mu);
guint oscats_rnd_binomial(guint n, gdouble p);
void oscats_rnd_multinomial(guint n, const GGslVector *p, GArray *x);
guint oscats_rnd_hypergeometric(guint n1, guint n2, guint N);

gdouble oscats_rnd_normal_p(gdouble x, gdouble sd);
gdouble oscats_rnd_chisq_p(gdouble x, gdouble nu);
gdouble oscats_rnd_F_p(gdouble x, gdouble nu1, gdouble nu2);
gdouble oscats_rnd_t_p(gdouble x, gdouble nu);

void oscats_rnd_sample(const GPtrArray *population, guint num,
                       GPtrArray *sample, gboolean replace);

G_END_DECLS
#endif
