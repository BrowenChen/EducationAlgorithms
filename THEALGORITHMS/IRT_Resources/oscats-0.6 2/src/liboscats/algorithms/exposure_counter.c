/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Track item exposure rate
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

#include "algorithm.h"
#include "algorithms/exposure_counter.h"

G_DEFINE_TYPE(OscatsAlgExposureCounter, oscats_alg_exposure_counter, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_exposure_counter_constructed (GObject *object);
static void oscats_alg_exposure_counter_dispose (GObject *object);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_exposure_counter_class_init (OscatsAlgExposureCounterClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  gobject_class->constructed = oscats_alg_exposure_counter_constructed;
  gobject_class->dispose = oscats_alg_exposure_counter_dispose;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;
}

static void oscats_alg_exposure_counter_init (OscatsAlgExposureCounter *self)
{
}

static void oscats_alg_exposure_counter_constructed (GObject *object)
{
  OscatsAlgExposureCounter *self = OSCATS_ALG_EXPOSURE_COUNTER(object);
//  G_OBJECT_CLASS(oscats_alg_exposure_counter_parent_class)->constructed(object);
  self->counts = g_hash_table_new(g_direct_hash, g_direct_equal);
}

static void oscats_alg_exposure_counter_dispose (GObject *object)
{
  OscatsAlgExposureCounter *self = OSCATS_ALG_EXPOSURE_COUNTER(object);
  G_OBJECT_CLASS(oscats_alg_exposure_counter_parent_class)->dispose(object);
  if (self->counts) g_hash_table_unref(self->counts);
  self->counts = NULL;
}

static void initialize(OscatsTest *test, OscatsExaminee *e, gpointer alg_data)
{
  OscatsAlgExposureCounter *self = OSCATS_ALG_EXPOSURE_COUNTER(alg_data);
  self->num_examinees++;
}

static void administered (OscatsTest *test, OscatsExaminee *e,
                          OscatsItem *item, guint resp, gpointer alg_data)
{
  guint count;
  OscatsAlgExposureCounter *self = OSCATS_ALG_EXPOSURE_COUNTER(alg_data);
  count = GPOINTER_TO_UINT(g_hash_table_lookup(self->counts, item));
  g_hash_table_insert(self->counts, item, GUINT_TO_POINTER(count+1));
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
  g_signal_connect_data(test, "initialize", G_CALLBACK(initialize),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_signal_connect_data(test, "administered", G_CALLBACK(administered),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_object_ref(alg_data);
}

/**
 * oscats_alg_exposure_counter_num_examinees:
 * @alg_data: the #OscatsAlgExposureCounter data object
 *
 * Returns: the number of examinees tested
 */
guint oscats_alg_exposure_counter_num_examinees(const OscatsAlgExposureCounter *alg_data)
{
  g_return_val_if_fail(OSCATS_IS_ALG_EXPOSURE_COUNTER(alg_data), 0);
  return alg_data->num_examinees;
}

/**
 * oscats_alg_exposure_counter_get_rate:
 * @alg_data: the #OscatsAlgExposureCounter data object
 * @item: the #OscatsItem for which to query the rate
 *
 * Returns: the exposure rate (or 0 if @item is unknown)
 */
gdouble oscats_alg_exposure_counter_get_rate(const OscatsAlgExposureCounter *alg_data,
                                             const OscatsItem *item)
{
  g_return_val_if_fail(OSCATS_IS_ALG_EXPOSURE_COUNTER(alg_data), 0);
  return GPOINTER_TO_UINT(g_hash_table_lookup(alg_data->counts, item)) /
           (gdouble)(alg_data->num_examinees);
}
