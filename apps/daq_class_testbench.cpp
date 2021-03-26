#include <iostream>
#include <string>
//#include "daq_class.h"
#include "../src/daq_class.h"
#include "demodulation_class.h"

#include <fstream>

#include <Dense>


using Eigen::MatrixXd;

using namespace std;

extern MatrixXd buffer_result;




int main() {

	// ====== SETUP ======

	double Fs = 100e3;
	int numSamps = 1000;

	// Sample specified channels at Fs and fills a buffer of numSamps samples (finite or cont)
	DAQ my_channel(Fs, numSamps, FALSE, "Dev2" , "0", "4");

	// Sets up demodulation parameters
	Demod filter(Fs, numSamps);


	// Create text file to write data to
	/*
	ofstream results_file;
	ofstream results_file_demod;
	results_file.open("samples.txt");
	results_file_demod.open("demodulated.txt");
	*/



	
	
	while (1)
	{

		filter.demodulate(numSamps, buffer_result);

		for (int i = 0; i < 8; i++)
			cout << filter.magnitude_r[i] << endl;

		cout << endl;

	}
	

	



	

	getchar();

	return 0;


}