#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <chrono>
#include <stdio.h>
#include <NIDAQmx.h>
#include <complex>

#define _USE_MATH_DEFINES
#include "math.h"

#include "coil_spec.h"
#include "field_coil_calc.h"
#include "sensor_objective_function.h"

#include "lsqcpp.h"
#include "solver_class.h"



#include <Dense>

using Eigen::MatrixXd;
using Eigen::MatrixXf;
using Eigen::VectorXd;
using Eigen::VectorXf;

using Eigen::MatrixXcd;
using Eigen::VectorXcd;

using Eigen::MatrixXcf;
using Eigen::VectorXcf;

using namespace std;
using namespace literals;

using namespace std;
using namespace std::chrono;

VectorXd sensor_flux(8);
vector<vector<double>> x_matrix(8);
vector<vector<double>> y_matrix(8);
vector<vector<double>> z_matrix(8);

MatrixXd X_Matrix(8, 102);
MatrixXd Y_Matrix(8, 102);
MatrixXd Z_Matrix(8, 102);

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

//float64     data[1000];
float64     buff_data[5000];
MatrixXf    buffer_result;
MatrixXd   buffer_result_d;

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void* callbackData);

int read_data_buffer(int num_of_channels_used, int samps_per_chan);
char* construct_channel_name(string channel_num);
int demodulate();
int write_txt();


double      min_voltage;
double      max_voltage;
int         sample_rate;
int		    n_samples;
int         samples_per_chan;
int         num_channels;
int         array_size;
string      channel;
char* channel_as_char_arr;


// Demodulation Variables

VectorXcd result(8), magnitude(8);

// Specify the sampling frequency per sensor channel
//double Fs = 100e3;
//double Ts = 1 / Fs;
//int numSamples = 5000;

//MatrixXcd E(8, numSamples);   // Matrix of complex doubles




void Solver::printmessage() { cout << "hello!" << endl; };

int Solver::ConfigureDAQ(double DAQfrequency, int no_of_samples, int no_of_chans ) {

	cout << "Setting up DAQ : " << endl;
	cout << "\nSampling Frequency = " << DAQfrequency << " Hz" << endl;
	cout << no_of_samples << " samples" << endl;
	cout << no_of_chans << " Channel(s)" << endl;

	int32       error = 0;
	TaskHandle  taskHandle = 0;
	char        errBuff[2048] = { '\0' };


	// Construct the string to specify the channel used and then write as char array (not really required)

	string ai_channel = "1";
	channel = "Dev1/ai" + ai_channel;
	char* channel_char_arr;
	string str_obj(channel);
	channel_char_arr = &str_obj[0];

	//******** USER DAQ SETUP ********\\

	min_voltage = 0;           // Voltage swing should be between -10 V -> +10 V
	max_voltage = 5;

	sample_rate = DAQfrequency;          // Set the sample rate of the DAQ for all channels in Hz
							   // Regardless if they are read or not this is how often the DAQ looks at the channel inputs.

	n_samples = no_of_samples;     // After this many samples are gathered PER CHANNEL the EveryNSamplesEvent will be called.
								// This event calls the function EveryNCallback (which calls the DAQmxReadAnalogF64 function)

	samples_per_chan = no_of_samples;    // Specify the number of samples read by each channel (usually ignored)
									  // This is used to calculate the buffer size if the default isnt appropriate

	num_channels = no_of_chans;          // Set the number of channels used to determine buffer size. 

	array_size = n_samples * num_channels;   // The array to read samples into, organized according to fillMode.

	buffer_result.resize(samples_per_chan, num_channels);    // Creating an Eigen Matrix that reads and stores the data buffer
															 // Each column represents the samples for a particular channel

	//******** DAQmx Configure Code ********\\

	DAQmxErrChk(DAQmxCreateTask("", &taskHandle));

	//DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
	// CREATE FIRST CHANNEL
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, channel_char_arr, "", DAQmx_Val_RSE, min_voltage, max_voltage, DAQmx_Val_Volts, NULL));
	// CREATE SECOND CHANNEL
	//DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, "Dev1/ai15", "", DAQmx_Val_RSE, min_voltage, max_voltage, DAQmx_Val_Volts, NULL));

	//DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", 100, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 10));
	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", sample_rate, DAQmx_Val_Rising, DAQmx_Val_ContSamps, samples_per_chan));

	//DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, 10, 0, EveryNCallback, NULL));
	DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, n_samples, 0, EveryNCallback, NULL));

	DAQmxErrChk(DAQmxRegisterDoneEvent(taskHandle, 0, DoneCallback, NULL));


	//******** DAQmx Start Code ********\\
	
	DAQmxErrChk(DAQmxStartTask(taskHandle));


	cout << "DAQ is configured and continuosly sampling!" << endl;


Error:
	if (DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
	if (taskHandle != 0) {

		//******** DAQmx Stop Code ********\\
		
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if (DAQmxFailed(error))
		printf("DAQmx Error: %s\n", errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
};

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData)
{
	int32       error = 0;
	char        errBuff[2048] = { '\0' };
	static int  totalRead = 0;
	int32       read = 0;
	float64     data[1000];


	//******** DAQmx Read Code ********\\
	

	//DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, samples_per_chan, 10.0, DAQmx_Val_GroupByScanNumber, data, array_size, &read, NULL));

	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, samples_per_chan, 10.0, DAQmx_Val_GroupByChannel, buff_data, array_size, &read, NULL));


	// Call the function that transfers the buffer data into an eigen matrix with each column representing a channel
	//read_data_buffer(num_channels, samples_per_chan);

	cout << "Entered EveryNCallback" << endl;

	// Call the demodulation function here
	//demodulate();


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

	cout << "Entered DoneCallback" << endl;

Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
		DAQmxClearTask(taskHandle);
		printf("DAQmx Error: %s\n", errBuff);
	}
	return 0;
}

VectorXcd Solver::AcquirePeaks(int numSamples, double Ts, int no_of_chans ) {

	// A function that when called returns a vector of demodulated data 
	cout << "\n -> ACQUIRING BUFFER AND DEMODULATING" << endl;


	// Sorts the buffer into columns in an eigen matrix

	for (int j = 0; j < no_of_chans; j++)
		for (int i = 0; i < numSamples; i++)
			buffer_result(i, j) = buff_data[i + (j * numSamples)];

	cout << "First result in column 1 = " << buffer_result(0,0) << endl;

	//******** DEMODULATION SETUP ********\\

	MatrixXcd E(8, numSamples);   // Matrix of complex doubles


	// Specify the number of time samples, must be the same as the length of X

	VectorXd t(numSamples);

	for (int i = 0; i < numSamples; i++)
		t(i) = (i * Ts);

	// Define the transmission frequencies of the emitter coil
	// These will be used for demodulation

	VectorXd F(8);

	F << 2500, 4860, 8500, 11020, 12500, 17888, 20000, 30000;

	// Define the demodulation matrix for the asynchronous demodulation scheme

	for (int j = 0; j < 8; j++)
		E.row(j) = exp(2 * M_PI * F(j) * 1i * t.array());

	E.transposeInPlace();		// Must transpose in place when replacing with a transpose of itself!!! 
								// Same as E = E.transpose().eval();




	buffer_result_d = buffer_result.cast<double>();

	result = buffer_result_d.col(0).transpose() * E;     // The first column is the sampled data of the first channel

	//cout << "\n\n\n Printing result " << endl << result << endl;

	magnitude = (2 * result.array().abs()) / numSamples;


	return magnitude;


};


