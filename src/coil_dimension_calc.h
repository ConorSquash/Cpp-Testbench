#ifndef COIL_DIMENSION_H
#define COIL_DIMENSION_H
#include <string>
#include <vector>

using namespace std;


void coil_dimension_calc(int N, double length, double width, double spacing, double thickness, double angle, vector<double>& X_points, vector<double>& Y_points, vector<double>& Z_points);


#endif