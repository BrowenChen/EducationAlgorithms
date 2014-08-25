/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Select Random Item
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

#include "random.h"
#include "algorithm.h"
#include "algorithms/pick_rand.h"

G_DEFINE_TYPE(OscatsAlgPickRand, oscats_alg_pick_rand, OSCATS_TYPE_ALGORITHM);

static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_pick_rand_class_init (OscatsAlgPickRandClass *klass)
{
//  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;
}

static void oscats_alg_pick_rand_init (OscatsAlgPickRand *self)
{
}

static gint select (OscatsTest *test, OscatsExaminee *e,
                    GBitArray *eligible, gpointer alg_data)
{
  guint i, item = 0;
  g_return_val_if_fail(eligible->num_set > 0, -1);
  i = oscats_rnd_uniform_int_range(1, eligible->num_set);
  g_bit_array_iter_reset(eligible);
  for (; i; i--) item = g_bit_array_iter_next(eligible);
  return item;
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
  g_signal_connect_data(test, "select", G_CALLBACK(select),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}
                   
