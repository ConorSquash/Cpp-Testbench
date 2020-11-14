#include "coil_spec.h"
#include <string>
#include <iostream>
#include <array>
#include <vector>

#define _USE_MATH_DEFINES
#include "math.h"

#include "coil_dimension_calc.h"


using namespace std;

string exactChoice = "EXACT";
string fastChoice = "FAST";

//const double pi = 3.14159265358979323846;


void coil_spec(double len, double wid, double space, double thickness, double angle, int Nturns, string ModelType, vector<double>& X_points, vector<double>& Y_points, vector<double>& Z_points) {

	if (exactChoice.compare(ModelType) == 0)
	{
		//cout << "You have chosen the exact option" << endl;
		coil_dimension_calc(Nturns,len,wid,space,thickness,angle,X_points, Y_points, Z_points);

	}
	else
	{
		//cout << "You have chosen the fast option" << endl;

		if (angle == M_PI_2)
		{
			//cout << "Angle is pi/2" << endl;
			X_points = {-61.5e-03 / 2, 61.5e-03 / 2, 61.5e-03 / 2, -61.5e-03 / 2, -61.5e-03 / 2 };
			Y_points = {-61.5e-03 / 2, -61.5e-03 / 2, 61.5e-03 / 2, 61.5e-03 / 2, -61.5e-03 / 2 };
			Z_points = {-1.6e-03 / 2, -1.6e-03 / 2, -1.6e-03 / 2, -1.6e-03 / 2, -1.6e-03 / 2 };

		}
		else if(angle == M_PI_4)
		{
			//cout << "Angle is pi/4" << endl;
			X_points = {-61.5e-03 / M_SQRT2, 0, 61.5e-03 / M_SQRT2, 0, -61.5e-03 / M_SQRT2 };
			Y_points = {0, -61.5e-03 / M_SQRT2, 0, 61.5e-03 / M_SQRT2, 0 };
			Z_points = {-1.6e-03 / 2, -1.6e-03 / 2, -1.6e-03 / 2, -1.6e-03 / 2, -1.6e-03 / 2 };
	
		}
		
		
	}


	return;
}