/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Track classification rates
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

#ifndef _LIBOSCATS_ALGORITHM_CLASS_RATES_H_
#define _LIBOSCATS_ALGORITHM_CLASS_RATES_H_
#include <glib-object.h>
#include <item.h>
#include <algorithm.h>
G_BEGIN_DECLS

#define OSCATS_TYPE_ALG_CLASS_RATES	(oscats_alg_class_rates_get_type())
#define OSCATS_ALG_CLASS_RATES(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), OSCATS_TYPE_ALG_CLASS_RATES, OscatsAlgClassRates))
#define OSCATS_IS_ALG_CLASS_RATES(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSCATS_TYPE_ALG_CLASS_RATES))
#define OSCATS_ALG_CLASS_RATES_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), OSCATS_TYPE_ALG_CLASS_RATES, OscatsAlgClassRatesClass))
#define OSCATS_IS_ALG_CLASS_RATES_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), OSCATS_TYPE_ALG_CLASS_RATES))
#define OSCATS_ALG_CLASS_RATES_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), OSCATS_TYPE_ALG_CLASS_RATES, OscatsAlgClassRatesClass))

typedef struct _OscatsAlgClassRates OscatsAlgClassRates;
typedef struct _OscatsAlgClassRatesClass OscatsAlgClassRatesClass;

/**
 * OscatsAlgClassRates
 *
 * Statistics algorithm (#OscatsTest::finalize).
 * Tracks the correct classification rates:
 * <itemizedlist>
 *   <listitem><para>Correct classification by attribute: r_k = N^-1 sum_i I_{alpha.hat_ik = alpha_ik}.</para></listitem>
 *   <listitem><para>Overall correct pattern classification: r = N^-1 sum_i I_{alpha.hat_i = alpha_i}.</para></listitem>
 *   <listitem><para>Correct classification by pattern (if #OscatsAlgClassRates:by-pattern): r_alpha = sum_i I_{alpha.hat_i = alpha_i} / sum_i I_{alpha_i = alpha}.</para></listitem>
 *   <listitem><para>Frequency of misclassifications: f_j = N^-1 sum_i I_{j = sum_k abs(alpha.hat_ik - alpha_ik)}.</para></listitem>
 * </itemizedlist>
 *
 * If the test space includes non-binary dimensions, they are silently ignored.
 */
struct _OscatsAlgClassRates {
  OscatsAlgorithm parent_instance;
  /*< private >*/
  GQuark simKey, estKey;
  guint num_attrs, num_examinees, correct_patterns;
  guint *correct_attribute, *misclassify_hist;
  GTree *rate_by_pattern;
};

struct _OscatsAlgClassRatesClass {
  OscatsAlgorithmClass parent_class;
};

GType oscats_alg_class_rates_get_type();

typedef void (*OscatsAlgClassRatesForeachPatternFunc) (GBitArray *pattern, guint num, guint correct, gpointer user_data);

guint oscats_alg_class_rates_num_examinees(const OscatsAlgClassRates *alg_data);
gdouble oscats_alg_class_rates_get_pattern_rate(const OscatsAlgClassRates *alg_data);
gdouble oscats_alg_class_rates_get_attribute_rate(const OscatsAlgClassRates *alg_data, guint i);
gdouble oscats_alg_class_rates_get_misclassify_freq(const OscatsAlgClassRates *alg_data, guint num);
guint oscats_alg_class_rates_num_examinees_by_pattern(const OscatsAlgClassRates *alg_data, const OscatsPoint *attr);
gdouble oscats_alg_class_rates_get_rate_by_pattern(const OscatsAlgClassRates *alg_data, const OscatsPoint *attr);
void oscats_alg_class_rates_foreach_pattern(OscatsAlgClassRates *alg_data, OscatsAlgClassRatesForeachPatternFunc func, gpointer user_data);

G_END_DECLS
#endif
