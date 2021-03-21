#ifndef DEMODULATION_CLASS_H
#define DEMODULATION_CLASS_H
#include <Dense>
#include <vector>

using Eigen::MatrixXd;
using Eigen::VectorXcd;
using Eigen::MatrixXcd;

class Demod
{
public:

	Demod(double frequency, int samples);

	int demodulate(double numSamples, MatrixXd buffer_result_d);

	std::vector<double> magnitude_r;

	MatrixXcd demod_matrix;

private:

	VectorXcd result;
	VectorXcd magnitude_c;

	double Ts;






};






#endif