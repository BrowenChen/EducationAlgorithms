#! perl -I../bindings/perl/blib/arch -I../bindings/perl/blib/lib

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

use Glib;
use oscats;

$N_EXAMINEES = 1000;
$N_ITEMS = 400;
$LEN = 30;

sub gen_items {
  my $space = shift;
  # Create an item bank to store the items.
  # Setting the property "sizeHint" increases allocation efficiency
  my $bank = new oscats::ItemBank("sizeHint", $N_ITEMS);
  for my $i (1 .. $N_ITEMS) {
    # First we create an IRT model container for our item
    my $model = new oscats::ModelL1p("space", $space);
    # Then, set the parameters.  Here there is only one, the difficulty (b).
    $model->set_param_by_index(0, oscats::Random::normal(1));
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

# Returns a list of new OscatsExaminee objects
sub gen_examinees {
  my $space = shift;
  my $dim = $space->get_dim_by_name("Cont.1");
  my @ret;
  for my $i (1 .. $N_EXAMINEES) {
    # Latent IRT ability parameter.  This is a one-dimensional test.
    my $theta = new oscats::Point("space", $space);
    # Sample the ability from N(0,1) distribution
    $theta->set_cont($dim, oscats::Random::normal(1));
    # Create a new examinee
    my $e = new oscats::Examinee();
    # Set the examinee's true (simulated) ability
    $e->set_sim_theta($theta);
    push @ret, $e;
  }
  return @ret;
}

## main()

@test_names = ("random", "matched", "match.5", "match.10");

# Create the latent space for the test: continuous unidimensional
$space = new oscats::Space("numCont", 1);
$dim = $space->get_dim_by_name("Cont.1");

print "Creating examinees.\n";
@examinees = gen_examinees($space);
print "Creating items.\n";
$bank = gen_items($space);

print "Creating tests.\n";
# Create new tests with the given properties
  # In this case, we know what the length of the test will be (30),
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
  (new oscats::AlgEstimate())->register($test);
  
  # In many cases, we don't care about the algorithm objects after they've
  # been registered, since they don't contain any interesting information. 
  # But, for the item exposure counter, we want to have access to the item
  # exposure rates when the test is over, so we keep the object.
  push @exposures,
    (new oscats::AlgExposureCounter())->register($test);
  
  (new oscats::AlgFixedLength("len", $LEN))->register($test);
}

# Here we register the item selection criteria for the different tests
(new oscats::AlgPickRand())->register($tests[0]);
# The default for OscatsAlgClosestDiff is to pick the exact closest item
(new oscats::AlgClosestDiff())->register($tests[1]);
# But, we can have the algorithm choose randomly from among the num closest
(new oscats::AlgClosestDiff("num", 5))->register($tests[2]);
(new oscats::AlgClosestDiff("num", 10))->register($tests[3]);

print "Administering.\n";
open OUT, ">ex01-examinees.dat" or die "Can't open >ex01-examinees.dat: $!\n";
print OUT "ID\ttrue\t" . join("\t", @test_names) . "\n";
$i = 1;
for $e (@examinees) {
  # An initial estimate for latent IRT ability must be provided.
  $e->set_est_theta(new oscats::Point("space", $space));

  printf OUT "%d\t%g", $i, $e->get_sim_theta()->get_cont($dim);
  for $test (@tests) {
    # Reset initial latent ability for this test
    $e->get_est_theta()->set_cont($dim, 0);
    
    # Do the administration!
    $test->administer($e);
    
    # Output the resulting theta.hat and its *variance*
    printf OUT "\t%g", $e->get_est_theta()->get_cont($dim);
  }
  print OUT "\n";
  $i += 1;
}
close OUT;

open OUT, ">ex01-items.dat" or die "Can't open >ex01-items.dat: $!\n";
print OUT "ID\tb\t" . join("\t", @test_names) . "\n";
for $i (1 .. $N_ITEMS) {
  $item = $bank->get_item($i-1);
  # Get item's difficulty paramter
  printf OUT "%d\t%g", $i, $item->get_model($item->get_default_model())->get_param_by_index(0);
  for $exposure (@exposures) {
    printf OUT "\t%g", $exposure->get_rate($item);
  }
  print OUT "\n";
}
close OUT;

# No cleanup necessary, since perl is garbage collected.
print "Done.\n";

