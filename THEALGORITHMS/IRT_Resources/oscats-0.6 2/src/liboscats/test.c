/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Test Administration Class
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
 * SECTION:test
 * @title:OscatsTest
 * @short_description: Computerized Adaptive Test Administration
 */

#include "test.h"
#include "marshal.h"

G_DEFINE_TYPE(OscatsTest, oscats_test, G_TYPE_OBJECT);

enum
{
  PROP_0,
  PROP_ID,
  PROP_BANK,
  PROP_LENGTH_HINT,
  PROP_ITERMAX_SELECT,
  PROP_ITERMAX_ITEMS,
};

static void oscats_test_dispose (GObject *object);
static void oscats_test_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec);
static void oscats_test_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec);

static gboolean accumulate_boolean_or(GSignalInvocationHint *hint,
                                      GValue *return_acc,
                                      const GValue *handler_return,
                                      gpointer data)
{
  g_value_set_boolean(return_acc,
                        g_value_get_boolean(return_acc) ||
                        g_value_get_boolean(handler_return));
  return TRUE;
}

static void oscats_test_class_init (OscatsTestClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GParamSpec *pspec;

  gobject_class->dispose = oscats_test_dispose;
  gobject_class->set_property = oscats_test_set_property;
  gobject_class->get_property = oscats_test_get_property;
  
/**
 * OscatsTest:id:
 *
 * A string identifier for the test.
 */
  pspec = g_param_spec_string("id", "ID", 
                            "String identifier for the test",
                            NULL,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_ID, pspec);

/**
 * OscatsTest:itembank:
 *
 * The item bank used for the test.
 */
  pspec = g_param_spec_object("itembank", "Item Bank", 
                              "Item Bank used for the test",
                              OSCATS_TYPE_ITEM_BANK,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                              G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                              G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_BANK, pspec);

/**
 * OscatsTest:length-hint:
 *
 * Guess for how long the test will be.
 */
  pspec = g_param_spec_uint("length-hint", "Length Hint", 
                            "Guess for how long the test will be",
                            0, G_MAXUINT, 0,
                            G_PARAM_READWRITE |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_LENGTH_HINT, pspec);

/**
 * OscatsTest:itermax-select:
 *
 * The maximum number of iterations in the item-selection loop.
 * If this value is reached without selecting an item, a warning is logged,
 * the #OscatsTest::finalize signal is emitted, and the test is aborted.
 */
  pspec = g_param_spec_uint("itermax-select", "Select Iteration Max",
                            "Maximum number of iterations for item selection",
                            1, G_MAXUINT, 50,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_ITERMAX_SELECT, pspec);

/**
 * OscatsTest:itermax-items:
 *
 * The maximum number of items.  If this value is reached (or the item bank
 * is exhausted) before the stoping criterion is triggered, a warning is
 * logged, the #OscatsTest::finalize signal is emitted, and the test is
 * concluded.
 */
  pspec = g_param_spec_uint("itermax-items", "Test Length Max",
                            "Maximum number of items in test",
                            1, G_MAXUINT, 200,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_ITERMAX_ITEMS, pspec);

/**
 * OscatsTest::initialize:
 * @test: an #OscatsTest
 * @e: an #OscatsExaminee
 *
 * Test initializiation.  All algorithms that need to initialize their
 * internal data structures should connect to the #OscatsTest::initialize
 * signal.  If the examinee's initial ability/class estimate is not set
 * before oscats_test_administer() is called, it should be set by a handler
 * attached to #OscatsTest::initialize.
 */
  klass->initialize = g_signal_new("initialize",
                                   OSCATS_TYPE_TEST, G_SIGNAL_ACTION, 0,
                                   NULL, NULL,
                                   g_cclosure_marshal_VOID__OBJECT,
                                   G_TYPE_NONE, 1,
                                   OSCATS_TYPE_EXAMINEE);

/**
 * OscatsTest::filter:
 * @test: an #OscatsTest
 * @e: an #OscatsExaminee
 * @eligible: a #GBitArray indicating eligible items in the itembank
 *
 * Item filtration.  The list of @eligible items is initialized to the
 * test's eligibility hint (default: all items), minus items examinee @e has
 * already been administered.  Algorithms connecting to #OscatsTest::filter
 * should clear bits in @eligible for undesired items.  If an algorithm sets
 * bits in @eligible, it is the algorithm's responsibility to ensure that
 * examinee @e has not already seen the given item.
 */
  klass->filter = g_signal_new("filter",
                               OSCATS_TYPE_TEST, G_SIGNAL_ACTION, 0,
                               NULL, NULL,
                               g_cclosure_user_marshal_VOID__OBJECT_OBJECT,
                               G_TYPE_NONE, 2,
                               OSCATS_TYPE_EXAMINEE, G_TYPE_BIT_ARRAY);

/**
 * OscatsTest::select:
 * @test: an #OscatsTest
 * @e: an #OscatsExaminee
 * @eligible: a #GBitArray indicating eligible items in the itembank
 *
 * Item selection.  The algorithm connecting to #OscatsTest::select must
 * select a single item from those in the item bank that are @eligible, or
 * return -1 on failure.  Only one algorithm should connect to
 * #OscatsTest::select.  If multiple algorithms connect, only the results
 * from the last connected algorithm will be used.
 *
 * Returns: the index of the selected #OscatsItem, or -1 on failure
 */
  klass->select = g_signal_new("select",
                               OSCATS_TYPE_TEST, G_SIGNAL_ACTION, 0,
                               NULL, NULL,
                               g_cclosure_user_marshal_INT__OBJECT_OBJECT,
                               G_TYPE_INT, 2,
                               OSCATS_TYPE_EXAMINEE, G_TYPE_BIT_ARRAY);

/**
 * OscatsTest::approve:
 * @test: an #OscatsTest
 * @e: an #OscatsExaminee
 * @item: the proposed #OscatsItem
 *
 * Item approval.  Algorithms (e.g. Sympson-Hetter) connecting to
 * #OscatsTest::approve should indicate whether the proposed @item is
 * acceptable.  If multiple algorithms attach to #OscatsTest::approve, each
 * one must approve an item before it will be administered to examinee @e. 
 * The @item may be %NULL, in which case handlers must silently return
 * %TRUE.
 *
 * Returns: %FALSE to accept the proposed item, %TRUE to reject
 */
  klass->approve = g_signal_new("approve",
                                OSCATS_TYPE_TEST, G_SIGNAL_ACTION, 0,
                                accumulate_boolean_or, NULL,
                              g_cclosure_user_marshal_BOOLEAN__OBJECT_OBJECT,
                                G_TYPE_BOOLEAN, 2,
                                OSCATS_TYPE_EXAMINEE, OSCATS_TYPE_ITEM);

/**
 * OscatsTest::administer:
 * @test: an #OscatsTest
 * @e: an #OscatsExaminee
 * @item: the proposed #OscatsItem
 *
 * Item administration.  Only one administrator should connect to the
 * #OscatsTest::administer signal.  If multiple algorithms attach to
 * #OscatsTest::administer, only the response returned from the last
 * administrator will be reported to algorithms connected to
 * #OscatsTest::administered.  Note: An algorithm connected to this signal
 * should call oscats_examinee_add_item(), as appropriate, to record the
 * response for the examinee so that it can be used in subsequent
 * ability/classification estimation.  Whether or not the response is
 * recorded (tests with online calibration, for example, may not record
 * every response), items are still not administered more than once.
 *
 * Returns: the #guint response of examinee @e to @item
 */
  klass->administer = g_signal_new("administer",
                                   OSCATS_TYPE_TEST, G_SIGNAL_ACTION, 0,
                                   NULL, NULL,
                                g_cclosure_user_marshal_UCHAR__OBJECT_OBJECT,
                                   G_TYPE_UCHAR, 2,
                                   OSCATS_TYPE_EXAMINEE, OSCATS_TYPE_ITEM);

/**
 * OscatsTest::administered:
 * @test: an #OscatsTest
 * @e: an #OscatsExaminee
 * @item: the proposed #OscatsItem
 * @resp: the #guint response
 *
 * The #OscatsTest::administered signal allows algorithms to record
 * statistics concerning items administered and the examinee's responses. 
 * In particular, an algorithm to update the examinee's estimated
 * ability/class should be connected to #OscatsTest::administered.
 */
  klass->administered = g_signal_new("administered",
                                     OSCATS_TYPE_TEST, G_SIGNAL_ACTION, 0,
                                     NULL, NULL,
                           g_cclosure_user_marshal_VOID__OBJECT_OBJECT_UCHAR,
                                     G_TYPE_NONE, 3,
                                     OSCATS_TYPE_EXAMINEE, OSCATS_TYPE_ITEM,
                                     G_TYPE_UCHAR);

/**
 * OscatsTest::stopcrit:
 * @test: an #OscatsTest
 * @e: an #OscatsExaminee
 *
 * Stoping criterion.  If multiple algorithms attach to #OscatsTest::stopcrit,
 * only one is required to end the test.  Note, the test will never have
 * more than #OscatsTest:itermax_items items.
 *
 * Returns: %TRUE to end the test, %FALSE to continue with another item
 */
  klass->stopcrit = g_signal_new("stopcrit",
                                 OSCATS_TYPE_TEST, G_SIGNAL_ACTION, 0,
                                 accumulate_boolean_or, NULL,
                                 g_cclosure_user_marshal_BOOLEAN__OBJECT,
                                 G_TYPE_BOOLEAN, 1,
                                 OSCATS_TYPE_EXAMINEE);

/**
 * OscatsTest::finalize:
 * @test: an #OscatsTest
 * @e: an #OscatsExaminee
 *
 * Test finalization.  Algorithms may compute final statistics and clean up
 * internal data structures by connecting to #OscatsTest::finalize.  It is
 * guaranteed that the test will emit #OscatsTest::finalize, even if the
 * test is aborted.
 */
  klass->finalize = g_signal_new("finalize",
                                 OSCATS_TYPE_TEST, G_SIGNAL_ACTION, 0,
                                 NULL, NULL,
                                 g_cclosure_marshal_VOID__OBJECT,
                                 G_TYPE_NONE, 1,
                                 OSCATS_TYPE_EXAMINEE);

}

static void oscats_test_init (OscatsTest *self)
{
}

static void oscats_test_dispose (GObject *object)
{
  OscatsTest *self = OSCATS_TEST(object);
  G_OBJECT_CLASS(oscats_test_parent_class)->dispose(object);
  if (self->itembank)
  {
    oscats_administrand_unfreeze(OSCATS_ADMINISTRAND(self->itembank));
    g_object_unref(self->itembank);
  }
  if (self->hint) g_object_unref(self->hint);
  self->itembank = NULL;
  self->hint = NULL;
}

static void oscats_test_set_property(GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec)
{
  OscatsTest *self = OSCATS_TEST(object);
  GString *id;
  switch (prop_id)
  {
    case PROP_ID:			// construction only
      self->id = g_value_dup_string(value);
      if (!self->id)
      {
        id = g_string_sized_new(18);
        g_string_printf(id, "[Item %p]", self);
        self->id = id->str;
        g_string_free(id, FALSE);
      }
      break;
    
    case PROP_BANK:			// construction only
      self->itembank = g_value_dup_object(value);
      oscats_administrand_freeze(OSCATS_ADMINISTRAND(self->itembank));
      break;
    
    case PROP_LENGTH_HINT:
      self->length_hint = g_value_get_uint(value);
      break;
    
    case PROP_ITERMAX_SELECT:
      self->itermax_select = g_value_get_uint(value);
      break;
    
    case PROP_ITERMAX_ITEMS:
      self->itermax_items = g_value_get_uint(value);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void oscats_test_get_property(GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
  OscatsTest *self = OSCATS_TEST(object);
  switch (prop_id)
  {
    case PROP_ID:
      g_value_set_string(value, self->id);
      break;
      
    case PROP_BANK:
      g_value_set_object(value, self->itembank);
      break;
    
    case PROP_LENGTH_HINT:
      g_value_set_uint(value, self->length_hint);
      break;
    
    case PROP_ITERMAX_SELECT:
      g_value_set_uint(value, self->itermax_select);
      break;
    
    case PROP_ITERMAX_ITEMS:
      g_value_set_uint(value, self->itermax_items);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/**
 * oscats_test_administer:
 * @test: the #OscatsTest to administer
 * @e: the #OscatsExaminee taking the test
 *
 * Core CAT administration loop.  The particular algorithms used to 
 * select and administer items as well as update ability/classification
 * estimates and tabulate statistics are specified by means of signals
 * connected to @test.
 *
 * The test proceeds in the following order:
 * <orderedlist>
 *  <listitem><para>The examinee's item/response vectors are reset.</para></listitem>
 *  <listitem><para>The #OscatsTest::initialize signal is emitted.</para></listitem>
 *  <listitem><para>The item eligibility vector is initialized with the current hinted value (default: all items).</para></listitem>
 *  <listitem><para>The #OscatsTest::filter, #OscatsTest::select, and #OscatsTest::approve signals are emitted.</para></listitem>
 *  <listitem><para>If the approval handler returns %TRUE, goto 3 (not more than #OscatsTest:itermax_select times).</para></listitem>
 *  <listitem><para>The #OscatsTest::administer signal is emitted (which should add the item/response pair to @e as necessary).</para></listitem>
 *  <listitem><para>The #OscatsTest::administered and #OscatsTest::stopcrit signals are emitted.</para></listitem>
 *  <listitem><para>If the stopping criterion has not been met, goto 3 (not more than #OscatsTest:itermax_items times).</para></listitem>
 *  <listitem><para>The #OscatsTest::finalize signal is emitted.</para></listitem>
 * </orderedlist>
 *
 * Algorithms must be connected to at minimum #OscatsTest::select, 
 * #OscatsTest::administer, and #OscatsTest::stopcrit.  An initial guess for
 * the examinee's latent IRT ability or classification must be supplied.
 */
void oscats_test_administer(OscatsTest *test, OscatsExaminee *e)
{
  OscatsTestClass *klass;
  GBitArray *eligible;
  GArray *seen_items;
  OscatsItem *item;
  gint item_index;
  guint8 resp;
  gboolean reselect, stop = TRUE;
  guint num_items, i, iter_select, iter_items = 0;
  
  g_return_if_fail(OSCATS_IS_TEST(test) && OSCATS_IS_EXAMINEE(e));
  num_items = oscats_item_bank_num_items(test->itembank);
  g_return_if_fail(num_items > 0);
  klass = OSCATS_TEST_GET_CLASS(test);
  eligible = g_bit_array_new(num_items);
  if (!test->hint)
  {
    test->hint = g_bit_array_new(num_items);
    g_bit_array_reset(test->hint, TRUE);
  }
  seen_items = g_array_sized_new(FALSE, FALSE, sizeof(guint),
                                 test->length_hint);
  
  oscats_examinee_prep(e, test->length_hint);
  g_signal_emit(test, klass->initialize, 0, e);
  do
  {
    iter_select = 0;
    do
    {
      if (iter_select++ == test->itermax_select)
      {
        g_warning("Maximum number (%d) of iterations for selecting item %d"
                  " reached in test [%s] for examinee [%s].",
                  test->itermax_select, iter_items+1, test->id, e->id);
        goto bail;
      }
      item_index = -1;	// In case nothing is connected to ::select
      g_bit_array_copy(eligible, test->hint);
      for (i=0; i < seen_items->len; i++)
        g_bit_array_clear_bit(eligible, g_array_index(seen_items, guint, i));
      g_signal_emit(test, klass->filter, 0, e, eligible);
      g_signal_emit(test, klass->select, 0, e, eligible, &item_index);
      if (item_index < 0 || item_index >= num_items)
        item = NULL;
      else
        item = g_ptr_array_index(test->itembank->items, item_index);
      reselect = FALSE;
      g_signal_emit(test, klass->approve, 0, e, item, &reselect);
    } while (reselect);
    if (!item)
    {			// Reached only if nothing connected to ::approve
      g_warning("No item selected in test [%s] for examinee [%s].",
                test->id, e->id);
      goto bail;
    }
    g_signal_emit(test, klass->administer, 0, e, item, &resp);
    g_array_append_val(seen_items, item_index);
    g_signal_emit(test, klass->administered, 0, e, item, resp);
    g_signal_emit(test, klass->stopcrit, 0, e, &stop);
  } while(!stop && ++iter_items < test->itermax_items);
  if (iter_items == test->itermax_items)
    g_warning("Maximum number (%d) of items reached in test [%s] "
              "for examinee [%s].", iter_items, test->id, e->id);

bail:
  g_signal_emit(test, klass->finalize, 0, e);
  g_object_unref(eligible);
  g_array_free(seen_items, TRUE);
}

/**
 * oscats_test_set_hint:
 * @test: an #OscatsTest
 * @hint: the initially eligible items
 *
 * Sets the item eligibility hint.  The length of @hint must be the same as
 * the number of items in #OscatsTest:itembank.  This is generally called in
 * handlers connected to #OscatsTest::initialize or
 * #OscatsTest::administered.
 */
void oscats_test_set_hint(OscatsTest *test, GBitArray *hint)
{
  g_return_if_fail(OSCATS_IS_TEST(test) && G_IS_BIT_ARRAY(hint) &&
                   oscats_item_bank_num_items(test->itembank) == 
                   g_bit_array_get_len(hint));
  if (!test->hint)
    test->hint = g_bit_array_new(g_bit_array_get_len(hint));
  g_bit_array_copy(test->hint, hint);
}
