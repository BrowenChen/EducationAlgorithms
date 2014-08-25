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

import oscats.Space;
import oscats.Point;
import oscats.ModelL1p;
import oscats.ItemBank;
import oscats.Item;
import oscats.Examinee;
import oscats.Test;
import oscats.AlgExposureCounter;

import java.io.FileOutputStream;
import java.util.Formatter;

public class ex01 {

  static final int N_EXAMINEES = 1000;
  static final int N_ITEMS = 400;
  static final int LEN = 30;

  static ItemBank gen_items(Space space)
  {
    // Create an item bank to store the items.
    // Setting the property "sizeHint" increases allocation efficiency.
    ItemBank bank = new ItemBank(N_ITEMS);
    for (int i=0; i < N_ITEMS; i++)
    {
      // First we create an IRT model container for our item
      // Defaults to unidimensional, using the first dimension of space
      ModelL1p model = new ModelL1p(space);
      // Then, set the parameters.  Here there is only one, the difficulty (b).
      model.setParamByIndex(0, oscats.Random.normal(1));
      // Create an item based on this model (as default model)
      Item item = new Item(model);
      // Add the item to the item bank
      bank.addItem(item);
      // Since Java is garbage collected, we don't have to worry about
      // reference counting.
    }
    return bank;
  }

  // Returns an array of new OscatsExaminee pointers
  static Examinee[] gen_examinees(Space space)
  {
    Examinee[] ret = new Examinee[N_EXAMINEES];
    char dim = Space.OSCATS_DIM_CONT + 0;  // First continuous dimension
    
    for (int i=0; i < N_EXAMINEES; i++)
    {
      // Latent IRT ability parameter.  This is a one-dimensional test.
      Point theta = new Point(space);
      // Sample the ability from N(0,1) distribution
      theta.setCont(dim, oscats.Random.normal(1));
      // Create a new examinee
      ret[i] = new Examinee();
      // Set the examinee's true (simulated) ability
      ret[i].setSimTheta(theta);
    }
    
    return ret;
  }

  public static void main(String[] args) throws java.io.FileNotFoundException, java.io.IOException
  {
    final int num_tests = 4;
    final String[] test_names = { "random", "matched", "match.5", "match.10" };
    Test[] test = new Test[num_tests];
    AlgExposureCounter[] exposure = new AlgExposureCounter[num_tests];
    int i, j;
    
    // Create the latent space for the test: continuous unidimensional
    Space space = new Space(1, 0);
    char dim = Space.OSCATS_DIM_CONT + 0;  // First continuous dimension
    
    System.out.println("Creating examinees.");
    Examinee[] examinees = gen_examinees(space);
    System.out.println("Creating items.");
    ItemBank bank = gen_items(space);

    System.out.println("Creating tests.");
    for (j=0; j < num_tests; j++)
    {
      // Create a new test with the given properties

      // In this case, we know what the length of the test will be (30),
      // so, we set length_hint for allocation efficiency.
      // Any good/reasonable guess will do---the size will be adjusted
      // automatically as necessary.
      test[j] = new Test(test_names[j], bank, LEN);

      // Register the CAT algorithms for this test.
      // A test must have at minimum a selection algorithm, and administration
      // algorithm, and a stoping critierion.

      new oscats.AlgSimulate().register(test[j]);
      new oscats.AlgEstimate().register(test[j]);

      // All calls to oscats.Algorithm.register() return an algorithm
      // data object.  In many cases, we don't care about this object, since
      // it doesn't contain any interesting information.  But, for the
      // item exposure counter, we want to have access to the item exposure
      // rates when the test is over, so we keep the object.
      exposure[j] = new oscats.AlgExposureCounter();
      exposure[j].register(test[j]);

      oscats.AlgFixedLength.createAlgFixedLength(LEN).register(test[j]);
    }
    
    // Here we register the item selection criteria for the different tests
    new oscats.AlgPickRand().register(test[0]);
    // The default for OscatsAlgClosestDiff is to pick the exact closest item
    new oscats.AlgClosestDiff().register(test[1]);
    // But, we can have the algorithm choose randomly from among the num closest
    oscats.AlgClosestDiff.createAlgClosestDiff(5).register(test[2]);
    oscats.AlgClosestDiff.createAlgClosestDiff(10).register(test[3]);
    
    System.out.println("Administering.");
    FileOutputStream fos = new FileOutputStream("ex01-examinees.dat");
    Formatter f = new Formatter(fos);
    f.format("ID\ttrue");
    for (j=0; j < num_tests; j++)
      f.format("\t%s", test_names[j]);
    f.format("\n");
    for (i=0; i < N_EXAMINEES; i++)
    {
  //    printf("  %s\n", examinees[i]->id);

      // An initial estimate for latent IRT ability must be provided.
      examinees[i].setEstTheta(new Point(space));

      f.format("%d\t%g", i+1, examinees[i].getSimTheta().getCont(dim));
      for (j=0; j < num_tests; j++)
      {
        // Reset initial latent ability for this test
        examinees[i].getEstTheta().setCont(dim, 0);
        // Do the administration!
        test[j].administer(examinees[i]);
        // Output the resulting theta.hat
        f.format("\t%g", examinees[i].getEstTheta().getCont(dim));
      }
      f.format("\n");
    }
    fos.close();
    
    fos = new FileOutputStream("ex01-items.dat");
    f = new Formatter(fos);
    f.format("ID\tb");
    for (j=0; j < num_tests; j++)
      f.format("\t%s", test_names[j]);
    f.format("\n");
    for (i=0; i < N_ITEMS; i++)
    {
      // Get the item's difficulty parameter
      f.format("%d\t%g", i+1, bank.getItem(i).getModel().getParamByIndex(0));
      // Get the exposure rate for this item in each test
      for (j=0; j < num_tests; j++)
        f.format("\t%g", exposure[j].getRate((Item)bank.getItem(i)) );
      f.format("\n");
    }
    fos.close();

    // No clean up necessary, since Java is garbage collected.
    System.out.println("Done.");
  }

}
