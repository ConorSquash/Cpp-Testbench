#include <iostream>
#include <string>
//#include "daq_class.h"
#include "../src/daq_class.h"
#include "demodulation_class.h"

#include <fstream>

#include <Dense>


using Eigen::MatrixXd;

using namespace std;


int main() {

	// ====== SETUP ======

	double Fs = 100e3;
	int numSamps = 5500;

	// Sample specified channels at Fs and fills a buffer of numSamps samples (finite or cont)
	DAQ my_channel(Fs, numSamps, TRUE, "Dev1" , "1", "15");

	// Sets up demodulation parameters
	Demod filter(Fs, numSamps);


	// Create text file to write data to
	ofstream results_file;
	ofstream results_file_demod;

	results_file.open("samples.txt");
	results_file_demod.open("demodulated.txt");

	while (1)
	{

		my_channel.ReadSamples();

		//cout << "Channel 1 " << my_channel.my_result(0,0) << endl;
		//cout << "Channel 15 " << my_channel.my_result(40, 1) << endl << endl;

		//cout << "Rows : " << my_channel.my_result.rows() << endl;

		//results_file << my_channel.my_result.col(0) << endl;

	
		filter.demodulate(numSamps, my_channel.my_result);

		//cout << filter.magnitude_r[5];

		for (int i = 0; i < 8; i++)
			//results_file_demod << filter.magnitude_r[i] << endl;
			cout << filter.magnitude_r[i] << endl;

		cout << endl;
	}

	



	

	getchar();

	return 0;


}