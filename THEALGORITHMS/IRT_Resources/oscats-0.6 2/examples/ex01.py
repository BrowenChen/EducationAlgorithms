#! python 

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

# Note, gobject *must* be imported before oscats
import gobject, oscats

N_EXAMINEES = 1000
N_ITEMS = 400
LEN = 30

def gen_items(space) :
  # Create an item bank to store the items.
  # Setting the property "sizeHint" increases allocation efficiency
  bank = gobject.new(oscats.ItemBank, sizeHint=N_ITEMS)
  for i in range(N_ITEMS) :
    # First we create an IRT model container for our item
    # Defaults to unidimensional, using the first dimension of space
    model = gobject.new(oscats.ModelL1p, space=space)
    # Then, set the parameters.  Here there is only one, the difficulty (b).
    model.set_param_by_index(0, oscats.oscats_rnd_normal(1))
    # Create an item based on this model
    item = gobject.new(oscats.Item)
    item.set_model(item.get_default_model(), model)
    # Add the item to the item bank
    bank.add_item(item)
    # Since Python is garbage collected, we don't have to worry about
    # reference counting.
  return bank

# Returns a list of new OscatsExaminee objects
def gen_examinees(space) :
  dim = space.get_dim_by_name("Cont.1")
  ret = []
  for i in range(N_EXAMINEES) :
    # Latent IRT ability parameter.  This is a one-dimensional test.
    theta = oscats.Point(space=space)
    # Sample the ability from N(0,1) distribution
    theta.set_cont(dim, oscats.oscats_rnd_normal(1))
    # Create a new examinee
    e = gobject.new(oscats.Examinee)
    # Set the examinee's true (simulated) ability
    e.set_sim_theta(theta)
    ret.append(e)
  return ret

## main()

test_names = ("random", "matched", "match.5", "match.10")

# Create the latent space for the test: continuous unidimensional
space = oscats.Space(numCont=1)
dim = space.get_dim_by_name("Cont.1")

print "Creating examinees."
examinees = gen_examinees(space)
print "Creating items."
bank = gen_items(space)

print "Creating tests."
# Create new tests with the given properties
  # In this case, we know what the length of the test will be (30),
  # so, we set length_hint for allocation efficiency.
  # Any good/reasonable guess will do---the size will be adjusted
  # automatically as necessary.
tests = [ gobject.new(oscats.Test, id=name, itembank=bank,
                      length_hint=LEN)   for name in test_names ]
exposures = []
# Register the CAT algorithms for each test
# A test must have at minimum a selection algorithm, and administration
# algorithm, and a stoping critierion.
for test in tests :
  oscats.AlgSimulate().register(test)
  oscats.AlgEstimate().register(test)
  
  # In many cases, we don't care about the algorithm objects after they've
  # been registered, since they don't contain any interesting information. 
  # But, for the item exposure counter, we want to have access to the item
  # exposure rates when the test is over, so we keep the object.
  exposures.append( oscats.AlgExposureCounter().register(test) )
  
  oscats.AlgFixedLength(len=LEN).register(test)

# Here we register the item selection criteria for the different tests
oscats.AlgPickRand().register(tests[0])
# The default for OscatsAlgClosestDiff is to pick the exact closest item
oscats.AlgClosestDiff().register(tests[1])
# But, we can have the algorithm choose randomly from among the num closest
oscats.AlgClosestDiff(num=5).register(tests[2])
oscats.AlgClosestDiff(num=10).register(tests[3])

print "Administering."
out = open("ex01-examinees.dat", "w")
out.write("ID\ttrue\t" + "\t".join(test_names) + "\n")
i = 1
for e in examinees :
  # An initial estimate for latent IRT ability must be provided.
  e.set_est_theta(oscats.Point(space=space))

  out.write("%d\t%g" % (i, e.get_sim_theta().get_cont(dim) ))
  for test in tests :
    # Reset initial latent ability for this test
    e.get_est_theta().set_cont(dim, 0)
    
    # Do the administration!
    test.administer(e)
    
    # Output the resulting theta.hat
    out.write("\t%g" % e.get_est_theta().get_cont(dim) )
  out.write("\n")
  i += 1
out.close()

out = open("ex01-items.dat", "w")
out.write("ID\tb\t" + "\t".join(test_names) + "\n")
for i in range(N_ITEMS) :
  item = bank.get_item(i)
  # Get item's difficulty paramter
  out.write("%d\t%g" % (i+1,
    item.get_model(item.get_default_model()).get_param_by_index(0) ))
  for exposure in exposures :
    out.write("\t%g" % exposure.get_rate(item))
  out.write("\n")
out.close()

# No cleanup necessary, since python is garbage collected.
print "Done."

