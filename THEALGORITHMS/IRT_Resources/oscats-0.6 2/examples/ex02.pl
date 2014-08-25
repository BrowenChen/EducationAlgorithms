#! perl -I../bindings/perl/blib/arch -I../bindings/perl/blib/lib

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

use Glib;
use oscats;

$N_EXAMINEES = 500;
$N_ITEMS = 500;
$LEN = 20;
$COVARIATE_NAME = "MyCovariate";

# Helper function to calculate the Euclidian distance between two points
sub distance {
  my ($a, $b) = @_;
  $d1 = $a->get_cont($dims[0]) - $b->get_cont($dims[0]);
  $d2 = $a->get_cont($dims[1]) - $b->get_cont($dims[1]);
  return sqrt($d1*$d1 + $d2*$d2);
}

# We will have to tell the models which covariate to use.  This is done
# by supplying an OscatsCovariates object that includes only that covariate
# to be included in the model.  In this case, all of our items will use
# the same covariate.  We'll create the OscatsCovariates object below when
# we create the latent space, and pass both to the gen_items() function.
sub gen_items {
  my $space = shift;
  my $covariate = shift;
  # Create an item bank to store the items.
  # Setting the property "sizeHint" increases allocation efficiency
  my $bank = new oscats::ItemBank("sizeHint", $N_ITEMS);
  for my $i (1 .. $N_ITEMS) {
    # First we create an IRT model container for our item
    # We have to specify which dimensions to be used with the "dims" array
    # (in this case, we use both of the dimensions of the space)
    my $model = new oscats::ModelL2p("space", $space, "dims", \@dims,
                                     "covariates", $covariate);
    # Then, set the parameters.  Here there are 4:
    # Discrimination on two dimensions, difficulty, and covariate coef.
    $model->set_param_by_name("Diff", oscats::Random::normal(sqrt(3)));
    $model->set_param_by_name("Discr.Cont.1",
                              oscats::Random::uniform_range(0, 1));
    $model->set_param_by_name("Discr.Cont.2",
                              oscats::Random::uniform_range(0, 2));
    $model->set_param_by_name($COVARIATE_NAME,
                              oscats::Random::uniform_range(0.5, 1.5));
    # Create an item based on this model
    # This is equivalent to oscats_item_new(0, model) in C
    my $item = new oscats::Item(0, $model);
    # Add the item to the item bank
    $bank->add_item($item)
    # Since Perl is garbage collected, we don't have to worry about
    # reference counting.
  }
  return $bank;
}

## main()

@test_names = ("A-opt", "D-opt", "KL");
@grid = (-1, 0, 1);

# Create the latent space for the test: 2 continuous dimensions
$space = new oscats::Space("numCont", 2);

# Fetch the first and second continuous dimensions via the default names
@dims = ($space->get_dim_by_name("Cont.1"), $space->get_dim_by_name("Cont.2"));

# Create the covariate
$covariate = new oscats::Covariates();
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
for $name (@test_names) {
  push @tests, new oscats::Test("id", $name, "itembank", $bank,
                                "length_hint", $LEN);
}
# Register the CAT algorithms for each test
# A test must have at minimum a selection algorithm, and administration
# algorithm, and a stoping critierion.
for $test (@tests) {
  (new oscats::AlgSimulate())->register($test);
  (new oscats::AlgEstimate("Nposterior", 5))->register($test);
  (new oscats::AlgFixedLength("len", $LEN))->register($test);
}

# Here we register the item selection criteria for the different tests
(new oscats::AlgMaxFisher("A-opt", 1, "num", 5))->register($tests[0]);
(new oscats::AlgMaxFisher("num", 5))->register($tests[1]);
(new oscats::AlgMaxKl("num", 5))->register($tests[2]);

# In Example 1, we created OscatsExaminee objects for each examinee.
# Here, we'll just use one object over again for each administration.
$e = new oscats::Examinee("covariates", $covariate);
$sim_theta = new oscats::Point("space", $space);
$est_theta = new oscats::Point("space", $space);
$e->set_sim_theta($sim_theta);
$e->set_est_theta($est_theta);

print "Administering.\n";
$| = 1;	  # Autoflush stdout
for $I (0 .. $#grid) {
  $sim_theta->set_cont($dims[0], $grid[$I]);
  for $J (0 .. $#grid) {
    print "\rAt grid point ($grid[$I], $grid[$J]) ...\n";
    $sim_theta->set_cont($dims[1], $grid[$J]);
    for $i (1 .. $N_EXAMINEES) {
      # "covariate" is the same OscatsCovariates object used by "e"
      $covariate->set_by_name($COVARIATE_NAME, oscats::Random::normal(1));
      print "\r$i";
      for $j (0 .. $#tests) {
        # Reset initial theta: Random start ~ N_2(0, I_2)
        $est_theta->set_cont($dims[0], oscats::Random::normal(1));
        $est_theta->set_cont($dims[1], oscats::Random::normal(1));
        # Administer the test
        $tests[$j]->administer($e);
        # Calculate error (Euclidean distance between sim/est theta)
        $err_sums[$j][$I][$J] += distance($sim_theta, $est_theta);
      }
    }
  }
}
                                                                                                
# Report results
open OUT, ">ex02-results.txt" or die "Can't open >ex02-results.txt: $!\n";
print OUT "D1\tD2\t" . join("\t", @test_names) . "\n";
for $I (0 .. $#grid) {
  for $J (0 .. $#grid) {
    printf OUT "%g\t%g\t", $grid[$I], $grid[$J];
    for $j (0 .. $#tests) {
      printf OUT "%g", $err_sums[$j][$I][$J]/$N_EXAMINEES;
    }
    print OUT "\n";
  }
}
close OUT;

# No cleanup necessary, since perl is garbage collected.
print "Done.\n";

