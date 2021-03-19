#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <Dense>


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



	double Fs = 100e3;
	double numSamps = 1000;

	// ====== DAQ ======

	// Sets up the DAQ to sample channel 1 at Fs and fills a buffer of numSamps samples
	DAQ my_channel(Fs, numSamps);

	// Reads the DAQ buffer into an Eigen matrix (variable my_result)
	my_channel.ReadSamples();

	cout << my_channel.my_result << endl;



	// ====== DEMOD ======

	//Demod filter(Fs, numSamps);

	//filter.demodulate(numSamps, my_channel.my_result);

	//cout << filter.magnitude << endl;


}