#ifndef SOLVER_CLASS_H
#define SOLVER_CLASS_H

#include <vector>

#include <Dense>

using Eigen::VectorXd;



using namespace std;


//PUT CLASS DECLARATION HERE
class Solver
{
private:

	std::vector <double> PandO;    // Convert from eigen to standard C++ vector
	int iterations;
	VectorXd initialGuess;

	double x_min;
	double y_min;
	double z_min;
	double theta_min;
	double phi_min;

	double x_max;
	double y_max;
	double z_max;
	double theta_max;
	double phi_max;

	vector <double> PandO_reset;

public:
	
	int Setup();
	int ConfigureSolver();
	int SetCalibration(vector <double> k);
	vector <double> generate_points(double points, double spacing, bool isZ, double offset);
	vector <double> Solve(vector <double> amplitudes, vector <double> initialGuess);

};

#endif