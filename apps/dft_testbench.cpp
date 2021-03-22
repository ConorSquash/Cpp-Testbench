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

double wrapMax(double x, double max);

double constrainAngle(double x);


int main() {

	// Specify the sampling frequency per sensor channel

	double Fs = 100e3;
	double Ts = 1 / Fs;
	int numSamples = 5000;


	// Specify the number of time samples, must be the same as the length of X

	VectorXd t(numSamples);

	for (int i = 0; i < numSamples; i++)
		t(i) = (i * Ts);


	// Create an example signal composed of a 20kHz and 15kHz sine wave

	VectorXd x1_test(numSamples), x2_test(numSamples), x_test(numSamples);

	x1_test = 2 * M_PI * 2000 * t;
	x1_test = x1_test.array().sin();

	x2_test = 2 * M_PI * 2000 * t;
	x2_test = x2_test.array() + 10;
	x2_test = x2_test.array().sin();

	x_test = x1_test + x2_test;



	// Define the transmission frequencies of the emitter coil
	// These will be used for demodulation

	VectorXd F(8);

	F << 2000, 5000, 10000, 26000, 28000, 30000, 15000, 34000;


	// Define the demodulation matrix for the asynchronous demodulation scheme

	MatrixXcd E(8, numSamples);   // Matrix of complex doubles


	for (int j = 0; j < 8; j++)
		E.row(j) = exp(2 * M_PI * F(j) * t.array() * 1i);


	//for (int i = 0; i < 20; i++)
		//cout << E(0,i).imag() << endl;

	E.transposeInPlace();		// Must transpose in place when replacing with a transpose of itself!!! 
								// Same as E = E.transpose().eval();

	VectorXcd result(8), magnitude(8);

	result = x_test.transpose() * E;

	magnitude = (2 * result.array().abs()) / numSamples;

	//cout << "Magnitude of each frequency component : " << endl << magnitude << endl;;
	


	//============================================================


	MatrixXd X(numSamples, 2);

	X.col(0) = x1_test;
	X.col(1) = x2_test;

	X.transposeInPlace();

	//for (int i = 0; i < 20; i++)
		//cout << E(i,0) << endl;

	
	//std::complex<double> z1 = 3.0 + 4i;   
	//cout << std::arg(z1);

	MatrixXcd Y;
	MatrixXd MagY,PhaseY;

	Y = X * E;

	//cout << Y << endl << endl;

	MagY = 2 * Y.array().abs();

	//cout << MagY << endl << endl;
	
	PhaseY = Y.array().arg();

	cout << PhaseY << endl << endl;


	for (int i = 0; i < PhaseY.rows(); i++)
		for (int j = 0; j < PhaseY.cols(); j++)
			PhaseY(i, j) = constrainAngle(PhaseY(i, j));

	cout << PhaseY << endl << endl;

		
		


	getchar();

	return 0;
}

double wrapMax(double x, double max)
{
	return fmod(max + fmod(x, max), max);		// integer math: `(max + x % max) % max` 
}

double constrainAngle(double x) {
	x = fmod(x, 2*M_PI);
	if (x < 0)
		x += 2 * M_PI;
	return x;
}


