#ifndef SENSOR_OBJECTIVE_FUNCTION_H
#define SENSOR_OBJECTIVE_FUNCTION_H
#include <string>
#include <vector>

using namespace std;

#include <Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

void sensor_objective_function(VectorXd& currentPandO, VectorXd& fluxReal, MatrixXd& xcoil, MatrixXd& ycoil, MatrixXd& zcoil, VectorXd& calibration, VectorXd& out);


#endif