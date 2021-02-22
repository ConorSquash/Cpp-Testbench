#include "sensor_objective_function.h"
#include "field_coil_calc.h"
#include <string>
#include <iostream>
#include <array>
#include <vector>


using namespace std;

#include <Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

 /* Objective function to calculate the difference between the modelledand measured magnetic flux values due to a single pcb emitter coil.
    This is the 'Cost' function for the LM algorithm.Each iteration of LMA executes this function 8 times, once for each coil.

    currentPandO = The current position and orientation vector.This vector is the variable parameter of the objective function.
   
    fluxReal = A scaler indicating the sensed magnetic flux the sensor.

    out = The difference between the sensed and calculated fluxes due to a single coil   */





void sensor_objective_function(VectorXd& currentPandO, VectorXd& fluxReal, MatrixXd & xcoil, MatrixXd & ycoil, MatrixXd & zcoil, VectorXd& calibration, VectorXd& out)
{

    double x = currentPandO(0);
    double y = currentPandO(1);
    double z = currentPandO(2);
    double theta = currentPandO(3);
    double phi = currentPandO(4);

    //cout << "Solving for a sensor flux of : \n" << fluxReal << endl << endl;

    VectorXd Hx(8), Hy(8), Hz(8);   // Create the vectors that will be returned from field_coil_calc                                           

    field_coil_calc(1, xcoil, ycoil, zcoil, x, y, z, Hx, Hy, Hz);

    VectorXd fluxModel(8);

    //calibration.transposeInPlace();

    fluxModel = calibration.array() * ((Hx.array() * sin(theta) * cos(phi)) + (Hy.array() * sin(theta) * sin(phi)) + (Hz.array() * cos(theta))); 

    out = fluxModel - fluxReal;


}
