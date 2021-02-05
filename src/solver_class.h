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

	int AcquireSamples(double Fs, double samples);

	VectorXd AcquirePeaks(int numSamples, double Ts);

	VectorXd Solve(VectorXd amplitudes, VectorXd initialGuess);

};

#endif