
//#include <iostream>
#include <string>
//#include <chrono>
#include <thread>
#include "TCSInit.h"

constexpr auto TCS_RESET_TIME = 20;	// seconds to reset everything in TCS
constexpr auto TCS_RUN_INTERVAL = 10; //seconds to run the tcs code

int recordSize = sizeof(CompsData);
int numRecords = comps_bin_len / recordSize;
vector<TCSComponents>			tcsComponents(numRecords);

using namespace std;



void TCS(vector<TCSComponents>& tcsComponents)
{
	/*
	* This function handles all thermal functions needed for the algorithm.
	* first, fetching the data from headers.
	* there are three sperate headers: components, heaters and sensors
	* in which there are .csv data packed about components, heaters and sensors, respectively.
	* After that, get the data from sensors.
	* Next, set components' temperature and decide what to do about them, specially sending RIDs.
	* Last, check whether anything should be done regarding the active members of thermal algorithm which
	* in this algorithm is just heaters but it can be extended to other methods and hardware as well.
	* In each step, the health of the thermal sub components and materials as well as the whole algorithm is checked
	* and respected RIDs are sent when something in violation of thermal control standards and procedures happens.
	*/

	

	
	//Check to see everything is ok
	cout << tcsComponents[1].MaxOpTemp << endl;
	cout << tcsComponents[5].sensor[2].IsDefined << endl;
	cout << tcsComponents[4].heater[0].HeaterPIDNumber << endl;

	
	for (int comps = 0; comps < numRecords; comps++)
	{
		tcsComponents[comps].getTemps();
		tcsComponents[comps].Temperature = tcsComponents[comps].setTemperature();

		tcsComponents[comps].AlgorithmBypass = tcsComponents[comps].checkBypass();
		if (!tcsComponents[comps].AlgorithmBypass)
		{
			tcsComponents[comps].decideOnTemperature();
			tcsComponents[comps].checkHeater();

		}
		else
		{
			// Do nothing. just report the temperature
		}
	}
	cout << "TCS called" << endl;
	
}


extern "C" __declspec(dllexport) int GetXValue(int Comp, int index)
{
	/*
	* Guidlines for sending values to GUI:
	* 1) The first input is the number of component
	* check the numbers according the combo_box in c# or the CompsNames in TCSInit.h
	* 2) second input is the index to identify which parameter should be sent:
	* there are a total of 41 parameters in the GUI: 15 for components, 18 for sensors
	* (6 for each), and 8 for heaters (4 for each).they are sent to the gui according to their presence
	* in the GUI. It means that first component properties are sent and then, sensor1, sensor2, sensor3,
	* heater1 and at last heater2. For each section, the order is from top to bottom.
	*/
	int value = -27300;

	if (Comp >= 0 && Comp < numRecords)
	{
		switch (index)
		{
			//Component properties
		case 0:
			value = tcsComponents[Comp].ComponentPIDNumber;
			break;
		case 1:
			value = tcsComponents[Comp].Temperature;
			break;
		case 2:
			value = tcsComponents[Comp].Status;
			break;
		case 3:
			value = tcsComponents[Comp].MaxOpTemp;
			break;
		case 4:
			value = tcsComponents[Comp].MinOpTemp;
			break;
		case 5:
			value = tcsComponents[Comp].MaxNopTemp;
			break;
		case 6:
			value = tcsComponents[Comp].MinNopTemp;
			break;
		case 7:
			value = tcsComponents[Comp].StartHeatOp;
			break;
		case 8:
			value = tcsComponents[Comp].StartHeatOp;
			break;
		case 9:
			value = tcsComponents[Comp].StartHeatNop;
			break;
		case 10:
			value = tcsComponents[Comp].StartHeatNop;
			break;
		case 11:
			value = tcsComponents[Comp].ValidSensors;
			break;
		case 12:
			value = tcsComponents[Comp].ValidHeater;
			break;
		case 13:
			value = tcsComponents[Comp].OBC;
			break;
		case 14:
			value = tcsComponents[Comp].AlgorithmBypass;
			break;
			//Sensor 1 properties
		default:
			break;
		}
	}
	else
	{
		//error selecting index in the combo box so the value is not changed
	}
	return value;
}

extern "C" __declspec(dllexport) int getComponentLength()
{
	// get the length of the components
	return numRecords;

}

extern "C" __declspec(dllexport) const char* GetObjectName(int index)
{

	if (index >= 0 && index < numRecords)
	{
		return CompsNames[index].c_str();
	}
	return nullptr; // Return null if index is invalid
}

extern "C" __declspec(dllexport) void mainFunc()
{
	
	
	//Initializing components
	cout << "initializing" << endl;
	InitializeData(tcsComponents);
	
	thread run_tcs([&]() {
		while (true) {
			TCS(tcsComponents);
			this_thread::sleep_for(chrono::seconds(TCS_RUN_INTERVAL));
		}
		});
	thread reset_tcs([&]() {
		while (true) {
			cout << "TCS Reset." << endl;
			InitializeData(tcsComponents);
			this_thread::sleep_for(chrono::seconds(TCS_RESET_TIME));
		}
		});

	run_tcs.join();
	reset_tcs.join();
	
	
}
