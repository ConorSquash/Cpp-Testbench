#ifndef DAQ_CLASS_H
#define DAQ_CLASS_H


using namespace std;


//PUT CLASS DECLARATION HERE
class DAQ
{
private:

public:
	

	int AcquireSamples(double Fs, double samples);
	int DemodSetup(double frequency, double samples);


};

#endif