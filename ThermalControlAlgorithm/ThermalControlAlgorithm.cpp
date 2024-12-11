#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include "TCSInit.h"

constexpr auto TCS_RESET_TIME = 1800;	// seconds to reset everything in TCS
constexpr auto TCS_RUN_INTERVAL = 10; //seconds to run the tcs code

int recordSize = sizeof(CompsData);
int numRecords = comps_bin_len / recordSize;

using namespace std;


vector<TCSComponents> TCS()
{
	/*
	* This function handles all thermal functions needed for the algorithm.
	* first, fetching the data from headers.
	* there are three sperate headers: components, heaters and sensors
	* in which there are .csv data packed about components, heaters and sensors, respectively.
	* After that, get the data from sensors.
	* Next, set components' temperature and decide what to do about them, specially sending RIDs.
	* Last, check whether anything should be done regarding the active members of thermal algorithm which
	* in this algorithm is just heaters but it can be extended to other method and hardware as well.
	* In each step, the health of thermal sub components and materials as well as the whole algorithm is checked
	* and respected RIDs are sent when something in violation of thermal control standards and procedures happens.
	*/

	vector<TCSComponents>			tcsComponents(numRecords);

	tcsComponents = InitializeData();
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
	return tcsComponents;
};


vector<TCSComponents> TCSReset()
{
	cout << "TCS RESET called" << endl;
	return InitializeData();
	
};

int main() 
{
	
	vector<TCSComponents>			tcsComponents(numRecords);
	
	thread run_tcs([&]() {
		while (true) {
			tcsComponents = TCS();
			this_thread::sleep_for(chrono::seconds(TCS_RUN_INTERVAL));
		}
		});
	thread reset_tcs([&]() {
		while (true) {
			tcsComponents = TCSReset();
			this_thread::sleep_for(chrono::seconds(TCS_RESET_TIME));
		}
		});

	run_tcs.join();
	reset_tcs.join();
	
	return 0;
};