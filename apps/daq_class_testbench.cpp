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

	// Sample specified channels at Fs and fills a buffer of numSamps samples (finite or cont)
	DAQ my_channel(Fs, numSamps, TRUE, "1", "15");

	my_channel.ReadSamples();

	cout << "Channel 1 " << my_channel.my_result(0,0) << endl;
	cout << "Channel 15 " << my_channel.my_result(40, 1) << endl;


	// Sets up demodulation parameters
	Demod filter(Fs, numSamps);
	
	filter.demodulate(numSamps, my_channel.my_result);

	cout << filter.magnitude_r[1] << endl;

	

	getchar();

	return 0;


}