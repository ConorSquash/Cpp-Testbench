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

	Ts = 1.0 / Fs;

	// Specify the number of time samples, must be the same as the length of X
	VectorXd t(numSamples);

	for (int i = 0; i < numSamples; i++)
		t(i) = ((double)i * Ts);

	// Define the transmission frequencies of the emitter coil
	VectorXd F(8);

	// FREQS FOR DEBUG WITH ARDUINO
	//F << 4860, 22000, 24000, 26000, 28000, 30000, 32000, 34000;

	// COIL FREQS FOR ANSER
	F << 20000, 22000, 24000, 26000, 28000, 30000, 32000, 34000;

	MatrixXcd E(8, numSamples);   // Matrix of complex doubles

	// Define the demodulation matrix for the asynchronous demodulation scheme

	for (int j = 0; j < 8; j++)
		E.row(j) = exp(2 * M_PI * F(j) * 1i * t.array());

	E.transposeInPlace();		// Must transpose in place when replacing with a transpose of itself!!! 
								// Same as E = E.transpose().eval();

	demod_matrix = E;

	magnitude_r.resize(8);
	signs.resize(8);


}


int Demod::demodulate(double numSamples, MatrixXd buffer_result_d) {

	// This function takes the sampled data and demodulates it

	// The first column is the sampled data of the first channel
	result = buffer_result_d.col(0).transpose() * demod_matrix;     

	magnitude_c = (2 * result.array().abs()) / numSamples;

	//magnitude_r.resize(8);


	for (int i = 0; i < 8; i++)
		magnitude_r[i] = magnitude_c.real()(i);

	return 0;
}

int Demod::demodulate_w_phase(double numSamples, MatrixXd sensor_and_coil_data) {

	// This function takes the sampled data of the sensor AND coil current


	// Demodulate each frequency component using this matrix fir method.
	Y = sensor_and_coil_data.transpose() * demod_matrix;

	//TRY
	//sensor_and_coil_data.transposeInPlace();
	//Y = sensor_and_coil_data * demod_matrix;

	// Calculate the amplitude of each component, both currentand magnetic field measurements are in here
	MagY = (2 * Y.array().abs()) / numSamples;

	MagY_c = (2 * Y.array().abs()) / numSamples;

	//cout << "MagY(0, 0) = " << MagY(0, 0) << endl;

	// Calculate the phase angle of the field
	PhaseY = Y.array().arg();

	for (int i = 0; i < PhaseY.rows(); i++)
		for (int j = 0; j < PhaseY.cols(); j++)
			PhaseY(i, j) = constrainAngle(PhaseY(i, j));

	// Calculate the phase between the currentand the magnetic field.
	// This helps determine the axial orientation of the sensor.

	Phase1 = PhaseY.row(0) - PhaseY.row(1);

	for (int i = 0; i < Phase1.rows(); i++)
		for (int j = 0; j < Phase1.cols(); j++)
			Phase1(i, j) = constrainAngle(Phase1(i, j));

	//signs.resize(8);

	for (int i = 0; i < 8; i++)
		//if (abs(Phase1(i)) > M_PI)
		signs(i) = sign(Phase1(i)); //* (2 * M_PI - abs(Phase1(i))));

	

	//for (int i = 0; i < 8; i++)
	//	signs(i) = sign(Phase1(i));

	Bfield = signs.transpose().array() * MagY.row(0).array();

	//magnitude_r.resize(8);

	//cout << "Bfield(0) = " << Bfield(0) << endl;

	for (int i = 0; i < 8; i++)
		magnitude_r[i] = Bfield(i);

	return 0;
}


int sign(double x)
{
	if (x > M_PI) return 1;
	if (x < M_PI) return -1;
	return 0;
}

double constrainAngle(double x)
{
	x = fmod(x, 2 * M_PI);
	if (x < 0)
		x += 2 * M_PI;
	return x;
}