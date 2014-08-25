/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Copyright 2011 Michael Culbertson <culbert1@illinois.edu>
 *
 * Example 3
 *
 * 300 Items: 3PL and DINA, read from file "ex03-items.dat"
 * 200 Examinees: Read from file "ex03-person.dat"
 * Simulate based on DINA (8 attributes).
 * Initial estimate: theta ~ N(-.37, 0.05), attributes ~ Bernoulli(0.5)
 * Item selection (choose from among 5):
 *  - pick randomly
 *  - fisher information
 *  - KL IRT information
 *  - KL CD information
 * Test length: 30
 * Report:
 *  - MSE(theta.hat)
 *  - Correct pattern rate
 *  - Correct attribute rate
 *  - Number of incorrect attributes/person
 *  - Item exposure
 */

#include <stdio.h>
#include <math.h>
#include <oscats.h>

#define N_EXAMINEES 200
#define N_ITEMS 300
#define N_ATTR 8
#define LEN 30

// Helper function reads up to the next '\n'
void skip_header(FILE *f) { while (!(fgetc(f) == '\n' || feof(f))); }

OscatsItemBank * read_items(FILE *f,
                            OscatsSpace *contSpace, OscatsSpace *binSpace)
{
  OscatsModel *model;
  OscatsAdministrand *item;
  // Create an item bank to store the items.
  // Setting the property "sizeHint" increases allocation efficiency.
  // Be sure to include NULL at the end of calls to g_object_new()!
  OscatsItemBank *bank = g_object_new(OSCATS_TYPE_ITEM_BANK,
                                      "sizeHint", N_ITEMS, NULL);
  OscatsDim dim;
  GString *str = g_string_new(NULL);
  gdouble a, b, c, slip, guess;
  guint i, count = 0;
  gchar attrStr[N_ATTR+1];  // The fixed-length buffer isn't exactly safe...
  
  skip_header(f);
  while (6 == fscanf(f, "%lf %lf %lf %lf %lf %s", &a, &b, &c, &slip, &guess,
                     attrStr))
  {
    // Create the item
    g_string_printf(str, "%d", ++count);
    item = g_object_new(OSCATS_TYPE_ITEM, "id", str->str, NULL);
    // Create the 3PL model
    dim = OSCATS_DIM_CONT;	// First continuous dimension
    model = oscats_model_new(OSCATS_TYPE_MODEL_L3P, contSpace, &dim, 1, NULL);
    oscats_model_set_param_by_name(model, "Discr.Cont.1", a);
    /* The difficulty parameter in the input file is specified as the
     * traditional IRT difficulty: logit P(x) = a(theta-b); but,
     * OscatsModelL3p uses the parameterization: logit P(x) = a*theta - b,
     * so we have to multiply here, accordingly.  */
    oscats_model_set_param_by_name(model, "Diff", a*b);
    oscats_model_set_param_by_name(model, "Guess", c);
    // Set the model (takes model's reference)
    oscats_administrand_set_model_by_name(item, "3PL", model);
    // Create the DINA model
    dim = OSCATS_DIM_BIN;	// First binary dimension
    for (i=0; attrStr[i] == '0' && i < N_ATTR; i++) dim++;
    model = oscats_model_new(OSCATS_TYPE_MODEL_DINA, binSpace, &dim, 1, NULL);
    oscats_model_set_param_by_name(model, "Slip", slip);
    oscats_model_set_param_by_name(model, "Guess", guess);
    // Set the model (takes model's reference)
    oscats_administrand_set_model_by_name(item, "DINA", model);
    // Add the item to the item bank
    oscats_item_bank_add_item(bank, item);
    g_object_unref(item);	// No longer needed here
  }
  if (count != N_ITEMS)
    g_warning("Expected %d items, but only read %d.", N_ITEMS, count);
  g_string_free(str, TRUE);
  return bank;
}

// Returns an array of new OscatsExaminee pointers
GPtrArray * read_examinees(FILE *f,
                           OscatsSpace *contSpace, OscatsSpace *binSpace)
{
  GPtrArray *ret = g_ptr_array_sized_new(N_EXAMINEES);
  GString *str = g_string_new(NULL);
  OscatsPoint *point;
  OscatsExaminee *e;
  OscatsDim dim;
  gdouble theta;
  guint i, count=0;
  gchar attrStr[N_ATTR+1];  // The fixed-length buffer isn't exactly safe...

  g_ptr_array_set_free_func(ret, g_object_unref);
  while (2 == fscanf(f, "%lf %s", &theta, attrStr))
  {
    // Create the examinee
    g_string_printf(str, "%d", ++count);
    e = g_object_new(OSCATS_TYPE_EXAMINEE, "id", str->str, NULL);
    // Set "true" continuous theta
    point = oscats_point_new_from_space(contSpace);
    dim = OSCATS_DIM_CONT;	// First continuous dimension
    oscats_point_set_cont(point, dim, theta);
    oscats_examinee_set_theta_by_name(e, "trueTheta", point);  // takes ref
    // Set "true" binary alpha
    point = oscats_point_new_from_space(binSpace);
    dim = OSCATS_DIM_BIN;	// First continuous dimension
    for (i=0; i < N_ATTR; i++, dim++)
      oscats_point_set_bin(point, dim, attrStr[i] == '1');
    oscats_examinee_set_theta_by_name(e, "trueAlpha", point);  // takes ref
    g_ptr_array_add(ret, e);
  }
  if (count != N_EXAMINEES)
    g_warning("Expected %d examinees, but read in %d.", N_EXAMINEES, count);

  g_string_free(str, TRUE);
  return ret;
}

int main()
{
  FILE *f;
  GPtrArray *examinees;
  OscatsSpace *contSpace, *binSpace;
  OscatsItemBank *bank;
  OscatsExaminee *e;
  OscatsDim dim;
  OscatsPoint *initTheta, *initAlpha;
  const guint num_tests = 4;
  const gchar *test_names[] = { "random", "FI", "KL.3PL", "KL.DINA" };
  OscatsTest *test[num_tests];
  OscatsAlgExposureCounter *exposure[num_tests];
  OscatsAlgClassRates *rates[num_tests];
  guint i, j, k;
  
  g_type_init();
  g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL);         // For debugging
  
  /* In the previous examples, we had only one latent space; but, here we
   * want to use two different models from *different* latent spaces.  The
   * first latent space is a unidimensional continuous space (for the 3PL
   * model); the second is an N_ATTR-dimensional binary space (for the DINA
   * model).  If appropriate to the simulation (e.g. to compare 2PL vs. 3PL)
   * we could have created two different models from the same space.
   *
   * Note: Even though OscatsSpace accommodates mixed-type (continuous and
   * binary) latent spaces, the models we use here are either only
   * continuous or only binary, so they come from different latent spaces. 
   * A mixed-type model would be something like the Fusion model, which has
   * both binary and continuous components.
   */
  contSpace = g_object_new(OSCATS_TYPE_SPACE, "numCont", 1, NULL);
  binSpace = g_object_new(OSCATS_TYPE_SPACE, "numBin", N_ATTR, NULL);
  initTheta = oscats_point_new_from_space(contSpace);
  initAlpha = oscats_point_new_from_space(binSpace);

  printf("Reading examinees.\n");
  f = fopen("ex03-person.dat", "r");
  examinees = read_examinees(f, contSpace, binSpace);
  fclose(f);
  printf("Reading items.\n");
  f = fopen("ex03-items.dat", "r");
  bank = read_items(f, contSpace, binSpace);
  fclose(f);

  printf("Creating tests.\n");
  for (j=0; j < num_tests; j++)
  {
    // Create a new test with the given properties
    // Be sure to end calls to g_object_new() with NULL!
    test[j] = g_object_new(OSCATS_TYPE_TEST, "id", test_names[j],
                           "itembank", bank, "length_hint", LEN, NULL);

    // Register the common CAT algorithms for this test.
    
    // Here, we always simulate from the DINA model.
    oscats_algorithm_register(
      g_object_new(OSCATS_TYPE_ALG_SIMULATE,
                   "modelKey", "DINA", "thetaKey", "trueAlpha", NULL),
      test[j]);

    // Usually we would only have one estimate algorithm, but here, we need
    // to estimate both the continuous ability and the latent class, each
    // based on a separate model.
    oscats_algorithm_register(
      g_object_new(OSCATS_TYPE_ALG_ESTIMATE,
                   "modelKey", "3PL", "thetaKey", "estTheta", NULL),
      test[j]);
    oscats_algorithm_register(
      g_object_new(OSCATS_TYPE_ALG_ESTIMATE,
                   "modelKey", "DINA", "thetaKey", "estAlpha", NULL),
      test[j]);

    exposure[j] = oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_EXPOSURE_COUNTER, NULL), test[j]);
    g_object_ref(exposure[j]);
    rates[j] = oscats_algorithm_register(
      g_object_new(OSCATS_TYPE_ALG_CLASS_RATES, "simKey", "trueAlpha",
                   "estKey", "estAlpha", NULL), test[j]);
    g_object_ref(rates[j]);

    oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_FIXED_LENGTH,
                                           "len", LEN, NULL), test[j]);
  }
  
  // Register the item selection criteria for the different tests:
  // Since we aren't just using the default model for everything, we have to
  // tell each algorithm which model/theta to use for selection.
  oscats_algorithm_register(g_object_new(OSCATS_TYPE_ALG_PICK_RAND, NULL), test[0]);
  oscats_algorithm_register(
    g_object_new(OSCATS_TYPE_ALG_MAX_FISHER, "num", 5,
                 "modelKey", "3PL", "thetaKey", "estTheta", NULL), test[1]);
  oscats_algorithm_register(
    g_object_new(OSCATS_TYPE_ALG_MAX_KL, "num", 5,
                 "modelKey", "3PL", "thetaKey", "estTheta", NULL), test[2]);
  oscats_algorithm_register(
    g_object_new(OSCATS_TYPE_ALG_MAX_KL, "num", 5,
                 "modelKey", "DINA", "thetaKey", "estAlpha", NULL), test[3]);
  
  printf("Administering.\n");
  f = fopen("ex03-person-results.dat", "w");
  fprintf(f, "ID\ttrue.theta\ttrue.alpha\tinit.theta\tinit.alpha");
  for (j=0; j < num_tests; j++)
    fprintf(f, "\t%s.theta\t%s.alpha", test_names[j], test_names[j]);
  fprintf(f, "\n");
  for (i=0; i < N_EXAMINEES; i++)
  {
    OscatsPoint *tmp;
    e = g_ptr_array_index(examinees, i);
    tmp = oscats_examinee_get_theta_by_name(e, "trueTheta");
    dim = OSCATS_DIM_CONT+0;	// first continuous dimension
    fprintf(f, "%s\t%g\t", e->id, oscats_point_get_cont(tmp, dim));
    tmp = oscats_examinee_get_theta_by_name(e, "trueAlpha");
    dim = OSCATS_DIM_BIN+0;	// first binary dimension
    for (k=0; k < N_ATTR; k++, dim++)
      fprintf(f, oscats_point_get_bin(tmp, dim) ? "1" : "0");

    // Choose initial theta and alpha for this examinee
    // We'll use the same initial abilities for all four tests
    dim = OSCATS_DIM_CONT+0;	// first continuous dimension
    oscats_point_set_cont(initTheta, dim, oscats_rnd_normal(sqrt(0.05))-0.37);
    dim = OSCATS_DIM_BIN+0;	// first binary dimension
    for (k=0; k < N_ATTR; k++, dim++)
      oscats_point_set_bin(initAlpha, dim, oscats_rnd_uniform() < 0.5);
    
    // Initialize ability estimates for this examinee
    // (Examinee takes references for the new OscatsPoint objects)
    oscats_examinee_set_theta_by_name(e, "estTheta",
                                      oscats_point_new_from_space(contSpace));
    oscats_examinee_set_theta_by_name(e, "estAlpha",
                                      oscats_point_new_from_space(binSpace));

    for (j=0; j < num_tests; j++)
    {
      // Reset initial latent ability for this test
      oscats_point_copy(
        oscats_examinee_get_theta_by_name(e, "estTheta"), initTheta);
      oscats_point_copy(
        oscats_examinee_get_theta_by_name(e, "estAlpha"), initAlpha);

      // Do the administration!
      oscats_test_administer(test[j], e);

      // Output the resulting theta.hat and alpha.hat
      tmp = oscats_examinee_get_theta_by_name(e, "estTheta");
      dim = OSCATS_DIM_CONT+0;	// first continuous dimension
      fprintf(f, "\t%g\t", oscats_point_get_cont(tmp, dim));
      tmp = oscats_examinee_get_theta_by_name(e, "estAlpha");
      dim = OSCATS_DIM_BIN+0;	// first binary dimension
      for (k=0; k < N_ATTR; k++, dim++)
        fprintf(f, oscats_point_get_bin(tmp, dim) ? "1" : "0");
    }
    fprintf(f, "\n");
  }
  fclose(f);
  
  f = fopen("ex03-exposure.dat", "w");
  fprintf(f, "ID");
  for (j=0; j < num_tests; j++)
    fprintf(f, "\t%s", test_names[j]);
  fprintf(f, "\n");
  for (i=0; i < N_ITEMS; i++)
  {
    OscatsItem *item = OSCATS_ITEM(oscats_item_bank_get_item(bank, i));
    fprintf(f, "%d", i+1);
    // Get the exposure rate for this item in each test
    for (j=0; j < num_tests; j++)
      fprintf(f, "\t%g",
            oscats_alg_exposure_counter_get_rate(exposure[j], item));
    fprintf(f, "\n");
  }
  fclose(f);

  f = fopen("ex03-rates.dat", "w");
  fprintf(f, "Rate");
  for (j=0; j < num_tests; j++)
    fprintf(f, "\t%s", test_names[j]);
  fprintf(f, "\nPattern");
  for (j=0; j < num_tests; j++)
    fprintf(f, "\t%g", oscats_alg_class_rates_get_pattern_rate(rates[j]));
  for (k=0; k < N_ATTR; k++)
  {
    fprintf(f, "\nattr.%d", k+1);
    for (j=0; j < num_tests; j++)
      fprintf(f, "\t%g",
              oscats_alg_class_rates_get_attribute_rate(rates[j], k));
  }
  for (k=0; k <= N_ATTR; k++)
  {
    fprintf(f, "\nfreq.%d", k);
    for (j=0; j < num_tests; j++)
      fprintf(f, "\t%g",
              oscats_alg_class_rates_get_misclassify_freq(rates[j], k));
  }
  fprintf(f, "\n");
  fclose(f);

  // Clean up
  printf("Done.\n");
  for (j=0; j < num_tests; j++)
  {
    g_object_unref(exposure[j]);
    g_object_unref(rates[j]);
    g_object_unref(test[j]);
  }
  g_object_unref(bank);
  g_ptr_array_free(examinees, TRUE);
  g_object_unref(initTheta);
  g_object_unref(initAlpha);
  g_object_unref(contSpace);
  g_object_unref(binSpace);
  
  return 0;
}
