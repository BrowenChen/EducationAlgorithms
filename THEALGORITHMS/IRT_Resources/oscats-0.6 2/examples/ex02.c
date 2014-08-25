/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Copyright 2011 Michael Culbertson <culbert1@illinois.edu>
 *
 * Example 2
 *
 * 500 Items: 2D 2PL with covariate
 *   a1 ~ U(0, 1), a2 ~ U(0, 2), b ~ N(0,3), covariate coef ~ U(0.5, 1.5)
 * 500 Examinees on each point of the grid {-1, 0, 1} x {-1, 0, 1}
 *   Examinee covariate ~ N(0, 1)
 * Item selection:
 *  - A-optimality
 *  - D-optimality
 *  - KL
 * Test length: 20
 * Report:
 *  - Mean Euclidian distance between estimated and simulated theta,
 *      calculated for each grid point
 */

#include <stdio.h>
#include <math.h>
#include <oscats.h>

#define N_EXAMINEES 10
#define N_ITEMS 500
#define LEN 20
#define COVARIATE_NAME "MyCovariate"

OscatsDim dims[2];

// Helper function to calculate the Euclidian distance between two points
gdouble distance(OscatsPoint *a, OscatsPoint *b)
{
  gdouble d1 = oscats_point_get_cont(a, dims[0])
             - oscats_point_get_cont(b, dims[0]);
  gdouble d2 = oscats_point_get_cont(a, dims[1])
             - oscats_point_get_cont(b, dims[1]);
  return sqrt(d1*d1 + d2*d2);
}

// We will have to tell the models which covariate to use.  This is done
// by supplying an OscatsCovariates object that includes only that covariate
// to be included in the model.  In this case, all of our items will use
// the same covariate.  We'll create the OscatsCovariates object below when
// we create the latent space, and pass both to the gen_items() function.
OscatsItemBank * gen_items(OscatsSpace *space, OscatsCovariates *covariate)
{
  OscatsModel *model;
  OscatsItem *item;
  // Create an item bank to store the items.
  // Setting the property "sizeHint" increases allocation efficiency.
  // Be sure to include NULL at the end of calls to g_object_new()!
  OscatsItemBank *bank = g_object_new(OSCATS_TYPE_ITEM_BANK,
                                      "sizeHint", N_ITEMS, NULL);
  guint i;
  // Create the items
  for (i=0; i < N_ITEMS; i++)
  {
    // First we create an IRT model container for our item
    // We'll use this helper function to supply the dimensions to be used
    model = oscats_model_new(OSCATS_TYPE_MODEL_L2P, space, dims, 2, covariate);
    // Then, set the parameters. Here there are 4:
    // Discrimination on two dimensions, difficulty, and covariate coef.
    oscats_model_set_param_by_name(model, "Diff",
                                   oscats_rnd_normal(sqrt(3)));
    oscats_model_set_param_by_name(model, "Discr.Cont.1",
                                   oscats_rnd_uniform_range(0, 1));
    oscats_model_set_param_by_name(model, "Discr.Cont.2",
                                   oscats_rnd_uniform_range(0, 2));
    oscats_model_set_param_by_name(model, COVARIATE_NAME,
                                   oscats_rnd_uniform_range(0.5, 1.5));
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

int main()
{
  FILE *f;
  OscatsSpace *space;
  OscatsCovariates *covariate;
  OscatsPoint *sim_theta, *est_theta;
  OscatsExaminee *e;
  OscatsItemBank *bank;
  const guint num_tests = 3;
  const gchar *test_names[] = { "A-opt", "D-opt", "KL", };
  const guint num_grid = 3;
  const gdouble grid[] = { -1, 0, 1 };
  OscatsTest *test[num_tests];
  gdouble *err_sums[num_tests];		// For error calculation
  guint i, j, I, J;
  
  // With G_TYPE_DEBUG_OBJECTS, we check for leaks at the end of the program
  // Could just have called g_type_init(), instead.
  g_type_init_with_debug_flags(G_TYPE_DEBUG_OBJECTS);
  g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);		// For debugging
  
  // Create the latent space for the test: 2 continuous dimensions
  space = g_object_new(OSCATS_TYPE_SPACE, "numCont", 2, NULL);

  // Fetch the first and second continuous dimensions via the default names
  dims[0] = oscats_space_get_dim_by_name(space, "Cont.1");
  dims[1] = oscats_space_get_dim_by_name(space, "Cont.2");

  // Create the covariate
  covariate = g_object_new(OSCATS_TYPE_COVARIATES, NULL);
  oscats_covariates_set_by_name(covariate, COVARIATE_NAME, 0);
  // "covariate" variable now includes one covariate called "MyCovariate"
  // It's current value is 0.

  // Set up space for error calculation
  for (j=0; j < num_tests; j++)
    err_sums[j] = g_new0(gdouble, num_grid*num_grid);

  printf("Creating items.\n");
  bank = gen_items(space, covariate);

  printf("Creating tests.\n");
  for (j=0; j < num_tests; j++)
  {
    // Create a new test with the given properties
    // Be sure to end calls to g_object_new() with NULL!

    // In this case, we know what the length of the test will be (20),
    // so, we set length_hint for allocation efficiency.
    // Any good/reasonable guess will do---the size will be adjusted
    // automatically as necessary.
    test[j] = g_object_new(OSCATS_TYPE_TEST, "id", test_names[j],
                           "itembank", bank, "length_hint", LEN, NULL);

    // Register the CAT algorithms for this test.
    // A test must have at minimum a selection algorithm, and administration
    // algorithm, and a stoping critierion.

    // See note in Example 1 on reference counts for algorithm objects.
    oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_SIMULATE, NULL), test[j]);
    oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_ESTIMATE,
                                           "Nposterior", 5, NULL), test[j]);
    oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_FIXED_LENGTH,
                                           "len", LEN, NULL), test[j]);
  }
  
  // Here we register the item selection criteria for the different tests
  oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_MAX_FISHER,
                                         "A-opt", TRUE, "num", 5, NULL),
                            test[0]);
  oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_MAX_FISHER,
                                         "num", 5, NULL), test[1]);
  oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_MAX_KL,
                                         "num", 5, NULL), test[2]);
  
  // In Example 1, we created OscatsExaminee objects for each examinee.
  // Here, we'll just use one object over again for each administration.
  e = g_object_new(OSCATS_TYPE_EXAMINEE, "covariates", covariate, NULL);
  sim_theta = oscats_point_new_from_space(space);
  est_theta = oscats_point_new_from_space(space);
  // Note: Examinee takes ownership of sim_theta and est_theta
  // For good form, we should g_object_ref() them for our own copies, but
  // we know that we won't reference them beyond the life of e.
  oscats_examinee_set_sim_theta(e, sim_theta);
  oscats_examinee_set_est_theta(e, est_theta);

  printf("Administering.\n");
  for (I=0; I < num_grid; I++)
  {
    oscats_point_set_cont(sim_theta, dims[0], grid[I]);
    for (J=0; J < num_grid; J++)
    {
      printf("\rAt grid point (%g, %g) ...\n", grid[I], grid[J]);
      oscats_point_set_cont(sim_theta, dims[1], grid[J]);
      for (i=0; i < N_EXAMINEES; i++)
      {
        // "covariate" is the same OscatsCovariates object used by "e"
        oscats_covariates_set_by_name(covariate, COVARIATE_NAME,
                                      oscats_rnd_normal(1));
        printf("\r%d", i); fflush(stdout);
        for (j=0; j < num_tests; j++)
        {
          // Reset initial theta: Random start ~ N_2(0, I_2)
          oscats_point_set_cont(est_theta, dims[0], oscats_rnd_normal(1));
          oscats_point_set_cont(est_theta, dims[1], oscats_rnd_normal(1));
          // Administer the test
          oscats_test_administer(test[j], e);
          // Calculate error (Euclidean distance between sim/est theta)
          err_sums[j][I*num_grid+J] += distance(sim_theta, est_theta);
        }
      }
    }
  }

  // Report results
  f = fopen("ex02-results.txt", "w");
  fprintf(f, "D1\tD2");
  for (j=0; j < num_tests; j++)
    fprintf(f, "\t%s", test_names[j]);
  fprintf(f, "\n");
  for (I=0; I < num_grid; I++)
    for (J=0; J < num_grid; J++)
    {
      fprintf(f, "%g\t%g", grid[I], grid[J]);
      for (j=0; j < num_tests; j++)
        fprintf(f, "\t%g", err_sums[j][I*num_grid+J]/N_EXAMINEES);
      fprintf(f, "\n");
    }
  fclose(f);

  // Clean up
  printf("\rDone.\n");
  for (j=0; j < num_tests; j++)
  {
    g_object_unref(test[j]);
    g_free(err_sums[j]);
  }
  g_object_unref(bank);
  g_object_unref(e);	// destroys sim_theta and est_theta
  g_object_unref(covariate);
  g_object_unref(space);
  
  return 0;
}
