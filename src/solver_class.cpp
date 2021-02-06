#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <chrono>
#include <stdio.h>


#define _USE_MATH_DEFINES
#include "math.h"

#include "coil_spec.h"
#include "field_coil_calc.h"
#include "sensor_objective_function.h"
#include "lsqcpp.h"
#include "solver_class.h"



#include <Dense>

using Eigen::MatrixXd;
using Eigen::MatrixXf;
using Eigen::VectorXd;
using Eigen::VectorXf;



using namespace std;



// Solver variables
VectorXd sensor_flux(8);
vector<vector<double>> x_matrix(8);
vector<vector<double>> y_matrix(8);
vector<vector<double>> z_matrix(8);

MatrixXd X_Matrix(8, 102);
MatrixXd Y_Matrix(8, 102);
MatrixXd Z_Matrix(8, 102);





struct MySolver
{


	void operator()(const VectorXd& xval, VectorXd& fval, MatrixXd& no_name) const
	{

		VectorXd currentPandO(5);

		currentPandO << xval(0), xval(1), xval(2), xval(3), xval(4);

		VectorXd out_vector(8);        // A vector to get the result of FluxModel - FluxReal

		vector<vector<double>> calibration = { {1},{1},{1},{1},{1},{1},{1},{1} };    // Create vector of calibration values (column)

		sensor_objective_function(currentPandO, sensor_flux, X_Matrix, Y_Matrix, Z_Matrix, calibration, out_vector);

		fval = out_vector;


	}
};







vector <double> Solver::Solve(vector <double> amplitudes, vector <double> initial_condition)
{



	//====================================================================


		// Define side length of outer square
		// Define width of each track
		// Define spacing between tracks
		// Define thickness of the emitter coil PCB
		// Define total number of turns per coil

	double l = 70e-03;
	double w = 0.5e-03;
	double s = 0.25e-03;
	double thickness = 1.6e-03;
	int Nturns = 25;


	// Generate coil filaments

	string ModelType = "EXACT";
	//string ModelType = "FAST";

	vector<double> x_points_angled, y_points_angled, z_points_angled;     // Make two vectors (for x,y,z) to store coordinates for the 
	vector<double> x_points_vert, y_points_vert, z_points_vert;           // "vertical" and "angled" coils

	coil_spec(l, w, s, thickness, M_PI_4, Nturns, ModelType, x_points_angled, y_points_angled, z_points_angled);
	coil_spec(l, w, s, thickness, M_PI_2, Nturns, ModelType, x_points_vert, y_points_vert, z_points_vert);


	// Generate each coil x and y centre point in mm
	array<double, 8> x_centre_points = { -93.543, 0, 93.543, -68.55, 68.55, -93.543, 0, 93.543 };
	array<double, 8> y_centre_points = { 93.543, 68.55, 93.543, 0, 0, -93.543, -68.55, -93.543 };

	for (int i = 0; i < x_centre_points.size(); i++)  // Convert to metres from mm
	{
		x_centre_points[i] *= 1e-03;
		y_centre_points[i] *= 1e-03;

	}

	// Add the centre position offsets to each coil

	vector<double> x_points1, x_points2, x_points3, x_points4, x_points5, x_points6, x_points7, x_points8;
	vector<double> y_points1, y_points2, y_points3, y_points4, y_points5, y_points6, y_points7, y_points8;
	vector<double> z_points1, z_points2, z_points3, z_points4, z_points5, z_points6, z_points7, z_points8;


	for (int i = 0; i < x_points_angled.size(); i++)
	{
		// Add x offset
		x_points1.push_back(x_points_vert[i] + x_centre_points[0]);
		x_points2.push_back(x_points_angled[i] + x_centre_points[1]);
		x_points3.push_back(x_points_vert[i] + x_centre_points[2]);
		x_points4.push_back(x_points_angled[i] + x_centre_points[3]);
		x_points5.push_back(x_points_angled[i] + x_centre_points[4]);
		x_points6.push_back(x_points_vert[i] + x_centre_points[5]);
		x_points7.push_back(x_points_angled[i] + x_centre_points[6]);
		x_points8.push_back(x_points_vert[i] + x_centre_points[7]);

		// Add y offset
		y_points1.push_back(y_points_vert[i] + y_centre_points[0]);
		y_points2.push_back(y_points_angled[i] + y_centre_points[1]);
		y_points3.push_back(y_points_vert[i] + y_centre_points[2]);
		y_points4.push_back(y_points_angled[i] + y_centre_points[3]);
		y_points5.push_back(y_points_angled[i] + y_centre_points[4]);
		y_points6.push_back(y_points_vert[i] + y_centre_points[5]);
		y_points7.push_back(y_points_angled[i] + y_centre_points[6]);
		y_points8.push_back(y_points_vert[i] + y_centre_points[7]);

		// No z offset
		z_points1.push_back(z_points_vert[i]);
		z_points2.push_back(z_points_angled[i]);
		z_points3.push_back(z_points_vert[i]);
		z_points4.push_back(z_points_angled[i]);
		z_points5.push_back(z_points_angled[i]);
		z_points6.push_back(z_points_vert[i]);
		z_points7.push_back(z_points_angled[i]);
		z_points8.push_back(z_points_vert[i]);

	}

	// Now bundle each into a matrix with coil vertices organised into columns


	x_matrix = { {x_points1},
				  {x_points2},
				  {x_points3},
				  {x_points4},
				  {x_points5},
				  {x_points6},
				  {x_points7},
				  {x_points8} };

	y_matrix = { {y_points1},
				  {y_points2},
				  {y_points3},
				  {y_points4},
				  {y_points5},
				  {y_points6},
				  {y_points7},
				  {y_points8} };

	z_matrix = { {z_points1},
				  {z_points2},
				  {z_points3},
				  {z_points4},
				  {z_points5},
				  {z_points6},
				  {z_points7},
				  {z_points8} };


	// Convert to Eigen Matrices

	 //Setting size of X_Matrix and others above..could resize here

	for (int i = 0; i < x_matrix.size(); i++)
		for (int j = 0; j < x_matrix[0].size(); j++)
		{
			X_Matrix(i, j) = x_matrix[i][j];
			Y_Matrix(i, j) = y_matrix[i][j];
			Z_Matrix(i, j) = z_matrix[i][j];
		}

	cout << "\n -> COILS MODELLED" << endl;

	//====================================================================

	lsq::LevenbergMarquardt <double, MySolver> optimizer;


	optimizer.setMaxIterations(5);

	optimizer.setMaxIterationsLM(250);

	optimizer.setLambdaIncrease(5);

	optimizer.setLambdaDecrease(1e-3);

	optimizer.setThreads(0);


	// Set the minimum length of the gradient.
	// The optimizer stops minimizing if the gradient length falls below this value.
	// Set it to 0 or negative to disable this stop criterion (default is 1e-9).
	optimizer.setMinGradientLength(1e-9);

	// Set the minimum length of the step.
	// The optimizer stops minimizing if the step length falls below this value.
	// Set it to 0 or negative to disable this stop criterion (default is 1e-9).
	optimizer.setMinStepLength(1e-9);

	// Set the minimum least squares error.
	// The optimizer stops minimizing if the error falls below this value.
	// Set it to 0 or negative to disable this stop criterion (default is 0).
	optimizer.setMinError(1e-5);

	// Set the the parametrized StepSize functor used for the step calculation.
	//optimizer.setStepSize(lsq::ArmijoBacktracking<double>(0.8, 0.1, 1e-10, 1.0, 0));

	// Turn verbosity on, so the optimizer prints status updates after each iteration.
	optimizer.setVerbosity(0);

	cout << "\n -> SOLVER CONFIGURED" << endl;

	//====================================================================


	// Call solver to solve for position vector variable xval()
	// Magnetic coil filaments constants passed to the objective function
	// The simulated sensor calibration contant is passed to the objective function
	// Objective function is called in Struct 'MySolver' at the top of the program



	//auto start = high_resolution_clock::now();		// Get starting timepoint

	VectorXd initialGuess(5);
	initialGuess << initial_condition[0], 
					initial_condition[1], 
					initial_condition[2], 
					initial_condition[3], 
					initial_condition[4];

	cout << "\n -> SOLVING FOR INITIAL GUESS" << endl;
	cout << " x : " << initialGuess(0) << endl;
	cout << " y : " << initialGuess(1) << endl;
	cout << " x : " << initialGuess(2) << endl;
	cout << " Pitch : " << initialGuess(3) << endl;
	cout << " Yaw : " << initialGuess(4) << endl << endl;

	int iterations = 0;

	for (int i = 0; i < 8; i++)
		sensor_flux(i) = amplitudes[i];
	

	// Start the optimization.
	auto result = optimizer.minimize(initialGuess);

	/*
	//cout << "Done! Converged: " << (result.converged ? "true" : "false")
		//<< " Iterations: " << result.iterations << std::endl;

	 //do something with final function value
	//cout << "Final xval: " << result.xval.transpose() << std::endl;


	//auto stop = high_resolution_clock::now();        // Get stopping timepoint
	//auto duration = duration_cast<milliseconds>(stop - start);    	 // Get duration by subtracting timepoints 
	//cout << "Number of times solved : " << iterations << endl;
	//cout << "Time taken by solver for this : " << duration.count() << " milliseconds" << endl;
	*/

	vector <double> PandO;

	for (int i = 0; i < 5; i++)
		PandO.push_back(result.xval(i));

	return PandO;

}