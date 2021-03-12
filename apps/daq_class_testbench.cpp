#include <iostream>
#include <string>
//#include "daq_class.h"
#include "../src/daq_class.h"



using namespace std;


int main() {

	//SAMPLE

	double Fs, samples;
	DAQ my_channel(Fs, samples);
	
	
	double Fs = 100e3;
	//double Ts = 1 / Fs;
	int numSamps = 1000;
	//int chans = 1;

	my_channel.DemodSetup(Fs,numSamps);

	my_channel.AcquireSamples(Fs,numSamps);   // Creates and fills a buffer of numSamps samples within the DAQ


	

	getchar();


}