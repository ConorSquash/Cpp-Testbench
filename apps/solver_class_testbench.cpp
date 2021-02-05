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

	Solver my_sensor;

	double Fs = 100e3;
	double Ts = 1 / Fs;
	int numSamps = 1000;
	int chans = 1;


	my_sensor.AcquireSamples(Fs,numSamps);

	VectorXcd peaks(8);

	// Return a vector of the peak amplitudes
	peaks = my_sensor.AcquirePeaks(numSamps,Ts);

	cout << "\n Magnitude of each frequency component : " << endl;
	cout << peaks << endl;


	getchar();


}