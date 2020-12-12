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
int read_data_buffer();

double      min_voltage;
double      max_voltage;
int         sample_rate;
int		    n_samples;
int         samples_per_chan;
int         num_channels;
int         array_size;
string      channel;





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



	// User configurable parameters

	min_voltage = 0;    // Voltage swing should be between -10 V -> +10 V
	max_voltage = 5;

	sample_rate = 500;        // Set the sample rate of the DAQ for all channels in Hz
							  // Regardless if they are read or not this is how often the DAQ looks at the channel inputs.

	n_samples = 250;           // After this many samples are gathered the EveryNSamplesEvent will be called.
							   // This calls the function EveryNCallback which calls the READ function

	samples_per_chan = 100;    // Specify the number of samples read by each channel

	num_channels = 1;          // Setting the number of channels used to determine buffer size 

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
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, channel_char_arr, "", DAQmx_Val_RSE, min_voltage, max_voltage, DAQmx_Val_Volts, NULL));


	//DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", 100, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 10));
	DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", sample_rate, DAQmx_Val_Rising, DAQmx_Val_ContSamps, samples_per_chan));


	//DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, 10, 0, EveryNCallback, NULL));
	DAQmxErrChk(DAQmxRegisterEveryNSamplesEvent(taskHandle, DAQmx_Val_Acquired_Into_Buffer, n_samples, 0, EveryNCallback, NULL));


	DAQmxErrChk(DAQmxRegisterDoneEvent(taskHandle, 0, DoneCallback, NULL));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk(DAQmxStartTask(taskHandle));

	//printf("Acquiring samples continuously. Press Enter to interrupt\n");
	//getchar();

	printf("Acquiring samples continuously.\n");






	while (exit != "q")
	{
		exit = getchar();

		if (exit == "b")
			read_data_buffer();

	}

	//getchar();

	cout << buffer_result(5, 0) << endl;    // Print the fifth result
	cout << buffer_result.col(0).size() << endl;    // Print the size of the column result

	getchar();


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

	DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, samples_per_chan, 10.0, DAQmx_Val_GroupByScanNumber, buff_data, array_size, &read, NULL));


	if (read > 0) {
		//printf("Acquired %d samples. Total %d\r", (int)read, (int)(totalRead += read));
		//fflush(stdout);

		//for (int i = 0; i < read; i++)
			//printf("Data point %d has value %f\n", i, data[i]);

		//printf("Data point 5 has value %f\n", data[5]);

		//for (int i = 0; i < read; i++)
		//	buff_data[i] = data[i];


	}

	printf("\n To read buffer press b! \t To quit press q! \n");


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

int read_data_buffer() {

	//cout << "You asked to read the buffer!" << endl;

	//printf("Data point 5 has value %f\n", buff_data[5]);

	for (int i = 0; i < buffer_result.col(0).size(); i++)
	{
		buffer_result(i, 0) = buff_data[i];
	}

	return 0;
}