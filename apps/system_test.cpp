#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex> 


#define _USE_MATH_DEFINES
#include "math.h"

//#include "daq_class.h"
#include "../src/daq_class.h"
#include "solver_class.h"
#include "OpenIGTLink_class.h"
#include "demodulation_class.h"


#include <Dense>

using namespace std::chrono;

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::MatrixXcd;
using Eigen::VectorXcd;


using namespace std;

extern VectorXd sensor_flux;
extern VectorXd K;
extern MatrixXd buffer_result;

std::mutex mtx;           // mutex for critical section

volatile int flag;

int main() {


	// Vectors and a matrix to store results from text file
	vector<vector<double>> magnetic_flux_matrix;
	vector<double> magnetic_flux_vector;
	vector<double> magnetic_flux_row;

	// Create text file to write P&O data to
	ofstream results_file;
	results_file.open("rotation_test_neg_y.txt");


	// READ TEXT FILE
	ifstream file("matlab_sensorflux.txt");

	if (file.is_open())
	{
		string line;

		while (getline(file, line))
		{
			magnetic_flux_vector.push_back(stod(line));    // Store every result in a vector
		}

		file.close();
	}
	else
		cout << "Cant open file!" << endl;

	// Sort results into a matrix of 8 columns and however many rows
	int num_of_points = magnetic_flux_vector.size() / 8;

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

	// ========= SETUP ================

	double Fs = 100e3;
	double numSamps = 1000;

	vector <double> static_cal = { -0.4324,-0.2946, -0.2106,-0.1565,-0.1234,-0.0984,-0.0785,-0.0710 };

	vector <double> cal = { 2.5968, -1.6552, -1.0803, -0.7937, -0.5783, -0.4606, -0.3466, -0.2964 };
	//vector <double> cal = { 2.5968, 1.6552, 1.0803, 0.7937, 0.5783, 0.4606, 0.3466, 0.2964 };

	int port = 18945;	// Default port = 18944
	float fps = 1000;

	// Provide an initial guess for x,y,z,theta,phi
	vector <double> initial_condition = { 0, 0, 0.25, M_PI_2 , M_PI_2 };

	// Sets up the DAQ to sample at Fs and fills a buffer of numSamps samples
	DAQ my_channel(Fs, numSamps, FALSE, "Dev2","0", "4");

	Demod filter(Fs, numSamps); 	// Sets up the demodulation parameters

	Solver my_sensor;

	my_sensor.Setup();						    // Models the coils
	my_sensor.ConfigureSolver();			    // Sets the parameters of the solver (Max Iterations, minimum error....)
	//my_sensor.SetCalibration(static_cal);     // Set the calibration values 
	my_sensor.SetCalibration(cal);				// Set the calibration values 

	//vector <double> PandO(5); 	         // Create a vector to store position and orientation info
	vector <double> PandO = { 0, 0, 0.20, M_PI_2 , M_PI_2 }; 	         // Create a vector to store position and orientation info

	OpenIGTLink my_server(port, fps);    // Create a server on the specified port 



	// ========= LOOP ================

	int count = 0;
	double total_time = 0;

	flag = 0;

	auto start = high_resolution_clock::now();		// Get starting timepoint
	while (count < 100)
	{ 

		if (flag)
		{

			mtx.lock();

			filter.demodulate_w_phase(numSamps, buffer_result);

			mtx.unlock();

			PandO = my_sensor.Solve(filter.magnitude_r, PandO);

			results_file << PandO[3]  << ", " <<
				PandO[4]  << endl;

			//results_file << PandO[0] << ", " <<
			//	PandO[1] << ", " <<
			//	PandO[2] << ", " <<
			//	PandO[3] << ", " <<
			//	PandO[4] << ", " << endl;

			// When using text file of sensor fluxes
			//PandO = my_sensor.Solve(magnetic_flux_matrix[count], initial_condition);   


			//cout << "\n -> SOLVED P&O " << endl;
			//cout << " x : " << PandO[0] * 100 << " cm" << endl;
			//cout << " y : " << PandO[1] * 100 << " cm" << endl;
			//cout << " z : " << PandO[2] * 100 << " cm" << endl;
			//cout << " Pitch : " << PandO[3] << " rads " << endl;
			//cout << " Yaw : " << PandO[4]  << " rads " << endl << endl;


			my_server.SendIGTMessage(PandO);

			flag = 0;

			count++;

		}

		//if (count == num_of_points)
		
			//count = 0;
		
	}
	
	
	auto stop = high_resolution_clock::now();        // Get stopping timepoint

	auto duration = duration_cast<milliseconds>(stop - start);    	 // Get duration by subtracting timepoints 

	total_time = duration.count();

	cout << "Update rate : " << (count / total_time) * 1000 << " Hz " << endl;

	cout << duration.count() / 1000 << endl;

	getchar();




}