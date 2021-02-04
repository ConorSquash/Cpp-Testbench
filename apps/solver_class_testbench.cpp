#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <chrono>

#define _USE_MATH_DEFINES
#include "math.h"

#include "coil_spec.h"
#include "field_coil_calc.h"
#include "sensor_objective_function.h"

#include "lsqcpp.h"
#include "solver_class.h"


#include <Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

using namespace std;
using namespace std::chrono;



int main() {

	Solver my_solver;

	double Fs = 100e3;
	double Ts = 1 / Fs;
	int numSamps = 5000;
	int chans = 1;

	// Congigure the DAQs sampling frequency, sample size and channels
	my_solver.ConfigureDAQ(Fs, numSamps, chans);

	//cout << "\n Press a key to extract peak values." << endl;
	//getchar();

	VectorXcd peaks(8);

	// Extract the buffer from the DAQ into Eigen Matrix, demodulate
	// and return a vector of the peak amplitudes
	peaks = my_solver.AcquirePeaks(numSamps,Ts,chans);

	cout << "\n\n Magnitude of each frequency component : " << endl;
	cout << peaks << endl;


	getchar();


}