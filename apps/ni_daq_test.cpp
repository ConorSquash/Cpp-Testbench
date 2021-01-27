#include <iostream>

#include <stdio.h>
#include <NIDAQmx.h>


//#include <Eigen/Dense>
#include <Dense>



using Eigen::MatrixXd;
using Eigen::MatrixXf;
using Eigen::VectorXd;

using namespace std;

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

//float64     data[1000];
float64     buff_data[1000];
MatrixXf    buffer_result;

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void* callbackData);
int read_data_buffer(int num_of_channels_used, int samps_per_chan);
char* construct_channel_name(string channel_num);

double      min_voltage;
double      max_voltage;
int         sample_rate;
int		    n_samples;
int         samples_per_chan;
int         num_channels;
int         array_size;
string      channel;
char*		channel_as_char_arr;





int main() {

	int32       error = 0;
	TaskHandle  taskHandle = 0;
	char        errBuff[2048] = { '\0' };


	/*


	// Get the User configurable parameters

	cout << "Enter the number of channels you want to use : " << endl;
	cin >> num_channels;

	cout << "Enter the analog input channel used (1-15) : " << endl;
	cin >> channel;
	channel = "Dev1/ai" + channel;

	char* channel_char_arr;
	string str_obj(channel);
	channel_char_arr = &str_obj[0];
	//cout << channel_char_arr << endl;

	cout << "Enter a value for the minimum voltage (V) : " << endl;
	cin >> min_voltage;

	cout << "Enter a value for the maximum voltage (V) : " << endl;
	cin >> max_voltage;

	cout << "Enter a sample rate of the DAQ (Hz) : " << endl;
	cin >> sample_rate;
	//sample_rate = sample_rate * 1000;

	cout << "Enter the number of samples per channel : " << endl;
	cin >> samples_per_chan;
	//user_samp_per_chan = sample_rate;

	array_size = samples_per_chan * num_channels;

	cout << "Enter the number of samples : " << endl;
	cin >> n_samples;


	*/

	// Construct the string to specify the channel used and then write as char array

	string ai_channel = "1";
	channel = "Dev1/ai" + ai_channel;
	char* channel_char_arr;
	string str_obj(channel);
	channel_char_arr = &str_obj[0];



	//channel_as_char_arr = construct_channel_name("1");
	//cout << "Channel : " << channel_as_char_arr << endl;
	//getchar();

	// User configurable parameters

	min_voltage = 0;    // Voltage swing should be between -10 V -> +10 V
	max_voltage = 5;

	sample_rate = 5000;        // Set the sample rate of the DAQ for all channels in Hz
							  // Regardless if they are read or not this is how often the DAQ looks at the channel inputs.

	n_samples = 1000;           // After this many samples are gathered the EveryNSamplesEvent will be called.
							   // This calls the function EveryNCallback which calls the DAQmxReadAnalogF64 function

	samples_per_chan = 500;    // Specify the number of samples read by each channel

	num_channels = 2;          // Setting the number of channels used to determine buffer size 

	array_size = samples_per_chan * num_channels;   // The array to read samples into, organized according to fillMode.

	buffer_result.resize(samples_per_chan, num_channels);  // Creating an Eigen Matrix that reads and stores the data buffer
												             // Each column represents the samples for a particular channel

	int a = 1;
	string exit;

	/*********************************************/
		// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk(DAQmxCreateTask("", &taskHandle));

	//DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
	// CREATE FIRST CHANNEL
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, channel_char_arr, "", DAQmx_Val_RSE, min_voltage, max_voltage, DAQmx_Val_Volts, NULL));
	// CREATE SECOND CHANNEL
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, "Dev1/ai15", "", DAQmx_Val_RSE, min_voltage, max_voltage, DAQmx_Val_Volts, NULL));

	//DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", 100, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 10));
	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", sample_rate, DAQmx_Val_Rising, DAQmx_Val_ContSamps, samples_per_chan));


	//DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, 10, 0, EveryNCallback, NULL));
	DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, n_samples, 0, EveryNCallback, NULL));


	DAQmxErrChk(DAQmxRegisterDoneEvent(taskHandle, 0, DoneCallback, NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/

	DAQmxErrChk(DAQmxStartTask(taskHandle));

	printf("Acquiring samples continuously. Press Enter to interrupt\n");
	getchar();

	




	/*
	
	while (exit != "q")
	{
		exit = getchar();

		if (exit == "b")
			read_data_buffer();

	}
	
	*/



	


Error:
	if (DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
	if (taskHandle != 0) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if (DAQmxFailed(error))
		printf("DAQmx Error: %s\n", errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
}

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData)
{
	int32       error = 0;
	char        errBuff[2048] = { '\0' };
	static int  totalRead = 0;
	int32       read = 0;
	float64     data[1000];

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/

	//DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, samples_per_chan, 10.0, DAQmx_Val_GroupByScanNumber, data, array_size, &read, NULL));

	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, samples_per_chan, 10.0, DAQmx_Val_GroupByChannel, buff_data, array_size, &read, NULL));


	//if (read > 0) {   }

	
	// Call the function that transfers the buffer data into an eigen matrix
	// with each column representing a channel
	read_data_buffer(num_channels, samples_per_chan);


Error:
	if (DAQmxFailed(error)) {
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
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

int read_data_buffer(int num_of_channels_used, int samps_per_chan) {

	//cout << "You asked to read the buffer!" << endl;

	//printf("Data point 5 has value %f\n", buff_data[5]);

	for (int j = 0; j < num_of_channels_used; j++)	
		for (int i = 0; i < samps_per_chan; i++)
			buffer_result(i , j) = buff_data[i + (j * samps_per_chan)];
		

	cout << "First result in column 1 = " << buffer_result(1, 0) << "\t" <<    // Print the fifth result
	  "First result in column 2 = " << buffer_result(1, 1) << endl;    // Print the fifth result

	

	return 0;
}

char* construct_channel_name(string channel_num)
{
	// This function constructs the char array required to specify the channel used
	// from a string. (Provided the name hasnt been altered in NI MAX)


	string channel = "Dev1/ai" + channel_num;

	cout << channel << endl;

	string str_obj(channel);

	char* my_channel = &str_obj[0];

	cout << my_channel << endl;

	return my_channel;
}


