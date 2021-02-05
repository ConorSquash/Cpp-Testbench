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



// DAQ Functions and variables
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

//float64     data[1000];
float64     buff_data[5000];
MatrixXf    buffer_result;
MatrixXd   buffer_result_d;

int32 CVICALLBACK EveryNCallback(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData);
int32 CVICALLBACK DoneCallback(TaskHandle taskHandle, int32 status, void* callbackData);

int read_data_buffer(int num_of_channels_used, int samps_per_chan);


// Demodulation Variables
VectorXcd result(8), magnitude_cmplx(8);
VectorXd magnitude(8);


// Solver variables
VectorXd sensor_flux(8);
vector<vector<double>> x_matrix(8);
vector<vector<double>> y_matrix(8);
vector<vector<double>> z_matrix(8);

MatrixXd X_Matrix(8, 102);
MatrixXd Y_Matrix(8, 102);
MatrixXd Z_Matrix(8, 102);


void Solver::printmessage() { cout << "hello!" << endl; };

/*


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


	cout << "DAQ is configured and continuously sampling!" << endl;


Error:
	if (DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
	if (taskHandle != 0) {

		//******** DAQmx Stop Code ********\\

		cout << "DAQ error" << endl;
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
	read_data_buffer(num_channels, samples_per_chan);

	//cout << "Entered EveryNCallback" << endl;

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

*/

struct MySolver
{


	void operator()(const VectorXd& xval, VectorXd& fval, MatrixXd& no_name) const
	{

		VectorXd currentPandO(5);

		currentPandO << xval(0), xval(1), xval(2), xval(3), xval(4);

		VectorXd out_vector(8);        // A vector to get the result of FluxModel - FluxReal

		vector<vector<double>> calibration = { {1},{1},{1},{1},{1},{1},{1},{1} };    // Create vector of calibration values (column)

		sensor_objective_function(currentPandO, sensor_flux, X_Matrix, Y_Matrix, Z_Matrix, calibration, out_vector);

		fval = out_vector;


	}
};

int Solver::AcquireSamples(double Fs, double samples) 
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


VectorXd Solver::AcquirePeaks(int numSamples, double Ts) {

	// A function that when called returns a vector of demodulated data 
	cout << "\n -> DEMODULATING AND EXTRACTING PEAKS" << endl;



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

	magnitude_cmplx = (2 * result.array().abs()) / numSamples;

	magnitude = magnitude_cmplx.real();


	return magnitude;


};


int read_data_buffer(int num_of_channels_used, int samps_per_chan) {

	// Sorts the buffer into columns in an eigen matrix

	for (int j = 0; j < num_of_channels_used; j++)
		for (int i = 0; i < samps_per_chan; i++)
			buffer_result(i, j) = buff_data[i + (j * samps_per_chan)];


	cout << "\tFirst result in column 1 = " << buffer_result(0, 0) << endl;
	//"\t" <<    // Print the fifth result
	//"First result in column 2 = " << buffer_result(0, 1) << endl;    // Print the fifth result



	return 0;
}


VectorXd Solver::Solve(VectorXd amplitudes, VectorXd initialGuess)
{



	//====================================================================


		// Define side length of outer square
		// Define width of each track
		// Define spacing between tracks
		// Define thickness of the emitter coil PCB
		// Define total number of turns per coil

	double l = 70e-03;
	double w = 0.5e-03;
	double s = 0.25e-03;
	double thickness = 1.6e-03;
	int Nturns = 25;


	// Generate coil filaments

	string ModelType = "EXACT";
	//string ModelType = "FAST";

	vector<double> x_points_angled, y_points_angled, z_points_angled;     // Make two vectors (for x,y,z) to store coordinates for the 
	vector<double> x_points_vert, y_points_vert, z_points_vert;           // "vertical" and "angled" coils

	coil_spec(l, w, s, thickness, M_PI_4, Nturns, ModelType, x_points_angled, y_points_angled, z_points_angled);
	coil_spec(l, w, s, thickness, M_PI_2, Nturns, ModelType, x_points_vert, y_points_vert, z_points_vert);


	// Generate each coil x and y centre point in mm
	array<double, 8> x_centre_points = { -93.543, 0, 93.543, -68.55, 68.55, -93.543, 0, 93.543 };
	array<double, 8> y_centre_points = { 93.543, 68.55, 93.543, 0, 0, -93.543, -68.55, -93.543 };

	for (int i = 0; i < x_centre_points.size(); i++)  // Convert to metres from mm
	{
		x_centre_points[i] *= 1e-03;
		y_centre_points[i] *= 1e-03;

	}

	// Add the centre position offsets to each coil

	vector<double> x_points1, x_points2, x_points3, x_points4, x_points5, x_points6, x_points7, x_points8;
	vector<double> y_points1, y_points2, y_points3, y_points4, y_points5, y_points6, y_points7, y_points8;
	vector<double> z_points1, z_points2, z_points3, z_points4, z_points5, z_points6, z_points7, z_points8;


	for (int i = 0; i < x_points_angled.size(); i++)
	{
		// Add x offset
		x_points1.push_back(x_points_vert[i] + x_centre_points[0]);
		x_points2.push_back(x_points_angled[i] + x_centre_points[1]);
		x_points3.push_back(x_points_vert[i] + x_centre_points[2]);
		x_points4.push_back(x_points_angled[i] + x_centre_points[3]);
		x_points5.push_back(x_points_angled[i] + x_centre_points[4]);
		x_points6.push_back(x_points_vert[i] + x_centre_points[5]);
		x_points7.push_back(x_points_angled[i] + x_centre_points[6]);
		x_points8.push_back(x_points_vert[i] + x_centre_points[7]);

		// Add y offset
		y_points1.push_back(y_points_vert[i] + y_centre_points[0]);
		y_points2.push_back(y_points_angled[i] + y_centre_points[1]);
		y_points3.push_back(y_points_vert[i] + y_centre_points[2]);
		y_points4.push_back(y_points_angled[i] + y_centre_points[3]);
		y_points5.push_back(y_points_angled[i] + y_centre_points[4]);
		y_points6.push_back(y_points_vert[i] + y_centre_points[5]);
		y_points7.push_back(y_points_angled[i] + y_centre_points[6]);
		y_points8.push_back(y_points_vert[i] + y_centre_points[7]);

		// No z offset
		z_points1.push_back(z_points_vert[i]);
		z_points2.push_back(z_points_angled[i]);
		z_points3.push_back(z_points_vert[i]);
		z_points4.push_back(z_points_angled[i]);
		z_points5.push_back(z_points_angled[i]);
		z_points6.push_back(z_points_vert[i]);
		z_points7.push_back(z_points_angled[i]);
		z_points8.push_back(z_points_vert[i]);

	}

	// Now bundle each into a matrix with coil vertices organised into columns


	x_matrix = { {x_points1},
				  {x_points2},
				  {x_points3},
				  {x_points4},
				  {x_points5},
				  {x_points6},
				  {x_points7},
				  {x_points8} };

	y_matrix = { {y_points1},
				  {y_points2},
				  {y_points3},
				  {y_points4},
				  {y_points5},
				  {y_points6},
				  {y_points7},
				  {y_points8} };

	z_matrix = { {z_points1},
				  {z_points2},
				  {z_points3},
				  {z_points4},
				  {z_points5},
				  {z_points6},
				  {z_points7},
				  {z_points8} };


	// Convert to Eigen Matrices

	 //Setting size of X_Matrix and others above..could resize here

	for (int i = 0; i < x_matrix.size(); i++)
		for (int j = 0; j < x_matrix[0].size(); j++)
		{
			X_Matrix(i, j) = x_matrix[i][j];
			Y_Matrix(i, j) = y_matrix[i][j];
			Z_Matrix(i, j) = z_matrix[i][j];
		}

	cout << "\n -> COILS MODELLED" << endl;

	//====================================================================

	lsq::LevenbergMarquardt <double, MySolver> optimizer;


	optimizer.setMaxIterations(5);

	optimizer.setMaxIterationsLM(250);

	optimizer.setLambdaIncrease(5);

	optimizer.setLambdaDecrease(1e-3);

	optimizer.setThreads(0);


	// Set the minimum length of the gradient.
	// The optimizer stops minimizing if the gradient length falls below this value.
	// Set it to 0 or negative to disable this stop criterion (default is 1e-9).
	optimizer.setMinGradientLength(1e-9);

	// Set the minimum length of the step.
	// The optimizer stops minimizing if the step length falls below this value.
	// Set it to 0 or negative to disable this stop criterion (default is 1e-9).
	optimizer.setMinStepLength(1e-9);

	// Set the minimum least squares error.
	// The optimizer stops minimizing if the error falls below this value.
	// Set it to 0 or negative to disable this stop criterion (default is 0).
	optimizer.setMinError(1e-5);

	// Set the the parametrized StepSize functor used for the step calculation.
	//optimizer.setStepSize(lsq::ArmijoBacktracking<double>(0.8, 0.1, 1e-10, 1.0, 0));

	// Turn verbosity on, so the optimizer prints status updates after each iteration.
	optimizer.setVerbosity(0);

	cout << "\n -> SOLVER CONFIGURED" << endl;

	//====================================================================


	// Call solver to solve for position vector variable xval()
	// Magnetic coil filaments constants passed to the objective function
	// The simulated sensor calibration contant is passed to the objective function
	// Objective function is called in Struct 'MySolver' at the top of the program



	//auto start = high_resolution_clock::now();		// Get starting timepoint

	cout << "\n -> SOLVING FOR INITIAL GUESS" << endl;
	cout << " x : " << initialGuess(0) << endl;
	cout << " y : " << initialGuess(1) << endl;
	cout << " x : " << initialGuess(2) << endl;
	cout << " Pitch : " << initialGuess(3) << endl;
	cout << " Yaw : " << initialGuess(4) << endl << endl;

	int iterations = 0;

	sensor_flux = amplitudes;

	// Start the optimization.
	auto result = optimizer.minimize(initialGuess);

	//cout << "Done! Converged: " << (result.converged ? "true" : "false")
	//	<< " Iterations: " << result.iterations << std::endl;

	// do something with final function value
	//cout << "Final xval: " << result.xval.transpose() << std::endl;


	//auto stop = high_resolution_clock::now();        // Get stopping timepoint

	//auto duration = duration_cast<milliseconds>(stop - start);    	 // Get duration by subtracting timepoints 


	//cout << "Number of times solved : " << iterations << endl;
	//cout << "Time taken by solver for this : " << duration.count() << " milliseconds" << endl;


	return result.xval;

}