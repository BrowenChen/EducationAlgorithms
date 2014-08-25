<?
# OSCATS: Open-Source Computerized Adaptive Testing System
# Copyright 2011 Michael Culbertson <culbert1@illinois.edu>
#
# Example 2
#
# 500 Items: 2D 2PL with covariate
#   a1 ~ U(0, 1), a2 ~ U(0, 2), b ~ N(0,3), covariate coef ~ U(0.5, 1.5)
# 500 Examinees on each point of the grid {-1, 0, 1} x {-1, 0, 1}
#   Examinee covariate ~ N(0, 1)
# Item selection:
#  - A-optimality
#  - D-optimality
#  - KL
# Test length: 20
# Report:
#  - Mean Euclidian distance between estimated and simulated theta,
#      calculated for each grid point

  # You *must* load the php_gtk2 extension *before* the OSCATS extension
  # This may be done dynamically (as here), or in php.ini
  if (!class_exists('gtk'))
    dl('php_gtk2.so');
  if (!class_exists('Oscats'))
    dl('php_oscats.so');

  $N_EXAMINEES = 2;
  $N_ITEMS = 500;
  $LEN = 20;
  $COVARIATE_NAME = "MyCovariate";

  # Helper function to calculate the Euclidian distance between two points
  function distance($a, $b) {
    global $dims;
    $d1 = $a->get_cont($dims[0]) - $b->get_cont($dims[0]);
    $d2 = $a->get_cont($dims[1]) - $b->get_cont($dims[1]);
    return sqrt($d1*$d1 + $d2*$d2);
  }

  # We will have to tell the models which covariate to use.  This is done
  # by supplying an OscatsCovariates object that includes only that covariate
  # to be included in the model.  In this case, all of our items will use
  # the same covariate.  We'll create the OscatsCovariates object below when
  # we create the latent space, and pass both to the gen_items() function.
  function gen_items($space, $covariate) {
    global $N_ITEMS, $dims, $COVARIATE_NAME;
    # Create an item bank to store the items.
    # Setting the property "sizeHint" increases allocation efficiency

    # It's a bit kludgy, but the php gtk bindings require passing
    # the name of the object to the objects constructor in order to
    # set the construction properties.
    $bank = new OscatsItemBank('OscatsItemBank', array("sizeHint" => $N_ITEMS));
    # Create the items
    for ($i=0; $i < $N_ITEMS; $i++) {
      # First we create an IRT model container for our item
      # We have to specify which dimensions to be used with the "dims" array
      # (in this case, we use both of the dimensions of the space)
      $model = new OscatsModelL2p('OscatsModelL2p',
                                  array("space" => $space, "dims" => $dims,
                                        "covariates" => $covariate));
      # Then, set the parameters.  Here there are 4:
      # Discrimination on two dimensions, difficulty, and covariate coef.
      $model->set_param_by_name("Diff", Oscats::rnd_normal(sqrt(3)));
      $model->set_param_by_name("Discr.Cont.1",
                                Oscats::rnd_uniform_range(0, 1));
      $model->set_param_by_name("Discr.Cont.2",
                                Oscats::rnd_uniform_range(0, 2));
      $model->set_param_by_name($COVARIATE_NAME,
                                Oscats::rnd_uniform_range(0.5, 1.5));
      # Create an item based on this model
      # This is equivalent to oscats_item_new(0, model) in C
      $item = new OscatsItem(0, $model);
      # Add the item to the item bank
      $bank->add_item($item);
      # Since php is garbage collected, we don't have to worry about
      # reference counting.
    }
    return $bank;
  }

  ## main()

  $test_names = array("A-opt", "D-opt", "KL");
  $grid = array(-1, 0, 1);

  # Create the latent space for the test: 2 continuous dimensions
  $space = new OscatsSpace('OscatsSpace', array("numCont" => 2));

  # Fetch the first and second continuous dimensions via the default names
  $dims = array($space->get_dim_by_name("Cont.1"),
                $space->get_dim_by_name("Cont.2"));

  # Create the covariate
  $covariate = new OscatsCovariates();
  $covariate->set_by_name($COVARIATE_NAME, 0);
  # "covariate" variable now includes one covariate called "MyCovariate"
  # It's current value is 0.

  print "Creating items.\n";
  $bank = gen_items($space, $covariate);

  print "Creating tests.\n";
  # Create new tests with the given properties
    # In this case, we know what the length of the test will be (20),
    # so, we set length_hint for allocation efficiency.
    # Any good/reasonable guess will do---the size will be adjusted
    # automatically as necessary.
  $tests = array();
  $err_sums = array();
  foreach ($test_names as $name) {
    $tests[] = new OscatsTest('OscatsTest', array("id" => $name,
                              "itembank" => $bank, "length_hint" => $LEN));
    $err_sums[] = array();
  }
  # Register the CAT algorithms for each test
  # A test must have at minimum a selection algorithm, and administration
  # algorithm, and a stoping critierion.
  foreach ($tests as $test) {
    $alg = new OscatsAlgSimulate();
    $alg->register($test);
    $alg = new OscatsAlgEstimate();
    $alg->register($test);
    $alg = new OscatsAlgFixedLength('OscatsAlgFixedLength',
                                    array("len" => $LEN));
    $alg->register($test);
  }

  # Here we register the item selection criteria for the different tests
  $alg = new OscatsAlgMaxFisher('OscatsAlgMaxFisher',
                                array("A_opt" => True, "num" => 5));
  $alg->register($tests[0]);
  $alg = new OscatsAlgMaxFisher('OscatsAlgMaxFisher', array("num" => 5));
  $alg->register($tests[1]);
  $alg = new OscatsAlgMaxKl('OscatsAlgMaxKl', array("num" => 5));
  $alg->register($tests[2]);

  # In Example 1, we created OscatsExaminee objects for each examinee.
  # Here, we'll just use one object over again for each administration.
  $e = new OscatsExaminee('OscatsExaminee', array("covariates" => $covariate));
  $sim_theta = new OscatsPoint('OscatsPoint', array("space" => $space));
  $est_theta = new OscatsPoint('OscatsPoint', array("space" => $space));
  $e->set_sim_theta($sim_theta);
  $e->set_est_theta($est_theta);

  print "Administering.\n";
  for ($I=0; $I < count($grid); $I++) {
    $sim_theta->set_cont($dims[0], $grid[$I]);
    for ($J=0; $J < count($grid); $J++) {
      print sprintf("\rAt grid point (%g, %g) ...\n", $grid[$I], $grid[$J]);
      $sim_theta->set_cont($dims[1], $grid[$J]);
      for ($i=0; $i < $N_EXAMINEES; $i++) {
        # "covariate" is the same OscatsCovariates object used by "e"
        $covariate->set_by_name($COVARIATE_NAME, Oscats::rnd_normal(1));
        print sprintf("\r%d", i);  flush();
        for ($j=0; $j < count($tests); $j++) {
          # Reset initial theta: Random start ~ N_2(0, I_2)
          $est_theta->set_cont($dims[0], Oscats::rnd_normal(1));
          $est_theta->set_cont($dims[1], Oscats::rnd_normal(1));
          # Administer the test
          $tests[$j]->administer($e);
          # Calculate error (Euclidean distance between sim/est theta)
          $err_sums[$j][$I*count($grid)+$J] += distance($sim_theta, $est_theta);
        }
      }
    }
  }
                                                                                                
  # Report results
  $out = fopen("ex02-results.txt", "w");
  fwrite($out, "D1\tD2\t" . join("\t", $test_names) . "\n");
  for ($I=0; $I < count($grid); $I++) {
    for ($J=0; $J < count($grid); $J++) {
      fwrite($out, sprintf("%g\t%g", $grid[$I], $grid[$J]));
      for ($j=0; $j < count($tests); $j++) {
        fwrite($out, sprintf("\t%g",
               $err_sums[$j][$I*count($grid)+$J]/$N_EXAMINEES));
      }
      fwrite($out, "\n");
    }
  }
  fclose($out);

  # No cleanup necessary, since php is garbage collected.
  print "Done.\n";
?>
