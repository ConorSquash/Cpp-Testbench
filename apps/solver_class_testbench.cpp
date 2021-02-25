#include <iostream>
#include <string>
#include "solver_class.h"
#include <fstream>
#include <chrono>

#define _USE_MATH_DEFINES
#include "math.h"

using namespace std;
using namespace std::chrono;





int main() {

	// Vectors and a matrix to store results from text file
	vector<vector<double>> magnetic_flux_matrix;
	vector<double> magnetic_flux_vector;
	vector<double> magnetic_flux_row;

	// Create text file to write P&O data to
	ofstream results_file;

	results_file.open("y10cm_results_cpp.txt");
	//results_file.open("y25cm_results_cpp.txt");
	//results_file.open("z10cm_results_cpp.txt");

	// READ TEXT FILE
	ifstream file("yOriented-10cm.txt");
	//ifstream file("yOriented-25cm.txt");
	//ifstream file("zOriented-10cm.txt");

	//ifstream file("simulated_data_w_noise.txt");

	if (file.is_open()) {

		string line;

		while (getline(file, line))
		{
			//printf("%s\n", line.c_str()); 
			magnetic_flux_vector.push_back(stod(line));    // Store every result in a vector
		}

		file.close();
	}
	else
	{
		cout << "Cant open file!" << endl;
	}

	int num_of_points = magnetic_flux_vector.size() / 8;
	cout << " Number of test points found in file : " << num_of_points << endl << endl;

	// Sort results into a matrix of 8 columns and however many rows

	int counter = 0;

	for (int j = 0; j < num_of_points; j++)
	{
		for (int i = 0; i <= 7; i++)
		{
			magnetic_flux_row.push_back(magnetic_flux_vector[counter]);    // Use row as a temp variable
			counter++;
		}

		magnetic_flux_matrix.push_back(magnetic_flux_row);    // Put row into matrix
		magnetic_flux_row.clear();                            // Clear row before repeating
	}



	// SOLVE
	Solver my_sensor;

	my_sensor.Setup();             // Models the coils

	my_sensor.ConfigureSolver();   // Sets the parameters of the solver (Max Iterations, minimum error....)

	vector <double> cal = { -0.4324,-0.2946, -0.2106,-0.1565,-0.1234,-0.0984,-0.0785,-0.0710};   

	my_sensor.SetCalibration(cal);    // Set the calibration values 

	vector <double> initial_condition = { 0, 0, 0.125, M_PI_2, M_PI_2 };      // Provide an initial guess for x,y,z,theta,phi

	vector <double> PandO; 	        // Create a vector to store position and orientation info

	cout << "\n -> SOLVING FOR INITIAL GUESS" << endl;
	cout << " x : " << initial_condition[0] << endl;
	cout << " y : " << initial_condition[1] << endl;
	cout << " z : " << initial_condition[2] << endl;
	cout << " Pitch : " << initial_condition[3] << endl;
	cout << " Yaw : " << initial_condition[4] << endl;

	cout << "\n\n";


	// Generate the points identical to those used to get the simulated data 
	// in Matlab for error calculations later on
	vector <double> x_points,y_points,z_points;

	// Offsets of the grid of points used in the actual test
	double deltaX = -9.029e-04;
	double deltaY = -0.00723;

	x_points = my_sensor.generate_points(7, 0.048,false, deltaX);
	y_points = my_sensor.generate_points(7, 0.048,false, deltaY);
	z_points = my_sensor.generate_points(1, 0.12,true,0);


	// Variables to calculate position error and orientation error
	double x_error, y_error, z_error, theta_error, phi_error;
	double total_pos_error_squared = 0;
	double total_theta_error_squared = 0;
	double total_phi_error_squared = 0;

	double total_time = 0;   	// Keep track of total time taken to solve 
	vector <double> times;      // Create a vector to store the time it took to solve for each point

	int count = 0;

	for (int k = 0; k < z_points.size(); k++)
		for (int j = 0; j < y_points.size(); j++)
			for (int i = 0; i < x_points.size(); i++)
			{
				auto start = high_resolution_clock::now();		// Get starting timepoint

				//initial_condition = { x_points[j], y_points[i], 0.12, M_PI_2, M_PI_2 };

				PandO = my_sensor.Solve(magnetic_flux_matrix[count], initial_condition);   // Pass initial guess and sensor flux and then solve

				auto stop = high_resolution_clock::now();        // Get stopping timepoint

				auto duration = duration_cast<milliseconds>(stop - start);    	 // Get duration by subtracting timepoints 

				times.push_back(duration.count());        // Save the time taken to solve for this point

				//cout << "Time taken by solver for this : " << duration.count() << " milliseconds \n" << endl;

				//Print out what point its at in the grid
				//cout << "\n Test point (cm) ->" "\t x : " << x_points[j] << "\t y : " 
					//<< y_points[i]  << "\t z : " << z_points[k]  << endl;

				//cout << "\n -> SOLVED P&O " << endl;
				//cout << " x : " << PandO[0]  << " cm" << endl;
				//cout << " y : " << PandO[1]  << " cm" << endl;
				//cout << " z : " << PandO[2]  << " cm" << endl;
				//cout << " Pitch : " << PandO[3] << " degrees" << endl;
				//cout << " Yaw : " << PandO[4]  << " degrees" << endl << endl;

				cout << "Point " << count + 1 << " : [" 
					<< PandO[0] << " " << PandO[1] << " " << PandO[2] << " "
					<< PandO[3] << " " << PandO[4] << " " << "]" << endl;

				results_file << PandO[0] << "," << PandO[1] << "," << PandO[2] << ","
					<< PandO[3] << "," << PandO[4] << "," << endl;

				//x_error = x_points[j] - PandO[0];
				//y_error = y_points[i] - PandO[1];
				//z_error = z_points[k] - PandO[2];
				//theta_error = 90 - PandO[3];
				//phi_error = 90 - PandO[4];

				//cout << "This is the error : " << endl;
				//cout << "Error in x = " << x_error * 10 << " mm" << endl;
				//cout << "Error in y = " << y_error * 10 << " mm" << endl;
				//cout << "Error in z = " << z_error * 10 << " mm" << endl;
				//cout << "Error in theta = " << theta_error << " degrees" << endl;
				//cout << "Error in phi = " << phi_error << " degrees" << endl << endl;

				//total_pos_error_squared = pow(x_error, 2) + pow(y_error, 2) + pow(z_error, 2) + total_pos_error_squared;
				//total_theta_error_squared =  pow(theta_error, 2) + total_theta_error_squared;
				//total_phi_error_squared = pow(phi_error, 2) + total_phi_error_squared;

				total_time = total_time + duration.count();

				count++;

				//initial_condition = PandO;           // Set the previous solved point as the new initial condition

				//getchar();

			}

	results_file.close();
	
	cout << "\n\n\n" << endl;

	cout << "Total number of points solved for : " << num_of_points << endl << endl;
	cout << "Time taken by solver for this : " << total_time << " milliseconds \n" << endl;
	cout << "Average time per solve ~ " <<  total_time / num_of_points  << " ms \n" << endl;
	cout << "Approximate solving rate ~ " << (num_of_points / total_time) * 1000 << " Hz " << endl;


	//double position_RMS_error = sqrt(total_pos_error_squared / num_of_points);
	//cout << "Total Position RMS error : " << position_RMS_error << endl;

	//double theta_RMS_error = sqrt(total_theta_error_squared / num_of_points);
	//cout << "Total Theta RMS error : " << theta_RMS_error << endl;

	//double phi_RMS_error = sqrt(total_phi_error_squared / num_of_points);
	//cout << "Total Phi RMS error : " << phi_RMS_error << endl;



	//cout << "\n\n Time to solve for each point in ms: " << endl;

	//for (int i = 0; i < times.size(); i++)
		//cout << times[i] << endl;



	
	getchar();


}


