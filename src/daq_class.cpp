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

using Eigen::MatrixXcd;
using Eigen::VectorXcd;

using Eigen::MatrixXcf;
using Eigen::VectorXcf;

using Eigen::VectorXd;

using namespace std;



// DAQ Functions and variables
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

//float64     data[1000];
float64     buff_data[5000];
MatrixXf    buffer_result;
MatrixXd   buffer_result_d;

MatrixXd  my_result;

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void* callbackData);

MatrixXd read_data_buffer(int num_of_channels_used, int samps_per_chan);
VectorXcd demodulate(double samples);


// Demodulation Variables

VectorXcd result(8), magnitude(8);
VectorXcd my_peaks(8);
MatrixXcd E;





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
	my_result = read_data_buffer(1,samples);

	my_peaks = demodulate(samples);

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

int DAQ::DemodSetup(double frequency, double samples)
{

	cout << "\n -> SETTING DEMOD PARMS " << endl;

	// Specify the sampling frequency per sensor channel
	double Fs = frequency;
	double Ts = 1 / Fs;
	int numSamples = samples;


	// Specify the number of time samples, must be the same as the length of X

	VectorXd t(numSamples);

	for (int i = 0; i < numSamples; i++)
		t(i) = (i * Ts);

	// Define the transmission frequencies of the emitter coil
	// These will be used for demodulation

	VectorXd F(8);

	F << 2500, 4860, 8500, 11020, 12500, 17888, 20000, 30000;

	E.resize(8, numSamples);   // Matrix of complex doubles

	// Define the demodulation matrix for the asynchronous demodulation scheme

	for (int j = 0; j < 8; j++)
		E.row(j) = exp(2 * M_PI * F(j) * 1i * t.array());

	E.transposeInPlace();		// Must transpose in place when replacing with a transpose of itself!!! 
								// Same as E = E.transpose().eval();
	return 0;
}





MatrixXd read_data_buffer(int num_of_channels_used, int samps_per_chan) {

	// Sorts the buffer into columns in an eigen matrix

	for (int j = 0; j < num_of_channels_used; j++)
		for (int i = 0; i < samps_per_chan; i++)
			buffer_result(i, j) = buff_data[i + (j * samps_per_chan)];

	buffer_result_d = buffer_result.cast<double>();      // Need to cast to double when demodulating later

	cout << "\tFirst result in column 1 = " << buffer_result_d(0, 0) << endl;
	//"\t" <<    // Print the fifth result
	//"First result in column 2 = " << buffer_result(0, 1) << endl;    // Print the fifth result



	return buffer_result_d;
}





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

