<?
# OSCATS: Open-Source Computerized Adaptive Testing System
# Copyright 2010, 2011 Michael Culbertson <culbert1@illinois.edu>
#
# Example 1
# 400 Items: 1PL, b ~ N(0,1)
# 1000 Examinees: theta ~ N(0,1)
# Item selection:
#  - pick randomly
#  - match theta with b, exactly
#  - match theta with b, randomize 5, 10 items
# Test length: 30
# Report:
#  - theta.hat, Item exposure

  # You *must* load the php_gtk2 extension *before* the OSCATS extension
  # This may be done dynamically (as here), or in php.ini
  if (!class_exists('gtk'))
    dl('php_gtk2.so');
  if (!class_exists('Oscats'))
    dl('php_oscats.so');

  $N_EXAMINEES = 1000;
  $N_ITEMS = 400;
  $LEN = 30;

  function gen_items($space) {
    global $N_ITEMS;
    # Create an item bank to store the items.
    # Setting the property "sizeHint" increases allocation efficiency

    # It's a bit kludgy, but the php gtk bindings require passing
    # the name of the object to the objects constructor in order to
    # set the construction properties.
    $bank = new OscatsItemBank('OscatsItemBank', array("sizeHint" => $N_ITEMS));
    for ($i=0; $i < $N_ITEMS; $i++) {
      # First we create an IRT model container for our item
      $model = new OscatsModelL1p('OscatsModelL1p', array("space" => $space));
      # Then, set the parameters.  Here there is only one, the difficulty (b).
      $model->set_param_by_index(0, Oscats::rnd_normal(1));
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

  # Returns a list of new OscatsExaminee objects
  function gen_examinees ($space) {
    global $N_EXAMINEES;
    $dim = $space->get_dim_by_name("Cont.1");
    $ret = array();
    for ($i=0; $i < $N_EXAMINEES; $i++) {
      # Latent IRT ability parameter.  This is a one-dimensional test.
      $theta = new OscatsPoint('OscatsPoint', array("space" => $space));
      # Sample the ability from N(0,1) distribution
      $theta->set_cont($dim, Oscats::rnd_normal(1));
      # Create a new examinee
      $e = new OscatsExaminee();
      # Set the examinee's true (simulated) ability
      $e->set_sim_theta($theta);
      $ret[] = $e;
    }
    return $ret;
  }

  ## main()

  $test_names = array("random", "matched", "match.5", "match.10");

  # Create the latent space for the test: continuous unidimensional
  $space = new OscatsSpace('OscatsSpace', array("numCont" => 1));
  $dim = $space->get_dim_by_name("Cont.1");

  print "Creating examinees.\n";
  $examinees = gen_examinees($space);
  print "Creating items.\n";
  $bank = gen_items($space);

  print "Creating tests.\n";
  # Create new tests with the given properties
    # In this case, we know what the length of the test will be (30),
    # so, we set length_hint for allocation efficiency.
    # Any good/reasonable guess will do---the size will be adjusted
    # automatically as necessary.
  $tests = array();
  foreach ($test_names as $name) {
    $tests[] = new OscatsTest('OscatsTest', array("id" => $name,
                              "itembank" => $bank, "length_hint" => $LEN));
  }
  # Register the CAT algorithms for each test
  # A test must have at minimum a selection algorithm, and administration
  # algorithm, and a stoping critierion.
  $exposures = array();
  foreach ($tests as $test) {
    $alg = new OscatsAlgSimulate();
    $alg->register($test);
    $alg = new OscatsAlgEstimate();
    $alg->register($test);
    
    # All calls to oscats_algorithm_register() return an algorithm
    # data object.  In many cases, we don't care about this object, since
    # it doesn't contain any interesting information.  But, for the
    # item exposure counter, we want to have access to the item exposure
    # rates when the test is over, so we keep the object.
    $alg = new OscatsAlgExposureCounter();
    $alg->register($test);
    $exposures[] = $alg;
    
    $alg = new OscatsAlgFixedLength('OscatsAlgFixedLength',
                                    array("len" => $LEN));
    $alg->register($test);
  }

  # Here we register the item selection criteria for the different tests
  $alg = new OscatsAlgPickRand();
  $alg->register($tests[0]);
  # The default for OscatsAlgClosestDiff is to pick the exact closest item
  $alg = new OscatsAlgClosestDiff();
  $alg->register($tests[1]);
  # But, we can have the algorithm choose randomly from among the num closest
  $alg = new OscatsAlgClosestDiff('OscatsAlgClosestDiff', array("num" => 5));
  $alg->register($tests[2]);
  $alg = new OscatsAlgClosestDiff('OscatsAlgClosestDiff', array("num" => 10));
  $alg->register($tests[3]);

  print "Administering.\n";
  $out = fopen("ex01-examinees.dat", "w");
  fwrite($out, "ID\ttrue\t" . join("\t", $test_names) . "\n");
  $i = 1;
  foreach ($examinees as $e) {
    # An initial estimate for latent IRT ability must be provided.
    $e->set_est_theta(new OscatsPoint('OscatsPoint', array("space" => $space)));
    
    fwrite($out, sprintf("%d\t%g", $i, $e->get_sim_theta()->get_cont($dim)));
    foreach ($tests as $test) {
      # Reset initial latent ability for this test
      $e->get_est_theta()->set_cont($dim, 0);
              
      # Do the administration!
      $test->administer($e);
      
      # Output the resulting theta.hat and its *variance*
      fwrite($out, sprintf("\t%g", $e->get_est_theta()->get_cont($dim)) );
    }
    fwrite($out, "\n");
    $i += 1;
  }
  fclose($out);

  $out = fopen("ex01-items.dat", "w");
  fwrite($out, "ID\tb\t" . join("\t", $test_names) . "\n");
  for($i=0; $i < $N_ITEMS; $i++) {
    $item = $bank->get_item($i);
    # Get item's difficulty paramter
    fwrite($out, sprintf("%d\t%g", $i+1,
      $item->get_model($item->get_default_model())->get_param_by_index(0)) );
    foreach ($exposures as $exposure) {
      fwrite($out, sprintf("\t%g", $exposure->get_rate($item)) );
    }
    fwrite($out, "\n");
  }
  fclose($out);

  # No cleanup necessary, since php is garbage collected.
  print "Done.\n";
?>
