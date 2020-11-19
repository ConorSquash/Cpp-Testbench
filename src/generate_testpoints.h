#ifndef GENERATE_TESTPOINTS_H
#define GENERATE_TESTPOINTS_H
#include <string>
#include <vector>
#include <Dense>
#include <Eigen>

using namespace std;
using namespace Eigen;


using Eigen::MatrixXd;

void generate_testpoints(int x_points, int y_points, int z_points, double x_spacing, double y_spacing, double z_spacing, VectorXd& x_values, VectorXd& y_values, VectorXd& z_values);


#endif