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
           
import oscats.Space;
import oscats.Point;
import oscats.ModelL2p;
import oscats.ItemBank;
import oscats.Item;
import oscats.Examinee;
import oscats.Covariates;
import oscats.Test;

import java.lang.Math;
import java.io.FileOutputStream;
import java.util.Formatter;

public class ex02 {

  static final int N_EXAMINEES = 2;
  static final int N_ITEMS = 500;
  static final int LEN = 20;
  static final String COVARIATE_NAME = "MyCovariate";
  static char[] dims;

  // Helper function to calculate the Euclidian distance between two points
  static double distance(Point a, Point b)
  {
    double d1 = a.getCont(dims[0]) - b.getCont(dims[0]);
    double d2 = a.getCont(dims[1]) - b.getCont(dims[1]);
    return Math.sqrt(d1*d1 + d2*d2);
  }

  /* We will have to tell the models which covariate to use.  This is done
   * by supplying an OscatsCovariates object that includes only that covariate
   * to be included in the model.  In this case, all of our items will use
   * the same covariate.  We'll create the OscatsCovariates object below when
   * we create the latent space, and pass both to the gen_items() function.
   */
  static ItemBank gen_items(Space space, Covariates covariate)
  {
    // Create an item bank to store the items.
    // Setting the property "sizeHint" increases allocation efficiency.
    ItemBank bank = new ItemBank(N_ITEMS);
    // Create the items
    for (int i=0; i < N_ITEMS; i++)
    {
      // First we create an IRT model container for our item
      // We have to specify which dimensions to be used with the "dims" array
      // (in this case, we use both of the dimensions of the space)
      ModelL2p model = new ModelL2p(space, dims, covariate);
      // Then, set the parameters.  Here there are 4:
      // Discrimination on two dimensions, difficulty, and covariate coef.
      model.setParamByName("Diff", oscats.Random.normal(Math.sqrt(3)));
      model.setParamByName("Discr.Cont.1", oscats.Random.uniformRange(0, 1));
      model.setParamByName("Discr.Cont.2", oscats.Random.uniformRange(0, 2));
      model.setParamByName(COVARIATE_NAME, oscats.Random.uniformRange(0.5, 1.5));
      // Create an item based on this model (as default model)
      Item item = new Item(model);
      // Add the item to the item bank
      bank.addItem(item);
      // Since Java is garbage collected, we don't have to worry about
      // reference counting.
    }
    return bank;
  }

  public static void main(String[] args) throws java.io.FileNotFoundException, java.io.IOException
  {
    final int num_tests = 3, num_grid = 3;
    final String[] test_names = { "A-opt", "D-opt", "KL" };
    final double[] grid = { -1, 0, 1 };
    Test[] test = new Test[num_tests];
    int i, j, I, J;
    
    // Create the latent space for the test: 2 continuous dimensions
    Space space = new Space(2, 0);
    
    // Fetch the first and second continuous dimensions via the default names
    dims = new char[2];
    dims[0] = space.getDim("Cont.1");
    dims[1] = space.getDim("Cont.2");
    
    // Create the covariate
    Covariates covariate = new Covariates();
    covariate.set(COVARIATE_NAME, 0);
    // "covariate" variable now includes one covariate called "MyCovariate"
    // It's current value is 0.

    // Set up space for error calculation
    double[][] err_sums = new double[num_tests][];
    for (i=0; i < num_tests; i++)
      err_sums[i] = new double[num_grid*num_grid];
  
    System.out.println("Creating items.");
    ItemBank bank = gen_items(space, covariate);

    System.out.println("Creating tests.");
    for (j=0; j < num_tests; j++)
    {
      // Create a new test with the given properties

      // In this case, we know what the length of the test will be (20),
      // so, we set length_hint for allocation efficiency.
      // Any good/reasonable guess will do---the size will be adjusted
      // automatically as necessary.
      test[j] = new Test(test_names[j], bank, LEN);

      // Register the CAT algorithms for this test.
      // A test must have at minimum a selection algorithm, and administration
      // algorithm, and a stoping critierion.

      new oscats.AlgSimulate().register(test[j]);
      new oscats.AlgEstimate(5).register(test[j]);
      oscats.AlgFixedLength.createAlgFixedLength(LEN).register(test[j]);
    }
    
    // Here we register the item selection criteria for the different tests
    oscats.AlgMaxFisher.createAlgMaxFisher(true, 5).register(test[0]);
    oscats.AlgMaxFisher.createAlgMaxFisher(5).register(test[1]);
    oscats.AlgMaxKl.createAlgMaxKl(5).register(test[2]);
    
    // In Example 1, we created OscatsExaminee objects for each examinee.
    // Here, we'll just use one object over again for each administration.
    Examinee e = new Examinee(covariate);
    Point sim_theta = new Point(space);
    Point est_theta = new Point(space);
    e.setSimTheta(sim_theta);
    e.setEstTheta(est_theta);
    
    System.out.println("Administering.");
    for (I=0; I < num_grid; I++)
    {
      sim_theta.setCont(dims[0], grid[I]);
      for (J=0; J < num_grid; J++)
      {
        System.out.printf("\rAt grid point (%g, %g) ...\n", grid[I], grid[J]);
        sim_theta.setCont(dims[1], grid[J]);
        for (i=0; i < N_EXAMINEES; i++)
        {
          // "covariate" is the same OscatsCovariates object used by "e"
          covariate.set(COVARIATE_NAME, oscats.Random.normal(1));
          System.out.printf("\r%d", i);  System.out.flush();
          for (j=0; j < num_tests; j++)
          {
            // Reset initial theta: Random start ~ N_2(0, I_2)
            est_theta.setCont(dims[0], oscats.Random.normal(1));
            est_theta.setCont(dims[1], oscats.Random.normal(1));
            // Administer the test
            test[j].administer(e);
            // Calculate error (Euclidean distance between sim/est theta)
            err_sums[j][I*num_grid+J] += distance(sim_theta, est_theta);
          }
        }
      }
    }

    // Report results
    FileOutputStream fos = new FileOutputStream("ex02-results.txt");
    Formatter f = new Formatter(fos);
    f.format("D1\tD2");
    for (i=0; i < num_tests; i++)
      f.format("\t%s", test_names[i]);
    f.format("\n");
    for (I=0; I < num_grid; I++)
    {
      for (J=0; J < num_grid; J++)
      {
        f.format("%g\t%g", grid[I], grid[J]);
        for (i=0; i < num_tests; i++)
          f.format("\t%g", err_sums[i][I*num_grid+J]/N_EXAMINEES);
        f.format("\n");
      }
    }
    fos.close();
    
    // No clean up necessary, since Java is garbage collected.
    System.out.println("Done.");
  }

}
