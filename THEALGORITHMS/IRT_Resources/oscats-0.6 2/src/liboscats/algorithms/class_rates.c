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

#include "algorithm.h"
#include "algorithms/class_rates.h"

enum
{
  PROP_0,
  PROP_BY_PATTERN,
  PROP_SIM_KEY,
  PROP_EST_KEY,
};

G_DEFINE_TYPE(OscatsAlgClassRates, oscats_alg_class_rates, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_class_rates_dispose (GObject *object);
static void oscats_alg_class_rates_finalize (GObject *object);
static void oscats_alg_class_rates_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_class_rates_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_class_rates_class_init (OscatsAlgClassRatesClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_alg_class_rates_dispose;
  gobject_class->finalize = oscats_alg_class_rates_finalize;
  gobject_class->set_property = oscats_alg_class_rates_set_property;
  gobject_class->get_property = oscats_alg_class_rates_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgClassRates:by-pattern:
 *
 * Track misclassification rate for each classification pattern observed.
 */
  pspec = g_param_spec_boolean("by-pattern", "Rates by pattern", 
                               "Track misclassification rates by pattern",
                               FALSE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_BY_PATTERN, pspec);

/**
 * OscatsAlgClassRates:simKey:
 *
 * Which latent point to consider the "correct" or "true" value.  A %NULL
 * value or empty string indicate the examinee's simulation default.
 */
  pspec = g_param_spec_string("simKey", "Reference Key", 
                               "Key for 'true' latent variable",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_SIM_KEY, pspec);

/**
 * OscatsAlgClassRates:estKey:
 *
 * Which latent point holds the estimated classification.  A %NULL value or
 * empty string indicates the examinee's estimation default.
 */
  pspec = g_param_spec_string("estKey", "Estimation Key", 
                               "Key for estimated latent variable",
                               NULL,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_EST_KEY, pspec);

}

static void oscats_alg_class_rates_init (OscatsAlgClassRates *self)
{
}

static void oscats_alg_class_rates_dispose (GObject *object)
{
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(object);
  G_OBJECT_CLASS(oscats_alg_class_rates_parent_class)->dispose(object);
  if (self->rate_by_pattern) g_tree_unref(self->rate_by_pattern);
  self->rate_by_pattern = NULL;
}

static void oscats_alg_class_rates_finalize (GObject *object)
{
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(object);
  g_free(self->correct_attribute);
  g_free(self->misclassify_hist);
  G_OBJECT_CLASS(oscats_alg_class_rates_parent_class)->finalize(object);
}

static void oscats_alg_class_rates_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(object);
  switch (prop_id)
  {
    case PROP_BY_PATTERN:			// construction only
      if(g_value_get_boolean(value))
        self->rate_by_pattern =
          g_tree_new_full((GCompareDataFunc)g_bit_array_serial_compare,
                          NULL, g_object_unref, g_free);
      break;
    
    case PROP_SIM_KEY:			// construction only
    {
      const gchar *key = g_value_get_string(value);
      if (key == NULL || key[0] == '\0') self->simKey = 0;
      else self->simKey = g_quark_from_string(key);
    }
      break;
    
    case PROP_EST_KEY:			// construction only
    {
      const gchar *key = g_value_get_string(value);
      if (key == NULL || key[0] == '\0') self->estKey = 0;
      else self->estKey = g_quark_from_string(key);
    }
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_class_rates_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(object);
  switch (prop_id)
  {
    case PROP_BY_PATTERN:
      g_value_set_boolean(value, self->rate_by_pattern != NULL);
      break;
    
    case PROP_SIM_KEY:
      g_value_set_string(value,
                         self->simKey ? g_quark_to_string(self->simKey) : "");
      break;
    
    case PROP_EST_KEY:
      g_value_set_string(value,
                         self->estKey ? g_quark_to_string(self->estKey) : "");
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void finalize(OscatsTest *test, OscatsExaminee *e, gpointer alg_data)
{
  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(alg_data);
  OscatsPoint *alpha_true, *alpha_hat;
  guint i, num = 0;	// num wrong

  alpha_true = (self->simKey ? oscats_examinee_get_theta(e, self->simKey)
                             : oscats_examinee_get_sim_theta(e) );
  alpha_hat  = (self->estKey ? oscats_examinee_get_theta(e, self->estKey)
                             : oscats_examinee_get_est_theta(e) );
  g_return_if_fail(oscats_point_space_compatible(alpha_true, alpha_hat));

  if (G_UNLIKELY(self->correct_attribute == NULL))
  {
    self->num_attrs = alpha_true->space->num_bin;
    self->correct_attribute = g_new0(guint, self->num_attrs);
    self->misclassify_hist = g_new0(guint, self->num_attrs+1);
  }
  else g_return_if_fail(self->num_attrs == alpha_true->space->num_bin);

  self->num_examinees++;
  for (i=0; i < self->num_attrs; i++)
    if (g_bit_array_get_bit(alpha_true->bin, i) !=
        g_bit_array_get_bit(alpha_hat->bin, i) )
      num++;
    else
      self->correct_attribute[i]++;
  if (num == 0) self->correct_patterns++;
  self->misclassify_hist[num]++;
  
  if (self->rate_by_pattern)
  {
    guint *data = g_tree_lookup(self->rate_by_pattern, alpha_true->bin);
    if (!data)
    {
      GBitArray *attr = g_bit_array_new(self->num_attrs);
      g_bit_array_copy(attr, alpha_true->bin);
      data = g_new0(guint, 2);
      g_tree_insert(self->rate_by_pattern, attr, data);
    }
    data[0]++;			// Number of examinees with this pattern
    if (num == 0) data[1]++;	// Number correctly classified
  }
}

/*
 * Note that unless someone does something naughty, alg_data will be of the
 * appropriate type, and test will be an OscatsTest.  The signal connections
 * should include oscats_algorithm_closure_finalize as the destruction
 * callback.  The first connection should take alg_data's reference.  Any
 * subsequent connections should be accompanied by g_object_ref(alg_data).
 */
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test)
{
//  OscatsAlgClassRates *self = OSCATS_ALG_CLASS_RATES(alg_data);

  g_signal_connect_data(test, "finalize", G_CALLBACK(finalize),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}

/**
 * oscats_alg_class_rates_num_examinees:
 * @alg_data: the #OscatsAlgClassRates data object
 *
 * Returns: the number of examinees tested
 */
guint oscats_alg_class_rates_num_examinees(const OscatsAlgClassRates *alg_data)
{
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data), 0);
  return alg_data->num_examinees;
}

/**
 * oscats_alg_class_rates_get_pattern_rate:
 * @alg_data: the #OscatsAlgClassRates data object
 *
 * The overall pattern classification success rate is:
 * r = N^-1 sum_i I_{alpha.hat_i = alpha_i}.
 *
 * Returns: the overall pattern classification success rate
 */
gdouble oscats_alg_class_rates_get_pattern_rate(const OscatsAlgClassRates *alg_data)
{
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data), 0);
  return (gdouble)(alg_data->correct_patterns) /
         (gdouble)(alg_data->num_examinees);
}

/**
 * oscats_alg_class_rates_get_attribute_rate:
 * @alg_data: the #OscatsAlgClassRates data object
 * @i: the index of the attribute for which to query the rate
 *
 * The attribute classification success rate is:
 * r_k = N^-1 sum_i I_{alpha.hat_ik = alpha_ik}.
 *
 * Returns: the classification success rate for attribute @i
 */
gdouble oscats_alg_class_rates_get_attribute_rate(
                          const OscatsAlgClassRates *alg_data, guint i)
{
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data), 0);
  g_return_val_if_fail(i < alg_data->num_attrs, 0);
  return (gdouble)(alg_data->correct_attribute[i]) /
         (gdouble)(alg_data->num_examinees);
}

/**
 * oscats_alg_class_rates_get_misclassify_freq:
 * @alg_data: the #OscatsAlgClassRates data object
 * @num: the number of misclassified attributes
 *
 * The frequency of having @num misclassified attributes is:
 * f_j = N^-1 sum_i I_{j = sum_k abs(alpha.hat_ik - alpha_ik)}.
 *
 * Returns: the rate of misclassifying @num attributes
 */
gdouble oscats_alg_class_rates_get_misclassify_freq(
                          const OscatsAlgClassRates *alg_data, guint num)
{
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data), 0);
  g_return_val_if_fail(num <= alg_data->num_attrs, 0);
  return (gdouble)(alg_data->misclassify_hist[num]) /
         (gdouble)(alg_data->num_examinees);
}

/**
 * oscats_alg_class_rates_num_examinees_by_pattern:
 * @alg_data: the #OscatsAlgClassRates data object
 * @attr: the attribute pattern to query
 *
 * Rates for individual patterns are tabulated only if
 * #OscatsAlgClassRates:by-pattern is set to %TRUE at registration.
 *
 * Returns: the number of examinees with true attribute pattern @attr.
 */
guint oscats_alg_class_rates_num_examinees_by_pattern(
          const OscatsAlgClassRates *alg_data, const OscatsPoint *attr)
{
  guint *data;
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data) &&
                       OSCATS_IS_POINT(attr), 0);
  g_return_val_if_fail(alg_data->rate_by_pattern != NULL, 0);
  data = g_tree_lookup(alg_data->rate_by_pattern, attr->bin);
  if (!data) return 0;
  return data[0];
}

/**
 * oscats_alg_class_rates_get_rate_by_pattern:
 * @alg_data: the #OscatsAlgClassRates data object
 * @attr: the attribute pattern to query
 *
 * The correct classification rate for a given pattern is:
 * r_alpha = sum_i I_{alpha.hat_i = alpha_i} / sum_i I_{alpha_i = alpha}.
 *
 * Rates for individual patterns are tabulated only if
 * #OscatsAlgClassRates:by-pattern is set to %TRUE at registration.
 *
 * Returns: the number of examinees with true attribute pattern @attr.
 */
gdouble oscats_alg_class_rates_get_rate_by_pattern(
          const OscatsAlgClassRates *alg_data, const OscatsPoint *attr)
{
  guint *data;
  g_return_val_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data) &&
                       OSCATS_IS_POINT(attr), 0);
  g_return_val_if_fail(alg_data->rate_by_pattern != NULL, 0);
  data = g_tree_lookup(alg_data->rate_by_pattern, attr->bin);
  if (!data) return 0;
  return (gdouble)(data[1])/(gdouble)(data[0]);
}

static gboolean foreach_func(gpointer key, gpointer value, gpointer closure)
{
  OscatsAlgClassRatesForeachPatternFunc func = ((gpointer*)closure)[0];
  gpointer user_data = ((gpointer*)closure)[1];
  guint *data = value;
  func(G_BIT_ARRAY(key), data[0], data[1], user_data);
  return FALSE;
}

/**
 * oscats_alg_class_rates_foreach_pattern:
 * @alg_data: the #OscatsAlgClassRates data object
 * @func: a function to call for each pattern
 * @user_data: user data to pass to the function
 *
 * If #OscatsAlgClassRates:by-pattern is %TRUE for @alg_data, calls @func on
 * each pattern observed in #GBitArray serialized order.  The @func takes
 * four parameters: a pointer to the #GBitArray pattern, the number of times
 * the pattern was observed, the number of times the pattern was correctly
 * classified, and @user_data.
 */
void oscats_alg_class_rates_foreach_pattern(OscatsAlgClassRates *alg_data,
         OscatsAlgClassRatesForeachPatternFunc func, gpointer user_data)
{
  gpointer closure[2] = { func, user_data };
  g_return_if_fail(OSCATS_IS_ALG_CLASS_RATES(alg_data));
  g_return_if_fail(func != NULL);
  g_return_if_fail(alg_data->rate_by_pattern != NULL);
  g_tree_foreach(alg_data->rate_by_pattern, foreach_func, closure);
}
