#include <iostream>
#include <string>
//#include "daq_class.h"
#include "../src/daq_class.h"
#include "demodulation_class.h"


#include <Dense>


using Eigen::MatrixXd;

using namespace std;


int main() {

	// ====== SETUP ======

	double Fs = 100e3;
	int numSamps = 1000;

	// Sets up the DAQ to sample channel 1 at Fs and fills a buffer of numSamps samples
	DAQ my_channel(Fs, numSamps);

	// Sets up demodulation parameters
	Demod filter(Fs, numSamps);
	


	// ====== LOOP ======


	// Reads the DAQ buffer into an Eigen matrix (variable my_result)
	// then demodulating this data


	for (int i = 0; i < 9; i++)
	{
		my_channel.ReadSamples();
		filter.demodulate(numSamps, my_channel.my_result);

		cout << filter.magnitude << endl;

	}

	getchar();

	return 0;


}