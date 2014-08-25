 % OSCATS: Open-Source Computerized Adaptive Testing System
 % Copyright 2011 Michael Culbertson <culbert1@illinois.edu>
 %
 % Example 2
 %
 % Example 2
 %
 % 500 Items: 2D 2PL with covariate
 %   a1 ~ U(0, 1), a2 ~ U(0, 2), b ~ N(0,3), covariate coef ~ U(0.5, 1.5)
 % 500 Examinees on each point of the grid {-1, 0, 1} x {-1, 0, 1}
 %   Examinee covariate ~ N(0, 1)
 % Item selection:
 %  - A-optimality
 %  - D-optimality
 %  - KL
 % Test length: 20
 % Report:
 %  - Mean Euclidian distance between estimated and simulated theta,
 %      calculated for each grid point

 function ex02_java()
 
%% Add glib.jar and oscats.jar to class path
%javaaddpath('../bindings/java/glib.jar');
%javaaddpath('../bindings/java/oscats.jar');
%% Note that the path to libglibjni and liboscatsjni 
%% must be in Matlab's toolbox/local/librarypath.txt

import oscats.Space;
import oscats.Point;
import oscats.Examinee;
import oscats.Test;

N_EXAMINEES = 500;
N_ITEMS = 500;
LEN = 20;
COVARIATE_NAME = 'MyCovariate';

%/ Helper function to calculate the Euclidian distance between two points
function ret = distance(a, b)
  d1 = a.getCont(dims(1)) - b.getCont(dims(1));
  d2 = a.getCont(dims(2)) - b.getCont(dims(2));
  ret = sqrt(d1*d1 + d2*d2);
end

% We will have to tell the models which covariate to use.  This is done by
% supplying an OscatsCovariates object that includes only that covariate to
% be included in the model.  In this case, all of our items will use the
% same covariate.  We'll create the OscatsCovariates object below when we
% create the latent space, and pass both to the gen_items() function.
function ret = gen_items(space, covariate, dims)
    import oscats.ItemBank
    import oscats.ModelL2p
    import oscats.Item
    % Create an item bank to store the items.
    % Setting the property "sizeHint" increases allocation efficiency.
    bank = ItemBank(N_ITEMS);
    % Create the items
    for k = 1:N_ITEMS
      % First we create an IRT model container for our item
      % We have to specify which dimensions to be used with the "dims" array
      % (in this case, we use both of the dimensions of the space)
      model = ModelL2p(space, dims, covariate);
      % Then, set the parameters.  Here there are 4:
      % Discrimination on two dimensions, difficulty, and covariate coef.
      model.setParamByName('Diff', oscats.Random.normal(sqrt(3)));
      model.setParamByName('Discr.Cont.1', oscats.Random.uniformRange(0, 1));
      model.setParamByName('Discr.Cont.2', oscats.Random.uniformRange(0, 2));
      model.setParamByName(COVARIATE_NAME, oscats.Random.uniformRange(0.5, 1.5));
      % Create an item based on this model (as default model)
      item = Item(model);
      % Add the item to the item bank
      bank.addItem(item);
      % Since Java and Matlab are garbage collected, we don't have to worry
      % about reference counting.
    end
    ret = bank;
end

%% main

num_tests = 3;  num_grid = 3;
test_names = { 'A-opt', 'D-opt', 'KL' };
grid = [ -1 0 1 ];
test = javaArray('oscats.Test', num_tests);
    
% Create the latent space for the test: 2 continuous dimensions
space = oscats.Space(2, 0);

% Fetch the first and second continuous dimensions via the default names
dims = [ space.getDim('Cont.1') space.getDim('Cont.2') ];

% reate the covariate
covariate = oscats.Covariates();
covariate.set(COVARIATE_NAME, 0);
% "covariate" variable now includes one covariate called "MyCovariate"
% It's current value is 0.

% Set up space for error calculation
err_sums = zeros(num_tests, num_grid*num_grid);

disp('Creating items.');
bank = gen_items(space, covariate, dims);

disp('Creating tests.');
for j = 1:num_tests
      % Create a new test with the given properties

      % In this case, we know what the length of the test will be (30),
      % so, we set length_hint for allocation efficiency.
      % Any good/reasonable guess will do---the size will be adjusted
      % automatically as necessary.
      test(j) = Test(test_names(j), bank, LEN);

      % Register the CAT algorithms for this test.
      % A test must have at minimum a selection algorithm, and administration
      % algorithm, and a stoping critierion.
      oscats.AlgSimulate().register(test(j));
      oscats.AlgEstimate(5).register(test(j));
      alg = oscats.AlgFixedLength.createAlgFixedLength(LEN);
      alg.register(test(j));
end
    
% Here we register the item selection criteria for the different tests
alg = oscats.AlgMaxFisher.createAlgMaxFisher(true, 5);
alg.register(test(1));
alg = oscats.AlgMaxFisher.createAlgMaxFisher(5);
alg.register(test(2));
alg = oscats.AlgMaxKl.createAlgMaxKl(5);
alg.register(test(3));
    
% In Example 1, we created OscatsExaminee objects for each examinee.
% Here, we'll just use one object over again for each administration.
e = Examinee(covariate);
sim_theta = Point(space);
est_theta = Point(space);
e.setSimTheta(sim_theta);
e.setEstTheta(est_theta);

disp('Administering.');
for I = 1:num_grid
  sim_theta.setCont(dims(1), grid(I));
  for J = 1:num_grid
    fprintf('At grid point (%g, %g) ...\n', grid(I), grid(J));
    sim_theta.setCont(dims(2), grid(J));
    for i = 1:N_EXAMINEES
      % "covariate" is the same OscatsCovariates object used by "e"
      covariate.set(COVARIATE_NAME, oscats.Random.normal(1));
      fprintf('%d\r', i);  % System.out.flush();
      for j = 1:num_tests
        % Reset initial theta: Random start ~ N_2(0, I_2)
        est_theta.setCont(dims(1), oscats.Random.normal(1));
        est_theta.setCont(dims(2), oscats.Random.normal(1));
        % Administer the test
        test(j).administer(e);
        % Calculate error (Euclidean distance between sim/est theta)
        err_sums(j, (I-1)*num_grid+J) = err_sums(j, (I-1)*num_grid+J) + distance(sim_theta, est_theta);
      end
    end
  end
end

% Report results
f = fopen('ex02-results.txt', 'w');
fprintf(f, 'D1\tD2');
for i = 1:num_tests
  fprintf(f, '\t%s', test_names{i});
end
fprintf(f, '\n');
for I = 1:num_grid
  for J = 1:num_grid
    fprintf(f, '%g\t%g', grid(I), grid(J));
    for i = 1:num_tests
      fprintf(f, '\t%g', err_sums(i, (I-1)*num_grid+J)/N_EXAMINEES);
    end
    fprintf(f, '\n');
  end
end
fclose(f);

% No clean up necessary, since Java and Matlab are garbage collected.
disp('Done.');

 end
