/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Copyright 2010, 2011 Michael Culbertson <culbert1@illinois.edu>
 *
 * Example 1
 *
 * 400 Items: 1PL, b ~ N(0,1)
 * 1000 Examinees: theta ~ N(0,1)
 * Item selection:
 *  - pick randomly
 *  - match theta with b, exactly
 *  - match theta with b, randomize 5, 10 items
 * Test length: 30
 * Report:
 *  - theta.hat, Item exposure
 */

#include <stdio.h>
#include <glib.h>
#include <oscats.h>

#define N_EXAMINEES 1000
#define N_ITEMS 400
#define LEN 30

OscatsItemBank * gen_items(OscatsSpace *space)
{
  OscatsModel *model;
  OscatsItem *item;
  // Create an item bank to store the items.
  // Setting the property "sizeHint" increases allocation efficiency.
  // Be sure to include NULL at the end of calls to g_object_new()!
  OscatsItemBank *bank = g_object_new(OSCATS_TYPE_ITEM_BANK,
                                      "sizeHint", N_ITEMS, NULL);
  guint i;
  for (i=0; i < N_ITEMS; i++)
  {
    // First we create an IRT model container for our item
    // Defaults to unidimensional, using the first dimension of space
    model = g_object_new(OSCATS_TYPE_MODEL_L1P, "space", space, NULL);
    // Then, set the parameters.  Here there is only one, the difficulty (b).
    oscats_model_set_param_by_index(model, 0, oscats_rnd_normal(1));
    // Create an item based on this model (as default model)
    // Item takes ownership of model
    item = oscats_item_new(OSCATS_DEFAULT_KEY, model);
    // Add the item to the item bank
    oscats_item_bank_add_item(bank, OSCATS_ADMINISTRAND(item));
    // We no longer need item (it's stored safely in the bank), so unref here
    g_object_unref(item);
  }
  return bank;
}

// Returns an array of new OscatsExaminee pointers
OscatsExaminee ** gen_examinees(OscatsSpace *space)
{
  OscatsExaminee ** ret = g_new(OscatsExaminee*, N_EXAMINEES);
  OscatsPoint *theta;
  OscatsDim dim = OSCATS_DIM_CONT + 0;  // First continuous dimension
  guint i;
  
  for (i=0; i < N_EXAMINEES; i++)
  {
    // Latent IRT ability parameter.  This is a one-dimensional test.
    theta = oscats_point_new_from_space(space);
    // Sample the ability from N(0,1) distribution
    oscats_point_set_cont(theta, dim, oscats_rnd_normal(1));
    // Create a new examinee
    ret[i] = g_object_new(OSCATS_TYPE_EXAMINEE, NULL);
    // Set the examinee's true (simulated) ability
    // Examinee assumes ownership of theta
    oscats_examinee_set_sim_theta(ret[i], theta);
  }
  
  return ret;
}

int main()
{
  FILE *f;
  OscatsSpace *space;
  OscatsExaminee **examinees;
  OscatsItemBank *bank;
  OscatsDim dim = OSCATS_DIM_CONT + 0;   // First continuous dimension
  const guint num_tests = 4;
  const gchar *test_names[] = { "random", "matched", "match.5", "match.10" };
  OscatsTest *test[num_tests];
  OscatsAlgExposureCounter *exposure[num_tests];
  guint i, j;
  
  // With G_TYPE_DEBUG_OBJECTS, we check for leaks at the end of the program
  // Could just have called g_type_init(), instead.
  g_type_init_with_debug_flags(G_TYPE_DEBUG_OBJECTS);
  
  // Create the latent space for the test: continuous unidimensional
  space = g_object_new(OSCATS_TYPE_SPACE, "numCont", 1, NULL);

  printf("Creating examinees.\n");
  examinees = gen_examinees(space);
  printf("Creating items.\n");
  bank = gen_items(space);

  printf("Creating tests.\n");
  for (j=0; j < num_tests; j++)
  {
    // Create a new test with the given properties
    // Be sure to end calls to g_object_new() with NULL!

    // In this case, we know what the length of the test will be (30),
    // so, we set length_hint for allocation efficiency.
    // Any good/reasonable guess will do---the size will be adjusted
    // automatically as necessary.
    test[j] = g_object_new(OSCATS_TYPE_TEST, "id", test_names[j],
                           "itembank", bank, "length_hint", LEN, NULL);

    // Register the CAT algorithms for this test.
    // A test must have at minimum a selection algorithm, and administration
    // algorithm, and a stoping critierion.

    // Algorithms are "floating" when they are created.  Registration
    // "sinks" the floating reference.  In other words, we don't have to
    // worry about keeping track of references to algorithms, unless we want
    // to keep a copy of the pointer ourselves, in which case, we need to
    // call g_object_ref_sink() or g_object_ref() on the algorithm.
    oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_SIMULATE, NULL), test[j]);
    oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_ESTIMATE, NULL), test[j]);

    // In many cases, we don't care about the algorithm objects we create
    // after they're registered, since they don't contain any interesting
    // information.  But, for the item exposure counter, we want to have
    // access to the item exposure rates when the test is over, so we keep
    // the object.
    exposure[j] = oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_EXPOSURE_COUNTER, NULL), test[j]);
    // But, the object doesn't belong to us, yet, so increase ref count:
    g_object_ref(exposure[j]);

    oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_FIXED_LENGTH,
                                           "len", LEN, NULL), test[j]);
  }
  
  // Here we register the item selection criteria for the different tests
  oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_PICK_RAND, NULL), test[0]);
  // The default for OscatsAlgClosestDiff is to pick the exact closest item
  oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_CLOSEST_DIFF, NULL), test[1]);
  // But, we can have the algorithm choose randomly from among the num closest
  oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_CLOSEST_DIFF,
                                         "num", 5, NULL), test[2]);
  oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_CLOSEST_DIFF,
                                         "num", 10, NULL), test[3]);
  
  printf("Administering.\n");
  f = fopen("ex01-examinees.dat", "w");
  fprintf(f, "ID\ttrue");
  for (j=0; j < num_tests; j++)
    fprintf(f, "\t%s", test_names[j]);
  fprintf(f, "\n");
  for (i=0; i < N_EXAMINEES; i++)
  {
//    printf("  %s\n", examinees[i]->id);

    // An initial estimate for latent IRT ability must be provided.
    // Note that examinee assumes ownership of the OscatsPoint
    oscats_examinee_set_est_theta(examinees[i],
                                  oscats_point_new_from_space(space));

    fprintf(f, "%d\t%g", i+1,
      oscats_point_get_cont(oscats_examinee_get_sim_theta(examinees[i]), dim));
    for (j=0; j < num_tests; j++)
    {
      // Reset initial latent ability for this test
      oscats_point_set_cont(oscats_examinee_get_est_theta(examinees[i]),
                            dim, 0);
      // Do the administration!
      oscats_test_administer(test[j], examinees[i]);
      // Output the resulting theta.hat
      fprintf(f, "\t%g",
        oscats_point_get_cont(oscats_examinee_get_est_theta(examinees[i]),
                              dim) );
    }
    fprintf(f, "\n");
  }
  fclose(f);
  
  f = fopen("ex01-items.dat", "w");
  fprintf(f, "ID\tb");
  for (j=0; j < num_tests; j++)
    fprintf(f, "\t%s", test_names[j]);
  fprintf(f, "\n");
  for (i=0; i < N_ITEMS; i++)
  {
    // Get the item's difficulty parameter (for the default model)
    fprintf(f, "%d\t%g", i+1,
            oscats_model_get_param_by_index(
              oscats_administrand_get_model(
                oscats_item_bank_get_item(bank, i), OSCATS_DEFAULT_KEY), 0));
    // Get the exposure rate for this item in each test
    for (j=0; j < num_tests; j++)
      fprintf(f, "\t%g",
            oscats_alg_exposure_counter_get_rate(exposure[j],
                  OSCATS_ITEM(oscats_item_bank_get_item(bank, i))) );
    fprintf(f, "\n");
  }
  fclose(f);

  // Clean up
  printf("Done.\n");
  for (j=0; j < num_tests; j++)
  {
    g_object_unref(exposure[j]);
    g_object_unref(test[j]);
  }
  g_object_unref(bank);
  for (i=0; i < N_EXAMINEES; i++)
    g_object_unref(examinees[i]);
  g_free(examinees);
  
  return 0;
}
