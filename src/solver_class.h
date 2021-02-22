#ifndef SOLVER_CLASS_H
#define SOLVER_CLASS_H

#include <vector>


using namespace std;


//PUT CLASS DECLARATION HERE
class Solver
{
private:

public:
	
	int Setup();
	int ConfigureSolver();
	int SetCalibration(vector <double> k);
	vector <double> generate_points(double points, double spacing, bool isZ);
	vector <double> Solve(vector <double> amplitudes, vector <double> initialGuess);

};

#endif