/*
"C++ Algorithms for Magnetic Tracking in Surgery"

Conor Walsh - 13/10/2020

The main.cpp file for the testbench.

This file along with other functions models the
electromagnetic coils, simulates the magnetic fields and
estimates a single sensor measurement using an
objective/cost function.

*/

/*						**NOTE**
*
	This code uses the Eigen3 linear algebra library.
	For the program to run, a folder called 'Dependencies'
	needs to be created in the project solution folder.
	This folder should then contain the unzipped eigen library.

	http://eigen.tuxfamily.org/index.php?title=Main_Page

	Also need to have the rookfighter least squares solver
	saved in the 'Dependencies' folder.

	https://github.com/Rookfighter/least-squares-cpp

*/


#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <chrono>
#include <iterator>
#include <random>

#define _USE_MATH_DEFINES
#include "math.h"

#include "coil_spec.h"
#include "field_coil_calc.h"
#include "sensor_objective_function.h"
#include "generate_testpoints.h"


#include "lsqcpp.h"


#include <Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

using namespace std;
using namespace std::chrono;


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





int main() {

	// =========== DEFINE COIL PARAMETERS IN METERS =================

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



	// ================ POSITION ALGORITHM PARAMETERS ================== \\

	lsq::LevenbergMarquardt <double, MySolver> optimizer;


	optimizer.setMaxIterations(250);

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




	// ====================  Generating White Gaussian Noise ================== \\

	// Simulate imperfect sensor measurements

    // Define random generator with Gaussian distribution
	const double mean = 0.0;
	const double stddev = 0.05;

	default_random_engine generator;
	normal_distribution<double> dist(mean, stddev);





	// ====================  Generating Cube of Testpoints ================== \\

	// Here the user specifies the number and spacing between points
	// to generate a cube of testpoints.

	int num_x_points = 5;
	int num_y_points = 2;
	int num_z_points = 1;

	double x_spacing = 0.01;
	double y_spacing = 0.02;
	double z_spacing = 0.05;


	// Make the 3 vectors that will store the testpoints

	VectorXd x_testpoints(num_x_points);
	VectorXd y_testpoints(num_y_points);
	VectorXd z_testpoints(num_z_points);

	// Generate the points
	generate_testpoints(num_x_points, num_y_points, num_z_points, x_spacing, y_spacing, z_spacing, x_testpoints, y_testpoints, z_testpoints);

	// Check the points
	/*cout << "Cube of testpoints : " << endl;
	cout << " x : " << x_testpoints << endl;
	cout << " y : " << y_testpoints << endl;
	cout << " z : " << z_testpoints << endl;*/

	// Provide an initial guess for x,y,z,theta,phi
	// Declaring it here so it is not declared every time there is a new testpoint

	VectorXd initialGuess(5);
	initialGuess << 0, 0, 0.2, 1, 1.5;


	// ================ CREATE SIMULATED SENSOR MEASUREMENT(S) ================== \\

	int iterations = 0;       // Keep track of how many points are solved for

	double total_time = 0;   	// Keep track of total time taken to solve 

	// Determine the error between the solved solution and the testpoint

	double x_error, y_error, z_error, theta_error, phi_error;
	double total_pos_error_squared = 0;
	double total_theta_error_squared = 0;
	double total_phi_error_squared = 0;


	// Loop through all previously generated points

	for (int k = 0; k < num_z_points; k++)
		for (int j = 0; j < num_y_points; j++)
			for (int i = 0; i < num_x_points; i++)
			{

				// Calculate flux through the sensor at position[x, y, z] with angles[theta, phi]
				double x_test = x_testpoints(i);
				double y_test = y_testpoints(j);
				double z_test = z_testpoints(k);
				double theta_test = M_PI / 3;
				double phi_test = M_PI_2;


				// Simulate magnetic field from each coil at a point in space.
				// Returns M x 1 vector of flux components, where M is the number of magnetic transmitter coils.

				VectorXd Hx(8), Hy(8), Hz(8);   // Create the vectors that will be returned from field_coil_calc 


				field_coil_calc(1, X_Matrix, Y_Matrix, Z_Matrix, x_test, y_test, z_test, Hx, Hy, Hz);


				sensor_flux = (Hx.array() * sin(theta_test) * cos(phi_test)) + (Hy.array() * sin(theta_test) * sin(phi_test)) + (Hz.array() * cos(theta_test));


				//cout << "Sensor flux before noise : " << endl;
				//cout << sensor_flux << endl;

				// Add Gaussian noise
		//sensor_flux = sensor_flux.array() + dist(generator);

				//cout << "Sensor flux after noise : " << endl;
				//cout << sensor_flux << endl;

					

				// ==================== CALL OBJECTIVE FUNCTION ================== \\

					// Call solver to solve for position vector variable xval()
					// Magnetic coil filaments constants passed to the objective function
					// The simulated sensor calibration contant is passed to the objective function
					// Objective function is called in Struct 'MySolver' at the top of the program




				auto start = high_resolution_clock::now();		// Get starting timepoint

				auto result = optimizer.minimize(initialGuess);

				cout << "Done! Converged: " << (result.converged ? "true" : "false")
					<< " Iterations: " << result.iterations << std::endl;

				auto stop = high_resolution_clock::now();        // Get stopping timepoint

				auto duration = duration_cast<milliseconds>(stop - start);    	 // Get duration by subtracting timepoints 
		
				cout << "Time taken by solver for this : " << duration.count() << " milliseconds \n" << endl;


				cout << "This is the test point : " << endl;
				cout << " x = " << x_test << endl;
				cout << " y = " << y_test << endl;
				cout << " z = " << z_test << endl;
				cout << " theta = " << theta_test << endl;
				cout << " phi = " << phi_test << endl;

				cout << "\n" << endl;

				cout << "This is the solved point : " << endl;
				cout << " x = " << result.xval(0) << endl;
				cout << " y = " << result.xval(1) << endl;
				cout << " z = " << result.xval(2) << endl;
				cout << " theta = " << result.xval(3) << endl;
				cout << " phi = " << result.xval(4) << endl;

				cout << "\n" << endl;

				x_error = x_test - result.xval(0);
				y_error = y_test - result.xval(1);
				z_error = z_test - result.xval(2);
				theta_error = theta_test - abs(result.xval(3));
				phi_error = phi_test - abs(result.xval(4));

				cout << "This is the error : " << endl;
				cout << "Error in x = " << x_error << endl;
				cout << "Error in y = " << y_error << endl;
				cout << "Error in z = " << z_error << endl;
				cout << "Error in theta = " << theta_error << endl;
				cout << "Error in phi = " << phi_error << endl;

				total_pos_error_squared = pow(x_error, 2) + pow(y_error, 2) + pow(z_error, 2) + total_pos_error_squared;
				total_theta_error_squared =  pow(theta_error, 2) + total_theta_error_squared;
				total_phi_error_squared = pow(phi_error, 2) + total_phi_error_squared;

				//cout << "error squared : " << total_error_squared << endl;

				cout << "\n\n\n" << endl;

				iterations++;

				total_time = total_time + duration.count();
		
			}

	


	cout << "Total number of points solved for : " << iterations << endl;
	cout << "Time taken by solver for this : " << total_time << " milliseconds \n\n" << endl;

	double position_RMS_error = sqrt( total_pos_error_squared / iterations);
	cout << "Total Position RMS error : " << position_RMS_error << endl;

	double theta_RMS_error = sqrt(total_theta_error_squared / iterations);
	cout << "Total Theta RMS error : " << theta_RMS_error << endl;

	double phi_RMS_error = sqrt(total_phi_error_squared / iterations);
	cout << "Total Phi RMS error : " << phi_RMS_error << endl;



	return 0;
}


