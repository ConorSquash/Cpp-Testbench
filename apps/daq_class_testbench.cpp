#include <iostream>
#include <string>
//#include "daq_class.h"
#include "../src/daq_class.h"

#include <Dense>


using Eigen::MatrixXd;

using namespace std;


int main() {


	double Fs = 100e3;
	int numSamps = 1000;

	// Sets up the DAQ to sample channel 1 at Fs and fills a buffer of numSamps samples
	DAQ my_channel(Fs, numSamps);
	
	//MatrixXd my_samples;

	my_channel.ReadSamples();

	//cout << "Sample 1 : " << my_samples(0,0) << endl;


	

	getchar();


}