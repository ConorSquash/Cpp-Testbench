#include <iostream>
#include <string>
#include "solver_class.h"
#include <Dense>

using Eigen::VectorXd;
using namespace std;


int main() {

	Solver my_sensor;

	//SAMPLE
	double Fs = 100e3;
	double Ts = 1 / Fs;
	int numSamps = 1000;
	int chans = 1;

	my_sensor.AcquireSamples(Fs,numSamps);   // Creates and fills a buffer of numSamps samples within the DAQ


	//DEMODULATE
	VectorXd peaks(8);
	peaks = my_sensor.AcquirePeaks(numSamps,Ts); 	// Return a vector of the peak amplitudes

	cout << "\n Magnitude of each frequency component : " << endl;
	cout << peaks << endl;


	// SOLVE
	VectorXd initialGuess(5); 					// Provide an initial guess for x,y,z,theta,phi
	initialGuess << 0.125, 0.13, 0.2, 1, 0;

	VectorXd PandO(5); 	                        // Create a vector to store position and orientation info

	PandO = my_sensor.Solve(peaks , initialGuess);   // Pass initial guess and sensor flux and then solve

	cout << "\n -> SOLVED P&O : " << endl;
	cout << " x : " << PandO(0) << endl;
	cout << " y : " << PandO(1) << endl;
	cout << " x : " << PandO(2) << endl;
	cout << " Pitch : " << PandO(3) << endl;
	cout << " Yaw : " << PandO(4) << endl;

	getchar();


}