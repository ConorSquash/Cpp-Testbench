
#include <iostream>
#define _USE_MATH_DEFINES
#include "math.h"

#include "OpenIGTLink_class.h"

using namespace std;




int main() {

	cout << "hello!" << endl;

	int port = 18944;
	float fps = 1;

	OpenIGTLink my_server(port, fps);

	vector <double> PandO = { 20, 45, 20, M_PI_2, M_PI_2 };

	my_server.SendMessage(PandO);

	//my_server.CloseSocket();

	cout << "test" << endl;

	getchar();


	return 0;
}