#ifndef FIELD_COIL_CALC_H
#define FIELD_COIL_CALC_H
#include <string>
#include <vector>
#include <Dense>
#include <Eigen>

using namespace std;
using namespace Eigen;


using Eigen::MatrixXd;

void field_coil_calc(double I, MatrixXd x_points, MatrixXd y_points, MatrixXd z_points, double Px, double Py, double Pz, VectorXd& Hx, VectorXd& Hy, VectorXd& Hz);


#endif
