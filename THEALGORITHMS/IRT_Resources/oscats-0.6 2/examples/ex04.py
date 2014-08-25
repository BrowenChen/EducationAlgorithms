#! python

# OSCATS: Open-Source Computerized Adaptive Testing System
# Copyright 2011 Michael Culbertson <culbert1@illinois.edu>
#
# Example 4
#
# Calculate average bias for examinees who get first 3 items all correct
# or incorrect under:  Max FI, KLI, and a-stratification
# 500 items, 3PL, a ~ Unif(0.5, 2.5), b ~ Norm(0, 1), c ~ Unif(0.2, 0.4)
# 10,000 examinees ~ Norm(0,1)
# 15 item test

prefix = 'ex04'
N_EXAMINEES = 10000
N_ITEMS = 1000
LEN = 20

import gobject, oscats
from math import sqrt

nullCovariates = oscats.Covariates()
class CustomSimulateAlg (oscats.PyAlgorithm) :
  num = gobject.property(type=int, default=2)
  high = gobject.property(type=bool, default=True)
  
  def administer(self, examinee, item):
    model = item.get_model(0)
    if (examinee.num_items() < self.num) :
      if (self.high) : resp = 1
      else : resp = 0
    else :
      if (oscats.oscats_rnd_uniform() <
          model.P(1, examinee.get_sim_theta(), nullCovariates)) :
        resp = 1
      else : resp = 0
    examinee.add_item(item, resp)
    return resp

  def __reg__ (self, test) :
    test.connect_object("administer", CustomSimulateAlg.administer, self)

class CustomBiasAlg (oscats.PyAlgorithm) :
  def finalize(self, examinee):
    theta = examinee.get_sim_theta()
    theta_hat = examinee.get_est_theta()
    dim = theta.get_property("space").get_dim_by_name("Cont.1")
    bias = theta_hat.get_double(dim) - theta.get_double(dim)
    if theta.get_double(dim) < 0 :
      self.sum_low += bias ; self.count_low += 1
    else :
      self.sum_high += bias ; self.count_high += 1

  def getResult(self) :
    if self.count_low > 0 : low = self.sum_low/self.count_low
    else : low = 1e500   # inf
    if self.count_high > 0 : high = self.sum_high/self.count_high
    else : high = 1e500   # inf
    return (low, high)

  def __reg__ (self, test) :
    self.count_low = 0
    self.sum_low = 0
    self.count_high = 0
    self.sum_high = 0
    test.connect_object("finalize", CustomBiasAlg.finalize, self)

def gen_items(space) :
  bank = oscats.ItemBank(sizeHint=N_ITEMS)
  for i in range(N_ITEMS) :
    model = gobject.new(oscats.ModelL3p, space=space)
    model.set_param_by_name("Diff", oscats.oscats_rnd_uniform_range(-3, 3))
    model.set_param_by_name("Discr.Cont.1",
                            oscats.oscats_rnd_uniform_range(0.8, 2.0))
    model.set_param_by_name("Guess", oscats.oscats_rnd_uniform_range(0.1, 0.3))
    item = oscats.Item(0, model)
    bank.add_item(item)
  return bank

def gen_examinees(space) :
  dim = space.get_dim_by_name("Cont.1")
  ret = []
  for i in range(N_EXAMINEES) :
    theta = oscats.Point(space=space)
    theta.set_cont(dim, oscats.oscats_rnd_normal(1))
    e = oscats.Examinee()
    e.set_sim_theta(theta)
    ret.append(e)
  return ret

## main()

test_names = ("MFI", "KLI", "aStrat")

space = oscats.Space(numCont=1)
dim = space.get_dim_by_name("Cont.1")

print "Creating examinees."
examinees = gen_examinees(space)
examinees.sort(lambda a, b :
  cmp(a.get_sim_theta().get_cont(dim), b.get_sim_theta().get_cont(dim)) )
examinees_high = examinees[int(N_EXAMINEES*0.9):N_EXAMINEES]
examinees_low = examinees[0:int(N_EXAMINEES*0.1)]
examinees = examinees_low + examinees_high
print "Creating item bank."
bank = gen_items(space)

print "Creating tests."
tests_high = [ oscats.Test(id=name+' high', itembank=bank, length_hint=LEN)
                 for name in test_names ]
tests_low  = [ oscats.Test(id=name+' low', itembank=bank, length_hint=LEN)
                 for name in test_names ]
results_low = []
results_high = []
for test in tests_high :
  CustomSimulateAlg(high=True).register(test)
  oscats.AlgEstimate(posterior=True).register(test)
  oscats.AlgFixedLength(len=LEN).register(test)
  results_high.append( CustomBiasAlg().register(test) )
for test in tests_low :
  CustomSimulateAlg(high=False).register(test)
  oscats.AlgEstimate(posterior=True).register(test)
  oscats.AlgFixedLength(len=LEN).register(test)
  results_low.append( CustomBiasAlg().register(test) )

oscats.AlgMaxFisher(num=5).register(tests_high[0])
oscats.AlgMaxKl(num=5).register(tests_high[1])
oscats.AlgAstrat(Nstrata=5, Nequal=4).register(tests_high[2])
oscats.AlgClosestDiff(num=5).register(tests_high[2])

oscats.AlgMaxFisher(num=5).register(tests_low[0])
oscats.AlgMaxKl(num=5).register(tests_low[1])
oscats.AlgAstrat(Nstrata=5, Nequal=4).register(tests_low[2])
oscats.AlgClosestDiff(num=5).register(tests_low[2])

from sys import stdout
print "Administering."
i = 0
for e in examinees :
  if i % 10 == 0 : print "\r%d" % i,
  stdout.flush()
  e.set_est_theta(oscats.Point(space=space))
  for test in tests_low + tests_high :
    e.get_est_theta().set_cont(dim, 0)
    test.administer(e)
  i += 1

print "Done."

out = open(prefix+'-results.txt', 'w')
out.write("Test\tLow\tHigh\n")
for i in range(len(test_names)) :
  out.write(test_names[i] + " low\t"
            + '\t'.join([ '%g' % x for x in results_low[i].getResult() ])
            + "\n")
  out.write(test_names[i] + " high\t"
            + '\t'.join([ '%g' % x for x in results_high[i].getResult() ])
            + "\n")
out.close()
