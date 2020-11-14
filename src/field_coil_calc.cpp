#include "field_coil_calc.h"
#include <iostream>
#include <array>
#include <string>
#include <vector>

#define _USE_MATH_DEFINES
#include "math.h"

#include <Dense>
#include <Eigen>

using Eigen::MatrixXd;
using namespace std;
using namespace Eigen;

void field_coil_calc(double I, MatrixXd x_points, MatrixXd y_points, MatrixXd z_points, double Px, double Py, double Pz, VectorXd & Hx, VectorXd & Hy, VectorXd & Hz)
{


	// Calculates the magnetic field due a straight current 
	// filament at an arbitary orientation relative to an origin
	// Based on "Accurate magnetic field intensity calculations for 
	// contactless energy transfer coils" - Sonntag, Spree, et al, 2007 (doi:)



		 // ** NOTE ON USING EIGEN **

	 // When adding/subtracting/multiplying.. matrices you can use the 'Matrix' class (as done before)
	 // When performing element wise operations ie. square all values in Matrix you must use the Array class
	 // Can convert easily from on to the other and back. eg myexamplematrix.array()
	 // Just need to break functions up into more steps

	 // See https://eigen.tuxfamily.org/dox/group__TutorialArrayClass.html


	 int c_size = x_points.rows();
	 int r_size = x_points.cols() - 1;


	// See Powerpoint VivaPres_Final slide 21
	// Need to calculate a,b,c vectors for x,y,z ie ax,ay,az,bx,by....
	
	MatrixXd ax(c_size, r_size), ay(c_size, r_size), az(c_size, r_size);    // Instantiate them with the same number
	MatrixXd bx(c_size, r_size), by(c_size, r_size), bz(c_size, r_size);    // of rows as matrix passed into function
	MatrixXd cx(c_size, r_size), cy(c_size, r_size), cz(c_size, r_size);

	//ax, ay, az

	ax = x_points.block(0, 1, x_points.rows(), x_points.cols() - 1) - x_points.block(0, 0, x_points.rows(), x_points.cols() - 1);
	ay = y_points.block(0, 1, x_points.rows(), x_points.cols() - 1) - y_points.block(0, 0, x_points.rows(), x_points.cols() - 1);
	az = z_points.block(0, 1, x_points.rows(), x_points.cols() - 1) - z_points.block(0, 0, x_points.rows(), x_points.cols() - 1);


	// bx, by, bz

	bx = (x_points.block(0, 1, x_points.rows(), x_points.cols() - 1)).array() - Px;
	by = (y_points.block(0, 1, x_points.rows(), x_points.cols() - 1)).array() - Py;
	bz = (z_points.block(0, 1, x_points.rows(), x_points.cols() - 1)).array() - Pz;


	// cx, cy, cz

	cx = (x_points.block(0, 0, x_points.rows(), x_points.cols() - 1)).array() - Px;
	cy = (y_points.block(0, 0, x_points.rows(), x_points.cols() - 1)).array() - Py;
	cz = (z_points.block(0, 0, x_points.rows(), x_points.cols() - 1)).array() - Pz;



	 // Calculate the magnitudes of the b and c vectors



	   // Getting magnitude of the c vector
	   // Need to square matrices individually, then add, then get square root

	   MatrixXd cx_squared(c_size, r_size), cy_squared(c_size, r_size), cz_squared(c_size, r_size);
	   MatrixXd mag_c_squared(c_size, r_size);
	   MatrixXd mag_c(c_size, r_size);

	   cx_squared = cx.array().square();
	   cy_squared = cy.array().square();
	   cz_squared = cz.array().square();

	   mag_c_squared = cx_squared + cy_squared + cz_squared;

	   mag_c = mag_c_squared.array().sqrt();


	   // Getting magnitude of the b vector
	   // Need to square matrices individually, then add, then get square root

	   MatrixXd bx_squared(c_size, r_size), by_squared(c_size, r_size), bz_squared(c_size, r_size);
	   MatrixXd mag_b_squared(c_size, r_size);
	   MatrixXd mag_b(c_size, r_size);

	   bx_squared = bx.array().square();
	   by_squared = by.array().square();
	   bz_squared = bz.array().square();

	   mag_b_squared = bx_squared + by_squared + bz_squared;

	   mag_b = mag_b_squared.array().sqrt();


	   // Calculating a . b
	   // Calculate individually then add
	   MatrixXd ax_dot_bx(c_size, r_size), ay_dot_by(c_size, r_size), az_dot_bz(c_size, r_size);
	   MatrixXd a_dot_b(c_size, r_size);

	   // Need to use the array class as it is coefficient wise 
	   // multiplication vs matrix multiplication
	   ax_dot_bx = ax.array() * bx.array();   
	   ay_dot_by = ay.array() * by.array();
	   az_dot_bz = az.array() * bz.array();

	   a_dot_b = ax_dot_bx + ay_dot_by + az_dot_bz;


	   // Calculating a . c
	   // Calculate individually then add
	   MatrixXd ax_dot_cx(c_size, r_size), ay_dot_cy(c_size, r_size), az_dot_cz(c_size, r_size);
	   MatrixXd a_dot_c(c_size, r_size);

	   // Need to use the array class as it is coefficient wise 
	   // multiplication vs matrix multiplication
	   ax_dot_cx = ax.array() * cx.array();
	   ay_dot_cy = ay.array() * cy.array();
	   az_dot_cz = az.array() * cz.array();

	   a_dot_c = ax_dot_cx + ay_dot_cy + az_dot_cz;



	   // Calculating c x a (Has 3 parts. ie produces a vector)
		
		 // Calculate resulting x value 
		 MatrixXd c_cross_a_x(c_size, r_size);
		 MatrixXd az_cy(c_size, r_size), ay_cz(c_size, r_size);

		 az_cy = az.array() * cy.array();
		 ay_cz = ay.array() * cz.array();

		 c_cross_a_x = az_cy - ay_cz;

		 // Calculate resulting y value 
		 MatrixXd c_cross_a_y(c_size, r_size); 
		 MatrixXd ax_cz(c_size, r_size), az_cx(c_size, r_size);

		 ax_cz = ax.array() * cz.array();
		 az_cx = az.array() * cx.array();

		 c_cross_a_y = ax_cz - az_cx;

		 // Calculate resulting z value 
		 MatrixXd c_cross_a_z(c_size, r_size);
		 MatrixXd cx_ay(c_size, r_size), cy_ax(c_size, r_size);

		 cx_ay = cx.array() * ay.array();
		 cy_ax = cy.array() * ax.array();

		 c_cross_a_z = cx_ay - cy_ax;



	  // Calculating the magnitude squared of c x a
		 MatrixXd c_cross_a_x_squared(c_size, r_size);
		 MatrixXd c_cross_a_y_squared(c_size, r_size);
		 MatrixXd c_cross_a_z_squared(c_size, r_size);

		 c_cross_a_x_squared = c_cross_a_x.array().square();
		 c_cross_a_y_squared = c_cross_a_y.array().square();
		 c_cross_a_z_squared = c_cross_a_z.array().square();

		 MatrixXd mag_squared_c_cross_a(c_size, r_size);
		 mag_squared_c_cross_a = c_cross_a_x_squared + c_cross_a_y_squared + c_cross_a_z_squared;



		 // Calculating the scalar second part of the equation
		 //   (     1     )( a . c      a . b )
		 //   ( --------- )( -----  -   ----- )
		 //   ( |c x a|^2 )(  |c|        |b|  )


		 MatrixXd scalar_2nd_bit(c_size, r_size);
		 MatrixXd adotc_over_mag_c(c_size, r_size);
		 MatrixXd adotb_over_mag_b(c_size, r_size);
		 MatrixXd equation_RHS(c_size, r_size);

		 adotc_over_mag_c = a_dot_c.array() / mag_c.array();
		 adotb_over_mag_b = a_dot_b.array() / mag_b.array();
		 equation_RHS = adotc_over_mag_c - adotb_over_mag_b;
		 scalar_2nd_bit = equation_RHS.array() / mag_squared_c_cross_a.array();

		 MatrixXd Hx_dum(c_size, r_size), Hy_dum(c_size, r_size), Hz_dum(c_size, r_size);

		 Hx_dum = (I / (4 * M_PI)) * c_cross_a_x.array() * scalar_2nd_bit.array();
		 Hy_dum = (I / (4 * M_PI)) * c_cross_a_y.array() * scalar_2nd_bit.array();
		 Hz_dum = (I / (4 * M_PI)) * c_cross_a_z.array() * scalar_2nd_bit.array();


		 // Now calculate the sum of the rows of Hx/Hy/Hz_dum and return as a column vector
		 Hx = Hx_dum.rowwise().sum();
		 Hy = Hy_dum.rowwise().sum();
		 Hz = Hz_dum.rowwise().sum();
}
