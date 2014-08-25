/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Fixed Length Stopping Criterion
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
#include "algorithms/fixed_length.h"

enum {
  PROP_0,
  PROP_LEN,
};

G_DEFINE_TYPE(OscatsAlgFixedLength, oscats_alg_fixed_length, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_fixed_length_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_fixed_length_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_fixed_length_class_init (OscatsAlgFixedLengthClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->set_property = oscats_alg_fixed_length_set_property;
  gobject_class->get_property = oscats_alg_fixed_length_get_property;
 
  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgFixedLength:len:
 *
 * Length of test.
 */
  pspec = g_param_spec_uint("len", "Length", 
                            "Length of test",
                            1, G_MAXUINT, 20,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_LEN, pspec);

}

static void oscats_alg_fixed_length_init (OscatsAlgFixedLength *self)
{
}

static void oscats_alg_fixed_length_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgFixedLength *self = OSCATS_ALG_FIXED_LENGTH(object);
  switch (prop_id)
  {
    case PROP_LEN:			// construction only
      self->len = g_value_get_uint(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_fixed_length_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgFixedLength *self = OSCATS_ALG_FIXED_LENGTH(object);
  switch (prop_id)
  {
    case PROP_LEN:
      g_value_set_uint(value, self->len);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static gboolean stopcrit (OscatsTest *test, OscatsExaminee *e,
                          gpointer alg_data)
{
  OscatsAlgFixedLength *self = OSCATS_ALG_FIXED_LENGTH(alg_data);
  g_return_val_if_fail(e->items, TRUE);
  return (e->items->len >= self->len);
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
  g_signal_connect_data(test, "stopcrit", G_CALLBACK(stopcrit),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}

