#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <complex>
#include <iterator>
#include <fstream>
#include <sstream>



#define _USE_MATH_DEFINES
#include "math.h"
#include <cmath>


//#include <Eigen/Dense>
#include <Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;


using Eigen::MatrixXd;
using Eigen::VectorXd;

using Eigen::MatrixXcd;
using Eigen::VectorXcd;

using namespace std;
using namespace literals;



int main() {

	// Specify the sampling frequency per sensor channel

	double Fs = 100e3;
	double Ts = 1 / Fs;
	int numSamples = 1000;





	// Specify the number of time samples, must be the same as the length of X

	VectorXd t(numSamples);

	for (int i = 0; i < numSamples; i++)
		t(i) = (i * Ts);




	// Create an example signal composed of a 20kHz and 15kHz sine wave

	VectorXd x1_test(numSamples), x2_test(numSamples), x_test(numSamples);

	x1_test = 2 * M_PI * 20000 * t;
	x1_test = x1_test.array().sin();

	x2_test = 2 * M_PI * 15000 * t;
	x2_test = x2_test.array().sin();

	x_test = x1_test + x2_test;



		// Define the transmission frequencies of the emitter coil
		// These will be used for demodulation

		VectorXd F(8);

		F << 20000, 5000, 24000, 15000, 28000, 30000, 15000, 34000;


		// Define the demodulation matrix for the asynchronous demodulation scheme

		MatrixXcd E(8, numSamples);   // Matrix of complex doubles


		for (int j = 0; j < 8; j++)
			E.row(j) = exp(2 * M_PI * F(j) * 1i * t.array());


		E.transposeInPlace();		// Must transpose in place when replacing with a transpose of itself!!! 
									// Same as E = E.transpose().eval();



		VectorXcd result(8), magnitude(8);

		result = x_test.transpose() * E;
		//result = square_test.transpose() * E;


		magnitude = (2 * result.array().abs()) / numSamples;

		cout << "Magnitude of each frequency component : " << endl;
		//cout << "Mag is " << magnitude.rows() << " by " << magnitude.cols() << endl;
		cout << magnitude;

		getchar();

		return 0;
}



