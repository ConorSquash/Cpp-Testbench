#ifndef SOLVER_CLASS_H
#define SOLVER_CLASS_H
#include <string>
#include <vector>
#include <Dense>
#include <Eigen>

using namespace std;
using namespace Eigen;


using Eigen::MatrixXd;

//PUT CLASS DECLARATION HERE
class Solver
{
private:

public:
	void printmessage();
	VectorXcd AcquirePeaks(int numSamples, double Ts);
	//int ConfigureDAQ(double DAQfrequency, int no_of_samples, int no_of_chans);
	int AcquireSamples(double Fs, double samples);
};

#endif