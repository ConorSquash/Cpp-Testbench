#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <Dense>
#include <thread>

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

using Eigen::MatrixXd;
using Eigen::VectorXd;

using Eigen::MatrixXcd;
using Eigen::VectorXcd;

using namespace std;

extern VectorXd sensor_flux;
extern VectorXd K;

extern MatrixXd buffer_result;


int main() {


	// ========= SETUP ================

	double Fs = 100e3;
	double numSamps = 1000;

	vector <double> cal = { -2.5968, - 1.6552, - 1.0803, - 0.7937, - 0.5783, - 0.4606, - 0.3466, - 0.2964 };
	//vector <double> cal = { 2.5968, 1.6552, 1.0803, 0.7937, 0.5783, 0.4606, 0.3466, 0.2964 };

	int port = 18944;
	float fps = 1000;

	// Provide an initial guess for x,y,z,theta,phi
	vector <double> initial_condition = { 0, 0, 0.2, M_PI_2 , M_PI_2 };

	// Sets up the DAQ to sample channel 1 at Fs and fills a buffer of numSamps samples
	//DAQ my_channel(Fs, numSamps, TRUE, "Dev1", "0", "4");
	DAQ my_channel(Fs, numSamps, FALSE, "Dev2","0", "4");

	Demod filter(Fs, numSamps); 	// Sets up the demodulation parameters

	Solver my_sensor;

	my_sensor.Setup();                // Models the coils
	my_sensor.ConfigureSolver();      // Sets the parameters of the solver (Max Iterations, minimum error....)
	my_sensor.SetCalibration(cal);    // Set the calibration values 

	vector <double> PandO(5); 	         // Create a vector to store position and orientation info

	OpenIGTLink my_server(port, fps);    // Create a server on the specified port 

	// ========= LOOP ================


	int count = 0;
	double total_time = 0;
	
	
	auto start = high_resolution_clock::now();		// Get starting timepoint
	
	while(count < 100)
	{

		//std::this_thread::sleep_for(std::chrono::seconds(1));


		

		filter.demodulate(numSamps, buffer_result);
		//filter.demodulate_w_phase(numSamps, buffer_result);



		//for (int i = 0; i < 8; i++)
		//	cout << filter.magnitude_r[i] << endl;

		//cout << endl << endl;



		// Initial condition here is previously solved point
		//sensor_flux(0) = -sensor_flux(0);
		//cout << "sf = " << endl << sensor_flux << endl ;


		//cout << "cal K = " << K << endl;
		PandO = my_sensor.Solve(filter.magnitude_r, initial_condition);

		cout << "\n -> SOLVED P&O " << endl;
		cout << " x : " << PandO[0] * 100 << " cm" << endl;
		cout << " y : " << PandO[1] * 100 << " cm" << endl;
		cout << " z : " << PandO[2] * 100 << " cm" << endl;
		cout << " Pitch : " << PandO[3] << " rads " << endl;
		cout << " Yaw : " << PandO[4]  << " rads " << endl << endl;


		my_server.SendMessage(PandO);




		count++;

	}
	
	auto stop = high_resolution_clock::now();        // Get stopping timepoint

	auto duration = duration_cast<milliseconds>(stop - start);    	 // Get duration by subtracting timepoints 

	total_time = total_time + duration.count();

	cout << "System rate : " << (count / total_time) * 1000 << " Hz " << endl;

	
	


	getchar();




}