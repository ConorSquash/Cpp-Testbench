
#include <iostream>

#include "OpenIGTLink_class.h"

using namespace std;




int main() {

	cout << "hello!" << endl;

	int port = 18944;
	float fps = 1;

	OpenIGTLink my_server(port, fps);

	cout << "test!" << endl;


	return 0;
}