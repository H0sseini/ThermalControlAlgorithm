

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