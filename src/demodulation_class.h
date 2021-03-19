#ifndef DEMODULATION_CLASS_H
#define DEMODULATION_CLASS_H
#include <Dense>

using Eigen::MatrixXd;
using Eigen::VectorXcd;
using Eigen::MatrixXcd;

class Demod
{
public:

	Demod(double frequency, int samples);

	int demodulate(double numSamples, MatrixXd buffer_result_d);

	VectorXcd magnitude;

	MatrixXcd demod_matrix;

private:

	VectorXcd result;

	double Ts;






};






#endif