#ifndef DAQ_CLASS_H
#define DAQ_CLASS_H


#include <NIDAQmx.h>
#include <Dense>

using namespace std;
using Eigen::MatrixXd;
using Eigen::MatrixXf;


//PUT CLASS DECLARATION HERE
class DAQ
{
private:

	int32       error;
	TaskHandle  taskHandle;
	int32       read;
	//float64     data[1000];
	char        errBuff[2048] = { '\0' };
	float64     buff_data[5000];

	double m_samples;

	//int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData);
	//int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void* callbackData);

	double m_samps_per_chan;
	double m_num_of_channels_used;



public:

	MatrixXd  my_result;

	DAQ(double Fs, double samples, bool is_finite);

	int ReadSamples();

	int ContinuousSamples();

};

#endif