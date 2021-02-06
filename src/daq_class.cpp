#include <iostream>
#include <string>
#include <vector>
#include <NIDAQmx.h>
#include "daq_class.h"
#include <Dense>

using Eigen::MatrixXd;
using Eigen::MatrixXf;



using namespace std;



// DAQ Functions and variables
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

//float64     data[1000];
float64     buff_data[5000];
MatrixXf    buffer_result;
MatrixXd   buffer_result_d;

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void* callbackData);

int read_data_buffer(int num_of_channels_used, int samps_per_chan);







int DAQ::AcquireSamples(double Fs, double samples) 
{

	// This function acquires a finite number of samples
	// and sorts them into an eigen matrix
	cout << "\n -> ACQUIRING " <<  samples << " SAMPLES" << endl;

	buffer_result.resize(samples, 1);

	int32       error = 0;
	TaskHandle  taskHandle = 0;
	int32       read;
	float64     data[1000];
	char        errBuff[2048] = { '\0' };

	// DAQmx analog voltage channel and timing parameters

	DAQmxErrChk(DAQmxCreateTask("", &taskHandle));

	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, "Dev1/ai1", "", DAQmx_Val_RSE, 0, 5, DAQmx_Val_Volts, NULL));

	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", Fs, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, samples));

	// DAQmx Start Code
	DAQmxErrChk(DAQmxStartTask(taskHandle));

	// DAQmx Read Code
	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, samples, 10.0, DAQmx_Val_GroupByChannel, buff_data, 1000, &read, NULL));

	// Extract buffer into Eigen Matrix
	read_data_buffer(1,samples);



	// Stop and clear task



Error:

	if (DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);

	if (taskHandle != 0) {

		DAQmxStopTask(taskHandle);

		DAQmxClearTask(taskHandle);

	}

	if (DAQmxFailed(error))
		printf("DAQmx Error: %s\n", errBuff);

	return 0;

}





int read_data_buffer(int num_of_channels_used, int samps_per_chan) {

	// Sorts the buffer into columns in an eigen matrix

	for (int j = 0; j < num_of_channels_used; j++)
		for (int i = 0; i < samps_per_chan; i++)
			buffer_result(i, j) = buff_data[i + (j * samps_per_chan)];

	buffer_result_d = buffer_result.cast<double>();      // Need to cast to double when demodulating later

	cout << "\tFirst result in column 1 = " << buffer_result_d(0, 0) << endl;
	//"\t" <<    // Print the fifth result
	//"First result in column 2 = " << buffer_result(0, 1) << endl;    // Print the fifth result



	return 0;
}

