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

extern VectorXd sensor_flux;
extern VectorXd K;

int main() {


	// ========= SETUP ================

	double Fs = 100e3;
	double numSamps = 1000;

	vector <double> cal = { -2.5968, - 1.6552, - 1.0803, - 0.7937, - 0.5783, - 0.4606, - 0.3466, - 0.2964 };
	//vector <double> cal = { 2.5968, 1.6552, 1.0803, 0.7937, 0.5783, 0.4606, 0.3466, 0.2964 };

	int port = 18944;
	float fps = 100;

	// Provide an initial guess for x,y,z,theta,phi
	vector <double> initial_condition = { 0, 0, 0.2, 0 , 0 };   

	// Sets up the DAQ to sample channel 1 at Fs and fills a buffer of numSamps samples
	DAQ my_channel(Fs, numSamps, TRUE, "Dev2","0", "4");

	Demod filter(Fs, numSamps); 	// Sets up the demodulation parameters

	Solver my_sensor;

	my_sensor.Setup();                // Models the coils
	my_sensor.ConfigureSolver();      // Sets the parameters of the solver (Max Iterations, minimum error....)
	my_sensor.SetCalibration(cal);    // Set the calibration values 

	vector <double> PandO(5); 	         // Create a vector to store position and orientation info

	OpenIGTLink my_server(port, fps);    // Create a server on the specified port 

	// ========= LOOP ================

	// SOLVE ONCE OUTSIDE LOOP USING INITIAL CONDITIONS
	// THEN USE DYNAMIC IC'S
	//my_channel.ReadSamples();

	////for (int i = 0; i < 100; i++)
	//	//cout << my_channel.my_result(i,0) << endl;

	//filter.demodulate_w_phase(numSamps, my_channel.my_result);

	////for (int i = 0; i < 8; i++)
	//	//cout << filter.magnitude_r[i] << endl;


	//PandO = my_sensor.Solve(filter.magnitude_r, initial_condition);



	
	//for (int i = 0; i < 9; i++)
	while(1)
	{
		
		// Reads the DAQ buffer into an Eigen matrix (variable my_result)
		my_channel.ReadSamples();

		filter.demodulate(numSamps, my_channel.my_result);
		//filter.demodulate_w_phase(numSamps, my_channel.my_result);


		for (int i = 0; i < 8; i++)
			cout << filter.magnitude_r[i] << endl;

		cout << endl << endl;

		// Initial condition here is previously solved point
		//sensor_flux(0) = -sensor_flux(0);
		//cout << "sf = " << endl << sensor_flux << endl ;

		////cout << "cal K = " << K << endl;
		//PandO = my_sensor.Solve(filter.magnitude_r, initial_condition);

		//cout << PandO[0] << " " << PandO[1] << " " << PandO[2] << " "
		//<< PandO[3] << " " << PandO[4] << endl << endl << endl;


		//my_server.SendMessage(PandO);

	}
	
	


	getchar();




}