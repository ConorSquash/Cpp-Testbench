#include "demodulation_class.h"
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <complex>
#include <iterator>

#define _USE_MATH_DEFINES
#include "math.h"
#include <cmath>

#include <Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

using Eigen::MatrixXd;
using Eigen::VectorXd;

using Eigen::MatrixXcd;
using Eigen::VectorXcd;

using namespace std;
using namespace literals;


Demod::Demod(double Fs, int numSamples)
{
	cout << "\n -> SETTING DEMOD PARMS " << endl;

	Ts = 1 / Fs;

	// Specify the number of time samples, must be the same as the length of X
	VectorXd t(numSamples);

	for (int i = 0; i < numSamples; i++)
		t(i) = (i * Ts);

	// Define the transmission frequencies of the emitter coil
	VectorXd F(8);

	F << 2500, 4860, 8500, 11020, 12500, 17888, 20000, 30000;

	MatrixXcd E(8, numSamples);   // Matrix of complex doubles

	// Define the demodulation matrix for the asynchronous demodulation scheme

	for (int j = 0; j < 8; j++)
		E.row(j) = exp(2 * M_PI * F(j) * 1i * t.array());

	E.transposeInPlace();		// Must transpose in place when replacing with a transpose of itself!!! 
								// Same as E = E.transpose().eval();

	demod_matrix = E;

}


int Demod::demodulate(double numSamples, MatrixXd buffer_result_d) {

	// This function takes the sampled data and demodulates it

	// The first column is the sampled data of the first channel
	result = buffer_result_d.col(0).transpose() * demod_matrix;     

	magnitude_c = (2 * result.array().abs()) / numSamples;

	for (int i = 0; i < 8; i++)
		magnitude_r.push_back(magnitude_c.real()(i));

	return 0;
}



