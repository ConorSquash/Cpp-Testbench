#ifndef DEMODULATION_CLASS_H
#define DEMODULATION_CLASS_H
#include <Dense>
#include <vector>

using Eigen::MatrixXd;
using Eigen::VectorXcd;
using Eigen::MatrixXcd;
using Eigen::VectorXd;

class Demod
{
public:

	Demod(double frequency, int samples);

	int demodulate(double numSamples, MatrixXd buffer_result_d);

	int demodulate_w_phase(double numSamples, MatrixXd sensor_and_coil_data);

	std::vector<double> magnitude_r;
	std::vector<double> b_field;


	MatrixXcd demod_matrix;

private:

	VectorXcd result;
	VectorXcd magnitude_c;

	MatrixXcd Y;
	MatrixXd MagY, PhaseY;

	MatrixXcd MagY_c;

	VectorXd Phase1;
	VectorXd signs, Bfield;

	double Ts;






};






#endif