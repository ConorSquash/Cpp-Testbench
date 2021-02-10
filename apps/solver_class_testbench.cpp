#include <iostream>
#include <string>
#include "solver_class.h"
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;





int main() {

	// Vectors and a matrix to store results from text file
	vector<vector<double>> magnetic_flux_matrix;
	vector<double> magnetic_flux_vector;
	vector<double> magnetic_flux_row;


	// READ TEXT FILE
	ifstream file("simulated_data.txt");
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

	vector <double> initial_condition = { 0,0,0.1,0,0 };      // Provide an initial guess for x,y,z,theta,phi

	vector <double> PandO; 	                                   // Create a vector to store position and orientation info

	cout << "\n -> SOLVING FOR INITIAL GUESS" << endl;
	cout << " x : " << initial_condition[0] << endl;
	cout << " y : " << initial_condition[1] << endl;
	cout << " z : " << initial_condition[2] << endl;
	cout << " Pitch : " << initial_condition[3] << endl;
	cout << " Yaw : " << initial_condition[4] << endl;


	// Generate the points identical to those used to get the simulated data 
	// in Matlab for error calculations later on
	vector <double> x_points,y_points,z_points;

	x_points = my_sensor.generate_points(5, 0.05,false);
	y_points = my_sensor.generate_points(5, 0.05,false);
	z_points = my_sensor.generate_points(1, 0.05,true);


	// Variables to calculate position error and orientation error
	double x_error, y_error, z_error, theta_error, phi_error;
	double total_pos_error_squared = 0;
	double total_theta_error_squared = 0;
	double total_phi_error_squared = 0;

	double total_time = 0;   	// Keep track of total time taken to solve 

	int count = 0;

	for (int k = 0; k < z_points.size(); k++)
		for (int j = 0; j < y_points.size(); j++)
			for (int i = 0; i < x_points.size(); i++)
			{
				auto start = high_resolution_clock::now();		// Get starting timepoint

				PandO = my_sensor.Solve(magnetic_flux_matrix[count], initial_condition);   // Pass initial guess and sensor flux and then solve

				auto stop = high_resolution_clock::now();        // Get stopping timepoint

				auto duration = duration_cast<milliseconds>(stop - start);    	 // Get duration by subtracting timepoints 

				//cout << "Time taken by solver for this : " << duration.count() << " milliseconds \n" << endl;

				cout << "\n -> SOLVED P&O " << endl;
				cout << " x : " << PandO[0] << endl;
				cout << " y : " << PandO[1] << endl;
				cout << " z : " << PandO[2] << endl;
				cout << " Pitch : " << PandO[3] << endl;
				cout << " Yaw : " << PandO[4] << endl << endl;

				x_error = x_points[j] - PandO[0];
				y_error = y_points[i] - PandO[1];
				z_error = z_points[k] - PandO[2];
				theta_error = 0 - abs(PandO[3]);
				phi_error = 0 - abs(PandO[4]);

				cout << "This is the error : " << endl;
				cout << "Error in x = " << x_error << endl;
				cout << "Error in y = " << y_error << endl;
				cout << "Error in z = " << z_error << endl;
				cout << "Error in theta = " << theta_error << endl;
				cout << "Error in phi = " << phi_error << endl << endl;

				total_pos_error_squared = pow(x_error, 2) + pow(y_error, 2) + pow(z_error, 2) + total_pos_error_squared;
				total_theta_error_squared =  pow(theta_error, 2) + total_theta_error_squared;
				total_phi_error_squared = pow(phi_error, 2) + total_phi_error_squared;

				total_time = total_time + duration.count();

				count++;

			}
	
	cout << "\n\n\n" << endl;

	cout << "Total number of points solved for : " << num_of_points << endl;
	cout << "Time taken by solver for this : " << total_time << " milliseconds \n\n" << endl;
	cout << "Approximate solving rate ~ " << (num_of_points / total_time) * 1000 << " Hz " << endl;

	double position_RMS_error = sqrt(total_pos_error_squared / num_of_points);
	cout << "Total Position RMS error : " << position_RMS_error << endl;

	double theta_RMS_error = sqrt(total_theta_error_squared / num_of_points);
	cout << "Total Theta RMS error : " << theta_RMS_error << endl;

	double phi_RMS_error = sqrt(total_phi_error_squared / num_of_points);
	cout << "Total Phi RMS error : " << phi_RMS_error << endl;



	
	getchar();


}


