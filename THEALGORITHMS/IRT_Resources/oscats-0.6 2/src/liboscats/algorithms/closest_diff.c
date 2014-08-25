/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Select Item with Closest Difficulty
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

#include "random.h"
#include "algorithms/closest_diff.h"

enum {
  PROP_0,
  PROP_NUM,
  PROP_MODEL_KEY,
  PROP_THETA_KEY,
};

G_DEFINE_TYPE(OscatsAlgClosestDiff, oscats_alg_closest_diff, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_closest_diff_dispose(GObject *object);
static void oscats_alg_closest_diff_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_closest_diff_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_closest_diff_class_init (OscatsAlgClosestDiffClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_alg_closest_diff_dispose;
  gobject_class->set_property = oscats_alg_closest_diff_set_property;
  gobject_class->get_property = oscats_alg_closest_diff_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgClosestDiff:num:
 *
 * Number of items from which to choose.  If one, then the exact closest
 * item is selected.  If greater than one, then a random item is chosen
 * from among the #OscatsAlgClosestDiff:num closest items.
 */
  pspec = g_param_spec_uint("num", "", 
                            "Number of items from which to choose",
                            1, G_MAXUINT, 1,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM, pspec);

/**
 * OscatsAlgClosestDiff:modelKey:
 *
 * The key indicating which model to use for selection.  A %NULL value or
 * empty string indicates the item's default model.
 */
  pspec = g_param_spec_string("modelKey", "model key", 
                            "Which model to use for selection",
                            NULL,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_MODEL_KEY, pspec);

/**
 * OscatsAlgClosestDiff:thetaKey:
 *
 * The key indicating which latent variable to use for selection.  A %NULL
 * value or empty string indicates the examinee's default estimation theta.
 */
  pspec = g_param_spec_string("thetaKey", "ability key", 
                            "Which latent variable to use for selection",
                            NULL,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_THETA_KEY, pspec);

}

static void oscats_alg_closest_diff_init (OscatsAlgClosestDiff *self)
{
}

static void oscats_alg_closest_diff_dispose (GObject *object)
{
  OscatsAlgClosestDiff *self = OSCATS_ALG_CLOSEST_DIFF(object);
  G_OBJECT_CLASS(oscats_alg_closest_diff_parent_class)->dispose(object);
  if (self->chooser) g_object_unref(self->chooser);
  self->chooser = NULL;
}

static void oscats_alg_closest_diff_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgClosestDiff *self = OSCATS_ALG_CLOSEST_DIFF(object);
  switch (prop_id)
  {
    case PROP_NUM:			// construction only
      self->chooser = g_object_new(OSCATS_TYPE_ALG_CHOOSER,
                                   "num", g_value_get_uint(value), NULL);
      break;
    
    case PROP_MODEL_KEY:
    {
      const gchar *key = g_value_get_string(value);
      if (key == NULL || key[0] == '\0') self->modelKey = 0;
      else self->modelKey = g_quark_from_string(key);
    }
      break;
    
    case PROP_THETA_KEY:
    {
      const gchar *key = g_value_get_string(value);
      if (key == NULL || key[0] == '\0') self->thetaKey = 0;
      else self->thetaKey = g_quark_from_string(key);
    }
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_closest_diff_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgClosestDiff *self = OSCATS_ALG_CLOSEST_DIFF(object);
  switch (prop_id)
  {
    case PROP_NUM:
      g_value_set_uint(value, self->chooser->num);
      break;
    
    case PROP_MODEL_KEY:
      g_value_set_string(value, self->modelKey ?
                        g_quark_to_string(self->modelKey) : "");
      break;
    
    case PROP_THETA_KEY:
      g_value_set_string(value, self->thetaKey ?
                         g_quark_to_string(self->thetaKey) : "");
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static gdouble criterion(const OscatsItem *item, OscatsExaminee *e,
                         gpointer data)
{
  OscatsAlgClosestDiff *self = (OscatsAlgClosestDiff*)data;
  OscatsModel *model = oscats_administrand_get_model(OSCATS_ADMINISTRAND(item), self->modelKey);
  OscatsPoint *theta = ( self->thetaKey ?
                           oscats_examinee_get_theta(e, self->thetaKey) :
                           oscats_examinee_get_est_theta(e) );
  return oscats_model_distance(model, theta, e->covariates);
}

static gint select (OscatsTest *test, OscatsExaminee *e,
                    GBitArray *eligible, gpointer alg_data)
{
  return oscats_alg_chooser_choose(OSCATS_ALG_CLOSEST_DIFF(alg_data)->chooser,
                                   e, eligible, alg_data);
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
  OscatsAlgClosestDiff *self = OSCATS_ALG_CLOSEST_DIFF(alg_data);
  self->chooser->bank = g_object_ref(test->itembank);
  self->chooser->criterion = (OscatsAlgChooserCriterion)criterion;
  g_signal_connect_data(test, "select", G_CALLBACK(select),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}
                   
