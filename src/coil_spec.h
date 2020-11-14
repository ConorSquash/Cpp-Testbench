#ifndef COIL_SPEC_H
#define COIL_SPEC_H
#include <string>
#include <vector>

using namespace std;


void coil_spec(double len, double wid, double space, double thickness, double angle, int Nturns, string ModelType,vector<double> &X_points, vector<double>& Y_points, vector<double>& Z_points);


#endif
