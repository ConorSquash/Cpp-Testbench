#include "generate_testpoints.h"
#include <string>
#include <iostream>
#include <array>
#include <vector>

#define _USE_MATH_DEFINES
#include "math.h"

#include <Dense>
#include <Eigen>

using Eigen::MatrixXd;
using Eigen::VectorXd;

using namespace std;
using namespace Eigen;

using namespace std;

void generate_testpoints(int x_points, int y_points, int z_points, double x_spacing, double y_spacing, double z_spacing, VectorXd& x_values, VectorXd& y_values, VectorXd& z_values)
{
	 //  generate_testpoints() takes 7 inputs : 
	 //  The no. of xyz points, the xyz spacing in metres and a matrix to return the points
	 //	 xyz_points contains all the test points for the solver.
	 //  It has 3 rows (one for each dimension) and however many columns it requires

	// Calculate the initial x and y point
	double current_x = -((x_spacing / 2) + x_spacing * ((x_points / 2) - 1));
	double current_y = -((y_spacing / 2) + y_spacing * ((y_points / 2) - 1));

	// Initial z point is slighly above the Field Generator
	double current_z = 0.1;

	

	// Generate x points

	for (int i = 0; i < x_points; i++)
	{
		x_values(i) = current_x;

		current_x = current_x + x_spacing;
	}


	// Generate y points

	for (int i = 0; i < y_points; i++)
	{

		y_values(i) = current_y;

		current_y = current_y + y_spacing;
	}


	// Generate z points

	for (int i = 0; i < z_points; i++)
	{

		z_values(i) = current_z;

		current_z = current_z + z_spacing;
	}

	
}