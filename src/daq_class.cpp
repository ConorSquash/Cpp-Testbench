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

MatrixXd read_data_buffer(int num_of_channels_used, int samps_per_chan, float64 buff_data[5000]);


DAQ::DAQ(double Fs, double samples) 
{
	// Setup code
	cout << "DAQ sampling at " << Fs << " Hz" << endl;

	m_samples = samples;

	error = 0;
	taskHandle = 0;
	read = 0;
	//errBuff[2048] = { '\0' };

	m_num_of_channels_used = 1;
	m_samps_per_chan = samples;

	buffer_result.resize(samples, 1);


	// DAQmx analog voltage channel and timing parameters

	DAQmxErrChk(DAQmxCreateTask("", &taskHandle));

	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, "Dev1/ai1", "", DAQmx_Val_RSE, 0, 5, DAQmx_Val_Volts, NULL));

	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", Fs, DAQmx_Val_Rising, DAQmx_Val_FiniteSamps, samples));

	// DAQmx Start Code
	DAQmxErrChk(DAQmxStartTask(taskHandle));




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
	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, m_samples, 10.0, DAQmx_Val_GroupByChannel, buff_data, 1000, &read, NULL));

	// Extract buffer into Eigen Matrix
	my_result = read_data_buffer(1, m_samples, buff_data);

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






/*

VectorXcd demodulate(double numSamples) {

	// This function takes the sampled data and demodulates it



	// The Eigen library gives out if you use the matrix of floats for the line result = ..
	// but you must use a buffer of floats with DAQmxReadAnalogF64 --> cast to a double below


	buffer_result_d = buffer_result.cast<double>();

	result = buffer_result_d.col(0).transpose() * E;     // The first column is the sampled data of the first channel

	//cout << "\n\n\n Printing result " << endl << result << endl;

	magnitude = (2 * result.array().abs()) / numSamples;


	cout << "\n\n\n\n Magnitude of each frequency component : " << endl;
	cout << magnitude << endl;



	return magnitude;
}
*/

