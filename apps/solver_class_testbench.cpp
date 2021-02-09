#include <iostream>
#include <string>
#include "solver_class.h"
#include <fstream>

using namespace std;





int main() {

	// Vectors and a matrix to store results from text file
	vector<vector<double>> magnetic_flux_matrix;
	vector<double> magnetic_flux_vector;
	vector<double> magnetic_flux_row;


	// READ TEXT FILE
	ifstream file("simulated_data.txt");

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

	vector <double> initial_condition = { 0,0,0.1,0.5,0.5 };      // Provide an initial guess for x,y,z,theta,phi

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

	for (int i = 0; i < z_points.size(); i++)
	{
		cout << z_points[i] << endl;
	}


	// Variables to calculate position error and orientation error
	//double x_error, y_error, z_error, theta_error, phi_error;
	//double total_pos_error_squared = 0;
	//double total_theta_error_squared = 0;
	//double total_phi_error_squared = 0;

	/*
	
	for (int i = 0; i < num_of_points; i++)
	{
		PandO = my_sensor.Solve(magnetic_flux_matrix[i], initial_condition);   // Pass initial guess and sensor flux and then solve

		cout << "\n -> SOLVED P&O " << endl;
		cout << " x : " << PandO[0] << endl;
		cout << " y : " << PandO[1] << endl;
		cout << " z : " << PandO[2] << endl;
		cout << " Pitch : " << PandO[3] << endl;
		cout << " Yaw : " << PandO[4] << endl << endl;


	}
	
	*/





	
	getchar();


}


