#include "coil_dimension_calc.h"
#include <string>
#include <iostream>
#include <array>
#include <vector>

#define _USE_MATH_DEFINES
#include "math.h"

using namespace std;

void coil_dimension_calc(int N, double length, double width, double spacing, double thickness, double angle, vector<double>& X_points, vector<double>& Y_points, vector<double>& Z_points)
{
	  /* N = Number of desired coil turns (N is odd)
		 length = Length of the longest straight segment of copper track (Also known as the seed length)
		 width = The desired width of the copper track
		 spacing = The desired spacing between each copper track
		 thickness = Thickness of the printed circuit board	*/


		
	double z_thick = thickness;    // PCB thickness
	double theta_rot = angle;      // Coil rotation angle (45 or 90 degrees)

	// Define dimensions
	double l = length;
	double w = width;
	double s = spacing;

	double ll_s = w + s;           // Line to line spacing 

	 //Start point is bottom left corner of spiral, define this as the origin (0, 0)

	  // Note: each turn requires 5 points, the next square starts at the last
	  // point of the previous square
	  // Total number of points equals(4 * N) + 1
	
	int Points_total = (4 * N) + 1;

	vector<double> x_points, y_points, z_points;

	for (int i = 0; i < Points_total; i++)
	{
		x_points.push_back(0);
		y_points.push_back(0);
		z_points.push_back(0);

		//cout << i << "  : " << x_points[i] << endl;
	}

	for (int i = 51; i < Points_total; i++)
		z_points[i] = -z_thick;

	//for (int i = 0; i < Points_total; i++)
	//	cout << i << "  : " << z_points[i] << endl;
	
//====================================================================================

	 // Define an array that contains the length of each segment, this follows
     // a pattern after the first two, there are  N * 4 line segments

	vector<double> line_segs;

	for (int i = 0; i < 4*N; i++)
		line_segs.push_back(0);

	for (int i = 0; i < 3; i++)     // The first 3 outer filaments are the specified length
		line_segs[i] = l;            // Line segments then start to get smaller (inwards spiral)

	int i_bigger = 1;   // Lets us increment the decrease in length

	for (int i = 3; i < (2*N); i = i+2)
	{
		line_segs[i] = l - (i_bigger * ll_s);
		line_segs[i+1] = l - (i_bigger * ll_s);

		i_bigger += 1;
	}

	int i_smaller = i_bigger - 1;                  // Coil now gets bigger again for the other side of the PCB

	for (int i = (2*N) - 1; i < (4*N) - 1; i = i+2)
	{
		line_segs[i] = l - (i_smaller * ll_s);
		line_segs[i+1] = l - (i_smaller * ll_s);
		i_smaller = i_smaller - 1;
		i_bigger += 1;

	}

	line_segs[(4 * N) - 3] = l;        // Last 3 line segments dont follow the pattern
	line_segs[(4 * N) - 2] = l;
	line_segs[(4 * N) - 1] = l - ll_s;

	
	//for (int i = 0; i < 4 * N; i++)
	//	cout << i << "  : " << line_segs[i] << endl;


//====================================================================================

	    // Make an array of trajectories that shows the direction to travel from
		// one point to another this repeats for every square
		// varies r(x, y) = cos(theta)x_hat + sin(theta)y_hat, moving through
		// "up,left,down,right" or 90 180 270 0


	vector<double> x_traj,y_traj;

	x_traj = {theta_rot,theta_rot + M_PI_2,theta_rot + M_PI,theta_rot + (3 * M_PI_2)};
	y_traj = {theta_rot,theta_rot + M_PI_2,theta_rot + M_PI,theta_rot + (3 * M_PI_2)};

	for (int i = 0; i < x_traj.size(); i++)
	{
		x_traj[i] = cos(x_traj[i]);
		y_traj[i] = sin(y_traj[i]);
	}

	int q = 0;

	for (int i = 1; i < Points_total; i++)
	{
		x_points[i] = (x_traj[q] * line_segs[i-1]) + x_points[i-1];      // q cycles through the four trajectories and repeats
		y_points[i] = (y_traj[q] * line_segs[i-1]) + y_points[i-1];

		q += 1;

		if (q == 4)
			q = 0;
		
	}


//	for (int i = 0; i < Points_total; i++)
	//	cout << i << "  : " << x_points[i] << endl;

//====================================================================================
	
	// Add actual z offset due to PCB thickness

	vector<double> x_points_new, y_points_new, z_points_new;

	for (int i = 0; i < Points_total + 1; i++)
	{
		x_points_new.push_back(0);
		y_points_new.push_back(0);
		z_points_new.push_back(0);
	}


	for (int i = 0; i < (2 * N) + 1; i++)
	{
		x_points_new[i] = x_points[i];
		y_points_new[i] = y_points[i];
	}

	x_points_new[(2 * N) + 1] = x_points[2 * N];
	y_points_new[(2 * N) + 1] = y_points[2 * N];

	for (int i = (2 * N) + 2; i < Points_total + 1; i++)
	{
		x_points_new[i] = x_points[i - 1];
		y_points_new[i] = y_points[i - 1];
	}

	for (int i = 0; i < (2 * N) + 2; i++)
		z_points_new[i] = z_points[i];

	z_points_new[(2 * N) + 2] = z_points[(2 * N) + 1];

	for (int i = (2 * N) + 3; i < Points_total + 1; i++)
		z_points_new[i] = z_points[i - 1];


	//for (int i = 0; i < x_points_new.size(); i++)
	//	cout << i << "  : " << z_points_new[i] << endl;


	// Let generated points equal output of function
	X_points = x_points_new;
	Y_points = y_points_new;
	Z_points = z_points_new;



	// Now offset back to centre
	if (theta_rot == M_PI_2)
	{
		for (int i = 0; i < X_points.size(); i++)
		{
			X_points[i] = X_points[i] + length / 2;
			Y_points[i] = Y_points[i] - length / 2;
		}
	}
	else
		for (int i = 0; i < Y_points.size(); i++)
			Y_points[i] = Y_points[i] - length / sqrt(2);
		

}
