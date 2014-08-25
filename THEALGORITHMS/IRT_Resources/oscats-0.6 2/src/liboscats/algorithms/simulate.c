/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Simulate Item Administration
 * Copyright 2011 Michael Culbertson <culbert1@illinois.edu>
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
#include "algorithms/simulate.h"

G_DEFINE_TYPE(OscatsAlgSimulate, oscats_alg_simulate, OSCATS_TYPE_ALGORITHM);

enum
{
  PROP_0,
  PROP_AUTO_RECORD,
  PROP_MODEL_KEY,
  PROP_THETA_KEY,
};

static void oscats_alg_set_property(GObject *object, guint prop_id,
                                    const GValue *value, GParamSpec *pspec);
static void oscats_alg_get_property(GObject *object, guint prop_id,
                                    GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_simulate_class_init (OscatsAlgSimulateClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;
  
  gobject_class->set_property = oscats_alg_set_property;
  gobject_class->get_property = oscats_alg_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgSimulate:auto-record:
 *
 * Whether to record all simulated responses.
 */
  pspec = g_param_spec_boolean("auto-record", "Auto-record", 
                               "Record all simulated responses",
                               TRUE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_AUTO_RECORD, pspec);

/**
 * OscatsAlgSimulate:modelKey:
 *
 * The key indicating which model to use for simulation.  A %NULL value or
 * empty string indicates the item's default model.
 */
  pspec = g_param_spec_string("modelKey", "model key", 
                            "Which model to use for simulation",
                            NULL,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_MODEL_KEY, pspec);

/**
 * OscatsAlgSimulate:thetaKey:
 *
 * The key indicating which latent variable to use for simulation.  A %NULL
 * value or empty string indicates the examinee's default simulation theta.
 */
  pspec = g_param_spec_string("thetaKey", "ability key", 
                            "Which latent variable to use for simulation",
                            NULL,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_THETA_KEY, pspec);

}

static void oscats_alg_simulate_init (OscatsAlgSimulate *self)
{
}

static void oscats_alg_set_property(GObject *object, guint prop_id,
                                    const GValue *value, GParamSpec *pspec)
{
  OscatsAlgSimulate *self = OSCATS_ALG_SIMULATE(object);
  switch (prop_id)
  {
    case PROP_AUTO_RECORD:
      self->record = g_value_get_boolean(value);
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

static void oscats_alg_get_property(GObject *object, guint prop_id,
                                    GValue *value, GParamSpec *pspec)
{
  OscatsAlgSimulate *self = OSCATS_ALG_SIMULATE(object);
  switch (prop_id)
  {
    case PROP_AUTO_RECORD:
      g_value_set_boolean(value, self->record);
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

static guint administer (OscatsTest *test, OscatsExaminee *e,
                         OscatsItem *item, gpointer alg_data)
{
  OscatsAlgSimulate *self = OSCATS_ALG_SIMULATE(alg_data);
  OscatsModel *model = oscats_administrand_get_model(OSCATS_ADMINISTRAND(item), self->modelKey);
  OscatsPoint *theta = ( self->thetaKey ?
                           oscats_examinee_get_theta(e, self->thetaKey) :
                           oscats_examinee_get_sim_theta(e) );
  guint resp, max = oscats_model_get_max(model);
  gdouble p, rnd = oscats_rnd_uniform();
  for (resp=0; resp <= max; resp++)
    if (rnd < (p=oscats_model_P(model, resp, theta, e->covariates)))
    {
      if (OSCATS_ALG_SIMULATE(alg_data)->record)
        oscats_examinee_add_item(e, item, resp);
      return resp;
    }
    else
      rnd -= p;
  g_warn_if_reached();		// Model probabilities don't sum to 1.
  return 0;
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
  g_signal_connect_data(test, "administer", G_CALLBACK(administer),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}
                   
