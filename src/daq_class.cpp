#include <iostream>
#include <string>
#include <vector>
#include <NIDAQmx.h>
#include "daq_class.h"
#include <Dense>

#define _USE_MATH_DEFINES
#include "math.h"

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

MatrixXd read_data_buffer(int num_of_channels_used, int samps_per_chan, float64 buff_data[5000]);

MatrixXd continuous_result;


DAQ::DAQ(double Fs, double samples, bool is_finite, string channel1, string channel2)
{

	channel1 = "Dev1/ai" + channel1;
	string str_obj1(channel1);
	channel_char_arr1 = &str_obj1[0];

	channel2 = "Dev1/ai" + channel2;
	string str_obj2(channel2);
	channel_char_arr2 = &str_obj2[0];

	// Setup code
	cout << "DAQ sampling at " << Fs << " Hz" << endl;

	m_samples = samples;

	error = 0;
	taskHandle = 0;
	read = 0;
	//errBuff[2048] = { '\0' };

	m_num_of_channels_used = 2;
	m_samps_per_chan = samples;

	buffer_result.resize(samples, 2);


	// DAQmx analog voltage channel and timing parameters

	DAQmxErrChk(DAQmxCreateTask("", &taskHandle));

	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, channel_char_arr1, "", DAQmx_Val_RSE, 0, 5, DAQmx_Val_Volts, NULL));
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, channel_char_arr2, "", DAQmx_Val_RSE, 0, 5, DAQmx_Val_Volts, NULL));

	if (is_finite) 
		DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", Fs, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, samples));
	else
		DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", Fs, DAQmx_Val_Rising, DAQmx_Val_ContSamps, samples));



Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);

		// DAQmx Stop Code

		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n", errBuff);
	}
}


int DAQ::ReadSamples()
{
	DAQmxErrChk(DAQmxStartTask(taskHandle));

	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, m_samples, 10.0, DAQmx_Val_GroupByChannel, buff_data, 2000, &read, NULL));

	// Extract buffer into Eigen Matrix
	my_result = read_data_buffer(2, m_samples, buff_data);

	DAQmxErrChk(DAQmxStopTask(taskHandle));

	return 0;

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);

		// DAQmx Stop Code

		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n", errBuff);
	}

}

int DAQ::ContinuousSamples()
{

	DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, 2*m_samples, 0, EveryNCallback, NULL));
	
	DAQmxErrChk(DAQmxStartTask(taskHandle));


	return 0;

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);

		// DAQmx Stop Code

		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n", errBuff);
	}

}


MatrixXd read_data_buffer(int num_of_channels_used, int samps_per_chan, float64 buff_data[5000]) 
{

	// Sorts the buffer into columns in an eigen matrix

	for (int j = 0; j < num_of_channels_used; j++)
		for (int i = 0; i < samps_per_chan; i++)
			buffer_result(i, j) = buff_data[i + (j * samps_per_chan)];


	// Need to cast to double when demodulating later
	buffer_result_d = buffer_result.cast<double>();      

	return buffer_result_d;
}



int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData)
{
	int32       error = 0;
	char        errBuff[2048] = { '\0' };
	static int  totalRead = 0;
	int32       read = 0;
	//float64     data[1000];

	// DAQmx Read Code

	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, nSamples, 10.0, DAQmx_Val_GroupByChannel, buff_data, 2*nSamples, &read, NULL));

	//continuous_result = read_data_buffer(1, nSamples, buff_data);

	cout << buff_data[0] << endl;
	//cout << buff_data[1001] << endl << endl;;

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);

		// DAQmx Stop Code
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n", errBuff);
	}
	return 0;
}



int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void* callbackData)
{
	int32   error = 0;
	char    errBuff[2048] = { '\0' };

	// Check to see if an error stopped the task.
	DAQmxErrChk(status);

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n", errBuff);
	}
	return 0;
}

