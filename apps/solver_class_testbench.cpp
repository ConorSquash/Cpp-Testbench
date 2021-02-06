#include <iostream>
#include <string>
#include "solver_class.h"

using namespace std;


int main() {

	
	// SOLVE
	Solver my_sensor;
	
	vector <double> measurement = {5,6,7,8,9,10,11,12};        // data extracted from file 
	
	vector <double> initial_condition = { 0,0,0.15,0,0 };      // Provide an initial guess for x,y,z,theta,phi

	vector <double> PandO; 	                                   // Create a vector to store position and orientation info

	PandO = my_sensor.Solve(measurement, initial_condition);   // Pass initial guess and sensor flux and then solve



	cout << "\n -> SOLVED P&O : " << endl;
	cout << " x : " << PandO[0] << endl;
	cout << " y : " << PandO[1] << endl;
	cout << " x : " << PandO[2] << endl;
	cout << " Pitch : " << PandO[3] << endl;
	cout << " Yaw : " << PandO[4] << endl;

	getchar();


}