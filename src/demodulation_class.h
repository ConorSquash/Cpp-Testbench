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

	int demodulate_w_phase(double numSamples, MatrixXd sensor_and_coil_data);

	std::vector<double> magnitude_r;

	MatrixXcd demod_matrix;

private:

	VectorXcd result;
	VectorXcd magnitude_c;
	VectorXcd PhaseY;

	double Ts;






};






#endif