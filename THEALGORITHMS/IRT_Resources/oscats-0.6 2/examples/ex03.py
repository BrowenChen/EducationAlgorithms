#! python 

# OSCATS: Open-Source Computerized Adaptive Testing System
# Copyright 2011 Michael Culbertson <culbert1@illinois.edu>
#
# Example 3
# 300 Items: 3PL and DINA, read from file "ex03-items.dat"
# 200 Examinees: Read from file "ex03-person.dat"
# Simulate based on DINA (8 attributes).
# Initial estimate: theta ~ N(-.37, 0.05), attributes ~ Bernoulli(0.5)
# Item selection (choose from among 5):
#  - pick randomly
#  - fisher information
#  - KL IRT information
#  - KL CD information
# Test length: 30
# Report:
#  - MSE(theta.hat)
#  - Correct pattern rate
#  - Correct attribute rate
#  - Number of incorrect attributes/person
#  - Item exposure

# Note, gobject *must* be imported before oscats
import gobject, oscats
from math import sqrt
from warnings import warn

N_EXAMINEES = 200
N_ITEMS = 300
N_ATTR = 8
LEN = 30

def read_items(f, contSpace, binSpace) :
  # Create an item bank to store the items.
  # Setting the property "sizeHint" increases allocation efficiency
  bank = gobject.new(oscats.ItemBank, sizeHint=N_ITEMS)
  f.readline()		# header line
  count = 0
  for line in f.readlines() :
    (a, b, c, slip, guess, attrStr) = line.split("\t")
    # Create the item
    count += 1
    item = gobject.new(oscats.Item, id=str(count))
    # Create the 3PL model
    model = gobject.new(oscats.ModelL3p, space=contSpace)
    model.set_param_by_name("Discr.Cont.1", float(a))
    # The difficulty parameter in the input file is specified as the
    # traditional IRT difficulty: logit P(x) = a(theta-b); but,
    # OscatsModelL3p uses the parameterization: logit P(x) = a*theta - b,
    # so we have to multiply here, accordingly.
    model.set_param_by_name("Diff", float(a)*float(b))
    model.set_param_by_name("Guess", float(c))
    # Set the model
    item.set_model_by_name("3PL", model)
    # Create the DINA model
    dim = binSpace.get_dim_by_name("Bin.1") + attrStr.find('1')
    model = gobject.new(oscats.ModelDina, space=binSpace, dims=[dim])
    model.set_param_by_name("Slip", float(slip))
    model.set_param_by_name("Guess", float(guess))
    # Set the model
    item.set_model_by_name("DINA", model)
    # Add the item to the item bank
    bank.add_item(item)
  if (count != N_ITEMS) :
    warn("Expected %d items, but only read %d." % (N_ITEMS, count))
  return bank

# Returns a list of new OscatsExaminee objects
def read_examinees(f, contSpace, binSpace) :
  count = 0
  ret = []
  for line in f.readlines() :
    (theta, attrStr) = line.split("\t")
    theta = float(theta)
    attrStr = attrStr.strip()
    # Create the examinee
    count += 1
    e = gobject.new(oscats.Examinee, id=str(count))
    # Set "true" continuous theta
    dim = contSpace.get_dim_by_name("Cont.1")
    point = oscats.Point(space=contSpace)
    point.set_cont(dim, theta)
    e.set_theta_by_name("trueTheta", point)
    # Set "true" binary alpha
    dim = binSpace.get_dim_by_name("Bin.1")
    point = oscats.Point(space=binSpace)
    for k in range(N_ATTR) :
      point.set_bin(dim, attrStr[k] == '1') ; dim += 1
    e.set_theta_by_name("trueAlpha", point)
    ret.append(e)
  if (count != N_EXAMINEES) :
    warn("Expected %d examinees, but read in %d." % (N_EXAMINEES, count))
  return ret

## main()

test_names = ("random", "FI", "KL.3PL", "KL.DINA")

# In the previous examples, we had only one latent space; but, here we
# want to use two different models from *different* latent spaces.  The
# first latent space is a unidimensional continuous space (for the 3PL
# model); the second is an N_ATTR-dimensional binary space (for the DINA
# model).  If appropriate to the simulation (e.g. to compare 2PL vs. 3PL)
# we could have created two different models from the same space.

# Note: Even though OscatsSpace accommodates mixed-type (continuous and
# binary) latent spaces, the models we use here are either only
# continuous or only binary, so they come from different latent spaces. 
# A mixed-type model would be something like the Fusion model, which has
# both binary and continuous components.
contSpace = oscats.Space(numCont=1)
binSpace = oscats.Space(numBin=N_ATTR)
initTheta = oscats.Point(space=contSpace)
initAlpha = oscats.Point(space=binSpace)

print "Reading examinees."
examinees = read_examinees(open("ex03-person.dat", 'r'), contSpace, binSpace)
print "Reading items."
bank = read_items(open("ex03-items.dat", 'r'), contSpace, binSpace)

print "Creating tests."
# Create new tests with the given properties
tests = [ gobject.new(oscats.Test, id=name, itembank=bank,
                      length_hint=LEN)   for name in test_names ]
exposures = []
rates = []
# Register the common CAT algorithms for each test
for test in tests :
  # Here, we always simulate from the DINA model
  oscats.AlgSimulate(modelKey="DINA", thetaKey="trueAlpha").register(test)
  # Usually we would only have one estimate algorithm, but here, we need
  # to estimate both the continuous ability and the latent class, each
  # based on a separate model.
  oscats.AlgEstimate(modelKey="3PL", thetaKey="estTheta").register(test)
  oscats.AlgEstimate(modelKey="DINA", thetaKey="estAlpha").register(test)

  exposures.append( oscats.AlgExposureCounter().register(test) )
  rates.append(oscats.AlgClassRates(simKey="trueAlpha",
                                    estKey="estAlpha").register(test) )
  
  oscats.AlgFixedLength(len=LEN).register(test)

# Register the item selection criteria for the different tests:
# Since we aren't just using the default model for everything, we have to
# tell each algorithm which model/theta to use for selection.
oscats.AlgPickRand().register(tests[0])
oscats.AlgMaxFisher(modelKey="3PL", thetaKey="estTheta",
                    num=5).register(tests[1])
oscats.AlgMaxKl(modelKey="3PL", thetaKey="estTheta", num=5).register(tests[2])
oscats.AlgMaxKl(modelKey="DINA", thetaKey="estAlpha", num=5).register(tests[2])

print "Administering."
out = open("ex03-person-results.dat", "w")
out.write("ID\ttrue.theta\ttrue.alpha\tinit.theta\tinit.alpha" +
          "\t".join([ "%s.theta\t%s.alpha" for name in test_names]) + "\n")
for e in examinees :
  tmp = e.get_theta_by_name("trueTheta")
  dim = contSpace.get_dim_by_name("Cont.1")
  out.write("%s\t%g\t" % (e.get_property("id"), tmp.get_cont(dim)))
  tmp = e.get_theta_by_name("trueAlpha")
  dim = binSpace.get_dim_by_name("Bin.1")
  for k in range(N_ATTR) :
    if tmp.get_bin(dim) : out.write("1")
    else                : out.write("0")
    dim += 1
  
  # Choose initial theta and alpha for this examinee
  # We'll use the same initial abilities for all four tests
  dim = contSpace.get_dim_by_name("Cont.1")
  initTheta.set_cont(dim, oscats.oscats_rnd_normal(sqrt(0.05))-0.37)
  dim = binSpace.get_dim_by_name("Bin.1")
  for k in range(N_ATTR) :
    initAlpha.set_bin(dim, oscats.oscats_rnd_uniform() < 0.5) ; dim += 1
  
  # Initialize ability estimates for this examinee
  e.set_theta_by_name("estTheta", oscats.Point(space=contSpace))
  e.set_theta_by_name("estAlpha", oscats.Point(space=binSpace))

  for test in tests :
    # Reset initial latent ability for this test
    e.get_theta_by_name("estTheta").copy(initTheta)
    e.get_theta_by_name("estAlpha").copy(initAlpha)
    
    # Do the administration!
    test.administer(e)
    
    # Output the resulting theta.hat and alpha.hat
    tmp = e.get_theta_by_name("estTheta")
    dim = contSpace.get_dim_by_name("Cont.1")
    out.write("\t%g\t" % tmp.get_cont(dim))
    tmp = e.get_theta_by_name("estAlpha")
    dim = binSpace.get_dim_by_name("Bin.1")
    for k in range(N_ATTR) :
      if tmp.get_bin(dim) : out.write("1")
      else                : out.write("0")
      dim += 1
  out.write("\n")
out.close()

out = open("ex03-exposure.dat", "w")
out.write("ID\t" + "\t".join(test_names) + "\n")
for i in range(N_ITEMS) :
  item = bank.get_item(i)
  out.write(item.get_property("id") + "\t" +
            "\t".join([ "%g" % exp.get_rate(item) for exp in exposures]) + "\n")
out.close()

out = open("ex03-rates.dat", "w")
out.write("Rate\t" + "\t".join(test_names) + "\n")
out.write("Pattern\t" +
          "\t".join(["%g" % rate.get_pattern_rate() for rate in rates]) + "\n")
for k in range(N_ATTR) :
  out.write(("attr.%d\t" % (k+1)) +
            "\t".join(["%g" % rate.get_attribute_rate(k) for rate in rates]) +
            "\n")
for k in range(N_ATTR+1) :
  out.write(("freq.%d\t" % k) +
            "\t".join(["%g" % rate.get_misclassify_freq(k) for rate in rates]) +
            "\n")
out.close()

# No cleanup necessary, since python is garbage collected.
print "Done."

