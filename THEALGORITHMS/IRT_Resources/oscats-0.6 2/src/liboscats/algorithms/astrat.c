/* OSCATS: Open-Source Computerized Adaptive Testing System
 * CAT Algorithm: Perform a Stratification (with optional b blocking)
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

#include <math.h>
#include "algorithms/astrat.h"
#include "models/l1p.h"
#include "models/l2p.h"
#include "models/l3p.h"
#include "marshal.h"

enum {
  PROP_0,
  PROP_EQUAL,
  PROP_N_STRATA,
  PROP_N_BLOCKS,
  PROP_N_ITEMS_EQUAL,
  PROP_N_ITEMS_UNEQUAL,
  PROP_MODEL_KEY,
  PROP_CUR_N,
  PROP_CUR_STRATUM,
};

GHashTable *discr_functions = NULL;
GHashTable *diff_functions = NULL;

static gdouble l1p_b (const OscatsModel *model) { return model->params[0]; }

static gdouble l1p_a (const OscatsModel *model) { return 1; }

static gdouble l2p_a (const OscatsModel *model)
{
  gdouble sum;
  guint i;
  switch (model->Ndims)
  {
    case 1:
      return model->params[1];
    case 2:
      return sqrt(model->params[1]*model->params[1] +
                  model->params[2]*model->params[2]);
    default:
      for (sum=0, i=0; i < model->Ndims; i++)
        sum += model->params[i+1]*model->params[i+1];
      return sqrt(sum);
  }
}

static gdouble l3p_a (const OscatsModel *model)
{
  gdouble sum;
  guint i;
  switch (model->Ndims)
  {
    case 1:
      return model->params[2];
    case 2:
      return sqrt(model->params[2]*model->params[2] +
                  model->params[3]*model->params[3]);
    default:
      for (sum=0, i=0; i < model->Ndims; i++)
        sum += model->params[i+2]*model->params[i+2];
      return sqrt(sum);
  }
}

G_DEFINE_TYPE(OscatsAlgAstrat, oscats_alg_astrat, OSCATS_TYPE_ALGORITHM);

static void oscats_alg_astrat_dispose(GObject *object);
static void oscats_alg_astrat_finalize(GObject *object);
static void oscats_alg_astrat_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec);
static void oscats_alg_astrat_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec);
static void alg_register (OscatsAlgorithm *alg_data, OscatsTest *test);

static void oscats_alg_astrat_class_init (OscatsAlgAstratClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec, *espec;

  oscats_alg_astrat_register_model(OSCATS_TYPE_MODEL_L1P, l1p_a, l1p_b);
  oscats_alg_astrat_register_model(OSCATS_TYPE_MODEL_L2P, l2p_a, l1p_b);
  oscats_alg_astrat_register_model(OSCATS_TYPE_MODEL_L3P, l3p_a, l1p_b);

  gobject_class->dispose = oscats_alg_astrat_dispose;
  gobject_class->finalize = oscats_alg_astrat_finalize;
  gobject_class->set_property = oscats_alg_astrat_set_property;
  gobject_class->get_property = oscats_alg_astrat_get_property;

  OSCATS_ALGORITHM_CLASS(klass)->reg = alg_register;

/**
 * OscatsAlgAstrat:equal:
 *
 * If %TRUE, the number of items selected from each stratum is the same,
 * namely #OscatsAlgAstrat:Nequal, and #OscatsAlgAstrat:Nitems is ignored.
 * If %FALSE, the number of items selected from each stratum must be
 * specified by #OscatsAlgAstrat:Nitems, and both #OscatsAlgAstrat:Nequal
 * and #OscatsAlgAstrat:Nstrata are ignored.
 */
  pspec = g_param_spec_boolean("equal", "Equally sized strata", 
                               "Select the same number of items from each stratum",
                               TRUE,
                               G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                               G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                               G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_EQUAL, pspec);

/**
 * OscatsAlgAstrat:Nstrata:
 *
 * The number of strata.  Note: If #OscatsAlgAstrat:equal is %FALSE, this
 * value may not be set.  Instead, the number of strata is taken from
 * #OscatsAlgAstrat:Nitems.
 */
  pspec = g_param_spec_uint("Nstrata", "Nstrata", 
                            "Number of strata",
                            1, G_MAXUINT, 2,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_STRATA, pspec);

/**
 * OscatsAlgAstrat:Nblocks:
 *
 * The number of b blocks.  If 0, then b blocking is not used.
 */
  pspec = g_param_spec_uint("Nblocks", "Nblocks", 
                            "Number of b blocks",
                            0, G_MAXUINT, 0,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_BLOCKS, pspec);

/**
 * OscatsAlgAstrat:Nequal:
 *
 * The number of items to select from each stratum if #OscatsAlgAstrat:equal
 * is %TRUE.  Otherwise, see #OscatsAlgAstrat:Nitems.
 */
  pspec = g_param_spec_uint("Nequal", "Nequal",
                            "Single number of items to select from each stratum",
                            1, G_MAXUINT, 5,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_ITEMS_EQUAL, pspec);

/**
 * OscatsAlgAstrat:Nitems:
 *
 * An array indicating how many items to select from each stratum. May only
 * be set if #OscatsAlgAstrat:equal is %FALSE.  The number of strata is
 * inferred from the length of the array.
 */
  espec = g_param_spec_uint("num", "number of items",
                            "Number of items to select from given stratum",
                            1, G_MAXUINT, 5,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  pspec = g_param_spec_value_array("Nitems", "Items in each stratum", 
                            "Number of items in each stratum",
                            espec,
                            G_PARAM_READWRITE | 
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_N_ITEMS_UNEQUAL, pspec);

/**
 * OscatsAlgAstrat:modelKey:
 *
 * The key indicating which model to use for stratification.  A %NULL value
 * or empty string indicates the item's default model.
 */
  pspec = g_param_spec_string("modelKey", "model key", 
                            "Which model to use for stratification",
                            NULL,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_MODEL_KEY, pspec);

/**
 * OscatsAlgAstrat:Ncur:
 *
 * The number (0-based) of the current stratum.
 */
  pspec = g_param_spec_uint("Ncur", "Ncur", 
                            "Number of the current stratum",
                            0, G_MAXUINT, 0,
                            G_PARAM_READABLE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_CUR_N, pspec);

/**
 * OscatsAlgAstrat:stratum:
 *
 * The bit array indicating which items are in the current stratum.
 */
  pspec = g_param_spec_object("stratum", "Current stratum", 
                              "Items in the current stratum",
                              G_TYPE_BIT_ARRAY,
                              G_PARAM_READABLE |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_CUR_STRATUM, pspec);

/**
 * OscatsAlgAstrat::stratum:
 * @test: an #OscatsTest
 * @e: an #OscatsExaminee
 * @stratum: the proposed #OscatsItem
 *
 * The #OscatsAlgAstrat::stratum signal is emitted when the examinee moves
 * to a new stratum (including at the beginning of the test).
 */
  klass->stratum = g_signal_new("stratum",
                                OSCATS_TYPE_ALG_ASTRAT, G_SIGNAL_ACTION,
                                0, NULL, NULL,
                                g_cclosure_user_marshal_VOID__OBJECT_OBJECT,
                                G_TYPE_NONE, 2,
                                OSCATS_TYPE_EXAMINEE, G_TYPE_BIT_ARRAY);

}

static void oscats_alg_astrat_init (OscatsAlgAstrat *self)
{
}

static void oscats_alg_astrat_dispose (GObject *object)
{
  OscatsAlgAstrat *self = OSCATS_ALG_ASTRAT(object);
  G_OBJECT_CLASS(oscats_alg_astrat_parent_class)->dispose(object);
  if (self->stratify) g_object_unref(self->stratify);
  self->stratify = NULL;
}

static void oscats_alg_astrat_finalize (GObject *object)
{
  OscatsAlgAstrat *self = OSCATS_ALG_ASTRAT(object);
  if (self->n_items) g_free(self->n_items);
  self->n_items = NULL;
  G_OBJECT_CLASS(oscats_alg_astrat_parent_class)->finalize(object);
}

static void oscats_alg_astrat_set_property(GObject *object,
              guint prop_id, const GValue *value, GParamSpec *pspec)
{
  OscatsAlgAstrat *self = OSCATS_ALG_ASTRAT(object);
  GValueArray *array;
  guint i, num;
  
  switch (prop_id)
  {
    case PROP_EQUAL:			// Construction only
      self->equalSize = g_value_get_boolean(value);
      break;
      
    case PROP_N_STRATA:
      if (self->n_items)
      {
        g_critical("OscatsAlgAstrat:Nstrata may not be set if OscatsAlgAstrat:equal is TRUE.");
        return;
      }
      self->n_strata = g_value_get_uint(value);
      break;
      
    case PROP_N_BLOCKS:
      self->n_blocks = g_value_get_uint(value);
      break;
      
    case PROP_N_ITEMS_EQUAL:
      self->n_equal = g_value_get_uint(value);
      break;
      
    case PROP_N_ITEMS_UNEQUAL:
      g_return_if_fail(self->equalSize == FALSE);
      array = g_value_get_boxed(value);
      g_return_if_fail(array);
      num = array->n_values;
      g_return_if_fail(num > 0);
      if (self->n_strata != num || self->n_items == NULL)
      {
        if (self->n_items) g_free(self->n_items);
        self->n_items = g_new(guint, num);
      }
      self->n_strata = num;
      for (i=0; i < num; i++)
        self->n_items[i] = g_value_get_uint(array->values+i);
      break;
      
    case PROP_MODEL_KEY:
    {
      const gchar *key = g_value_get_string(value);
      if (key == NULL || key[0] == '\0')
        self->modelKey = 0;
      else
        self->modelKey = g_quark_from_string(key);
    }
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_alg_astrat_get_property(GObject *object,
              guint prop_id, GValue *value, GParamSpec *pspec)
{
  OscatsAlgAstrat *self = OSCATS_ALG_ASTRAT(object);
  GValueArray *array;
  GValue v;
  guint i;
  
  switch (prop_id)
  {
    case PROP_EQUAL:
      g_value_set_boolean(value, self->equalSize);
      break;
      
    case PROP_N_STRATA:
      g_value_set_uint(value, self->n_strata);
      break;
      
    case PROP_N_BLOCKS:
      g_value_set_uint(value, self->n_blocks);
      break;
      
    case PROP_N_ITEMS_EQUAL:
      g_value_set_uint(value, self->n_equal);
      break;
      
    case PROP_N_ITEMS_UNEQUAL:
      array = g_value_array_new(self->n_strata);
      g_value_init(&v, G_TYPE_UINT);
      for (i=0; i < self->n_strata; i++)
      {
        if (self->n_items) g_value_set_uint(&v, self->n_items[i]);
        else g_value_set_uint(&v, self->n_equal);
        g_value_array_append(array, &v);
      }
      g_value_take_boxed(value, array);
      break;
      
    case PROP_MODEL_KEY:
      g_value_set_string(value,
        self->modelKey ? g_quark_to_string(self->modelKey) : "");
      break;
    
    case PROP_CUR_N:
      g_value_set_uint(value, self->cur);
      break;
      
    case PROP_CUR_STRATUM:
      g_value_set_object(value,
        oscats_alg_stratify_get_stratum(self->stratify, self->cur));
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static gdouble discr_criterion(const OscatsAdministrand *item, gpointer data)
{
  OscatsAlgAstrat *self = OSCATS_ALG_ASTRAT(data);
  OscatsModel *model = oscats_administrand_get_model(item, self->modelKey);
  OscatsAlgAstratCriterion f =
    g_hash_table_lookup(discr_functions, (gpointer)G_OBJECT_TYPE(model));
  g_return_val_if_fail(f, 0);
  return f(model);
}

static gdouble diff_criterion(const OscatsAdministrand *item, gpointer data)
{
  OscatsAlgAstrat *self = OSCATS_ALG_ASTRAT(data);
  OscatsModel *model = oscats_administrand_get_model(item, self->modelKey);
  OscatsAlgAstratCriterion f =
    g_hash_table_lookup(diff_functions, (gpointer)G_OBJECT_TYPE(model));
  g_return_val_if_fail(f, 0);
  return f(model);
}

static void initialize(OscatsTest *test, OscatsExaminee *e, gpointer alg_data)
{
  OscatsAlgAstrat *self = OSCATS_ALG_ASTRAT(alg_data);
  GBitArray *stratum;
  self->flag = FALSE;
  self->cur = 0;
  if (self->equalSize) self->rem = self->n_equal;
  else
  {
    g_return_if_fail(self->n_items);
    self->rem = self->n_items[0];
  }
  oscats_alg_stratify_reset(self->stratify);
  stratum = oscats_alg_stratify_next(self->stratify);	// First stratum
  oscats_test_set_hint(test, stratum);
  g_signal_emit(self, OSCATS_ALG_ASTRAT_GET_CLASS(self)->stratum, 0, e,
                stratum);
}

static void administered (OscatsTest *test, OscatsExaminee *e,
                          OscatsItem *item, guint resp, gpointer alg_data)
{
  OscatsAlgAstrat *self = OSCATS_ALG_ASTRAT(alg_data);
  GBitArray *stratum;
  
  g_return_if_fail(e->items);
  if (e->items->len > 0 && e->items->pdata[e->items->len-1] == item)
  {
    g_return_if_fail(self->flag == FALSE);
    if (--self->rem == 0) 		// Move to next stratum
    {
      self->cur++;
      stratum = oscats_alg_stratify_next(self->stratify);
      if (stratum)
      {
        if (self->equalSize) self->rem = self->n_equal;
        else self->rem = self->n_items[self->cur];
        oscats_test_set_hint(test, stratum);
        g_signal_emit(self, OSCATS_ALG_ASTRAT_GET_CLASS(self)->stratum, 0,
                      e, stratum);
      } else self->flag = TRUE;		// No more strata
    }
  }
  // Otherwise, item was not recorded, so do nothing.
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
  OscatsAlgAstrat *self = OSCATS_ALG_ASTRAT(alg_data);

  self->stratify = g_object_new(OSCATS_TYPE_ALG_STRATIFY,
                                "itembank", test->itembank, NULL);
  oscats_alg_astrat_restratify(self);

  g_signal_connect_data(test, "initialize", G_CALLBACK(initialize),
                        alg_data, oscats_algorithm_closure_finalize, 0);
  g_object_ref(alg_data);
  g_signal_connect_data(test, "administered", G_CALLBACK(administered),
                        alg_data, oscats_algorithm_closure_finalize, 0);
}
                   
/**
 * oscats_alg_astrat_register_model:
 * @model: the #GType of the #OscatsModel
 * @a: function that returns the discrimination
 * @b: function that returns the difficulty
 *
 * Registers functions to identify the discrimination and difficulty
 * parameters for @model.  Note that this happens automatically for
 * #OscatsModelL1p, #OscatsModelL2p, and #OscatsModelL3p.
 */
void oscats_alg_astrat_register_model(GType model,
                                      OscatsAlgAstratCriterion a,
                                      OscatsAlgAstratCriterion b)
{
  g_return_if_fail(g_type_is_a(model, OSCATS_TYPE_MODEL));
  g_return_if_fail(a && b);
  if (discr_functions == NULL)
    discr_functions = g_hash_table_new(g_direct_hash, g_direct_equal);
  if (diff_functions == NULL)
    diff_functions = g_hash_table_new(g_direct_hash, g_direct_equal);
  g_hash_table_insert(discr_functions, (gpointer)model, a);
  g_hash_table_insert(diff_functions, (gpointer)model, b);
}

/**
 * oscats_alg_astrat_restratify:
 * @alg_data: the #OscatsAlgAstrat agorithm data
 *
 * Performs the stratification again.  This function only needs to be called
 * in complex simulations for which the stratification parameters (such as
 * the number of strata, number of items per statum, the model key, etc.)
 * change after the test has been constructed, which is
 * <emphasis>not</emphasis> usual.
 *
 * Note: This will reset the internal iterator to the first stratum, but
 * does not emit #OscatsAlgAstrat::stratum.
 */
void oscats_alg_astrat_restratify(OscatsAlgAstrat *alg_data)
{
  g_return_if_fail(OSCATS_IS_ALG_ASTRAT(alg_data));
  g_return_if_fail(OSCATS_IS_ALG_STRATIFY(alg_data->stratify));

  oscats_alg_stratify_stratify(alg_data->stratify,
                               alg_data->n_strata, discr_criterion, alg_data,
                               alg_data->n_blocks, diff_criterion, alg_data);

  alg_data->flag = FALSE;
  alg_data->cur = 0;
  if (alg_data->equalSize) alg_data->rem = alg_data->n_equal;
  else
  {
    g_return_if_fail(alg_data->n_items);
    alg_data->rem = alg_data->n_items[0];
  }
}
