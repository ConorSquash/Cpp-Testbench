#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <Dense>

#define _USE_MATH_DEFINES
#include "math.h"

//#include "daq_class.h"
#include "../src/daq_class.h"
#include "solver_class.h"
#include "OpenIGTLink_class.h"
#include "demodulation_class.h"


#include <Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

using Eigen::MatrixXd;
using Eigen::VectorXd;

using Eigen::MatrixXcd;
using Eigen::VectorXcd;

using namespace std;


int main() {


	// ========= SETUP ================

	double Fs = 100e3;
	double numSamps = 1000;

	vector <double> cal = { -0.4324,-0.2946, -0.2106,-0.1565,-0.1234,-0.0984,-0.0785,-0.0710 };

	int port = 18944;
	float fps = 1;

	// Provide an initial guess for x,y,z,theta,phi
	vector <double> initial_condition = { 0, 0, 0.275, M_PI_2 , M_PI_2 };   

	// Sets up the DAQ to sample channel 1 at Fs and fills a buffer of numSamps samples
	DAQ my_channel(Fs, numSamps);

	Demod filter(Fs, numSamps); 	// Sets up the demodulation parameters

	Solver my_sensor;

	my_sensor.Setup();                // Models the coils
	my_sensor.ConfigureSolver();      // Sets the parameters of the solver (Max Iterations, minimum error....)
	my_sensor.SetCalibration(cal);    // Set the calibration values 

	vector <double> PandO; 	         // Create a vector to store position and orientation info

	OpenIGTLink my_server(port, fps);    // Create a server on the specified port 

	// ========= LOOP ================

	// SOLVE ONCE OUTSIDE LOOP USING INITIAL CONDITIONS
	// THEN USE DYNAMIC IC'S
	my_channel.ReadSamples();
	filter.demodulate(numSamps, my_channel.my_result);
	PandO = my_sensor.Solve(filter.magnitude_r, initial_condition);

	for (int i = 0; i < 9; i++)
	{

		// Reads the DAQ buffer into an Eigen matrix (variable my_result)
		my_channel.ReadSamples();

		filter.demodulate(numSamps, my_channel.my_result);

		//for (int i = 0; i < 8; i++)
			//cout << filter.magnitude_r[i] << endl;

		// Initial condition here is previously solved point
		PandO = my_sensor.Solve(filter.magnitude_r, PandO);

		cout << PandO[0] << " " << PandO[1] << " " << PandO[2] << " "
		<< PandO[3] << " " << PandO[4] << " " << "]" << endl;


		my_server.SendMessage(PandO);

	}


	getchar();




}