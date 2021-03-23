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



double constrainAngle(double x);

int sign(double x);

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

int Demod::demodulate_w_phase(double numSamples, MatrixXd sensor_and_coil_data) {

	// This function takes the sampled data of the sensor AND coil current


	// Demodulate each frequency component using this matrix fir method.
	Y = sensor_and_coil_data.transpose() * demod_matrix;

	// Calculate the amplitude of each component, both currentand magnetic field measurements are in here
	MagY = (2 * Y.array().abs()) / numSamples;

	// Calculate the phase angle of the field
	PhaseY = Y.array().arg();

	for (int i = 0; i < PhaseY.rows(); i++)
		for (int j = 0; j < PhaseY.cols(); j++)
			PhaseY(i, j) = constrainAngle(PhaseY(i, j));

	// Calculate the phase between the currentand the magnetic field.
	// This helps determine the axial orientation of the sensor.

	Phase1 = PhaseY.row(0) - PhaseY.row(1);

	for (int i = 0; i < 7; i++)
		if (abs(Phase1(i)) > M_PI)
			Phase1(i) = -sign(Phase1(i) * (2 * M_PI - abs(Phase1(i))));

	for (int i = 0; i < 7; i++)
		signs(i) = sign(Phase1(i));

	Bfield = signs.transpose().array() * MagY.row(1).array();


	for (int i = 0; i < 8; i++)
		magnitude_r.push_back(Bfield(i));

	return 0;
}


int sign(double x)
{
	if (x > 0) return 1;
	if (x < 0) return -1;
	return 0;
}

double constrainAngle(double x)
{
	x = fmod(x, 2 * M_PI);
	if (x < 0)
		x += 2 * M_PI;
	return x;
}