/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Abstract CAT Algorithm Class
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
 * SECTION:algorithm
 * @title:OscatsAlgorithm
 * @short_description: Abstract CAT Algorithm Class
 */

#include "algorithm.h"

G_DEFINE_TYPE(OscatsAlgorithm, oscats_algorithm, G_TYPE_INITIALLY_UNOWNED);

static void null_register (OscatsAlgorithm *alg_data, OscatsTest *test)
{
  g_critical("Abstract CAT Algorithm should be overloaded.");
}
                   
static void oscats_algorithm_class_init (OscatsAlgorithmClass *klass)
{
//  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

  klass->reg = null_register;
}

static void oscats_algorithm_init (OscatsAlgorithm *self)
{
}

/**
 * oscats_algorithm_register:
 * @alg_data: (transfer full): the #OscatsAlgorithm descendant to register
 * @test: the #OscatsTest on which to register the algorithm
 *
 * Registers the algorithm @alg_data for use in @test.  This will sink the
 * floating reference to @alg_data.  (Callers who want to keep a pointer to
 * @alg_data should call g_object_ref_sink() themselves.)  In general, an
 * algorithm object is registered to only one test.
 *
 * Returns: (transfer none): @alg_data
 */
OscatsAlgorithm * oscats_algorithm_register(OscatsAlgorithm *alg_data, OscatsTest *test)
{
  OscatsAlgorithmClass *klass = OSCATS_ALGORITHM_GET_CLASS(alg_data);
  g_return_val_if_fail(OSCATS_IS_ALGORITHM(alg_data) && OSCATS_IS_TEST(test), NULL);
  g_object_ref_sink(alg_data);
  klass->reg(alg_data, test);
  return alg_data;
}

/**
 * oscats_algorithm_closure_finalize:
 * @alg_data: data to free
 * @closure: signal handler
 *
 * Calls g_object_unref(alg_data).  Should not be invoked directly, but
 * is supplied as the destroy_notifier at signal connection.
 */
void oscats_algorithm_closure_finalize(gpointer alg_data, GClosure *closure)
{ g_object_unref(alg_data); }
