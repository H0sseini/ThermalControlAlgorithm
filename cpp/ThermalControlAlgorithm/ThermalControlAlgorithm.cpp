
<<<<<<< HEAD
//#include <iostream>
=======
#include <iostream>
>>>>>>> 346009d3cbb0fd4d7141c41c93a6204dbedd4bfd
#include <string>
//#include <chrono>
#include <thread>
#include "TCSInit.h"

constexpr auto TCS_RESET_TIME = 1800;	// seconds to reset everything in TCS
<<<<<<< HEAD
<<<<<<< HEAD
constexpr auto TCS_RUN_INTERVAL = 10; //seconds to run the tcs code. this parameter is sent to c# for visualization)
=======
extern "C" {
	__declspec(dllexport) constexpr auto TCS_RUN_INTERVAL = 10; //seconds to run the tcs code. this parameter is sent to c# for visualization)
}
>>>>>>> 346009d3cbb0fd4d7141c41c93a6204dbedd4bfd
=======
extern "C" {
	__declspec(dllexport) constexpr auto TCS_RUN_INTERVAL = 10; //seconds to run the tcs code. this parameter is sent to c# for visualization)
}
>>>>>>> 346009d3cbb0fd4d7141c41c93a6204dbedd4bfd


int recordSize = sizeof(CompsData);
int numRecords = comps_bin_len / recordSize;
vector<TCSComponents>			tcsComponents(numRecords);
vector<TCSComponents>			tcsBackup(numRecords);

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

	
	for (int comps = 0; comps < numRecords; comps++)
	{
		tcsComponents[comps].getTemps(); //getting values from sensors
		tcsComponents[comps].Temperature = tcsComponents[comps].setTemperature(); //analyzing and setting temperatures for components
		

		tcsComponents[comps].AlgorithmBypass = tcsComponents[comps].checkBypass();
		if (!tcsComponents[comps].AlgorithmBypass)
		{
			tcsComponents[comps].decideOnTemperature(); //check to see whether anything should be done or not
			// if component has heaters, the decision about turning on/off heaters sending RIDs and turning off component is made in the next step
			// (chechHeater) and here just the status quo of the component is decided. Otherwise (no heaters), RIDs are sent here.
			tcsComponents[comps].checkHeater(); // making decision about the status of heaters and corresponding component

		}
		else
		{
			// Do nothing. just report the temperature
		}
	}
<<<<<<< HEAD
<<<<<<< HEAD
	//cout << "TCS called" << endl;
=======
>>>>>>> 346009d3cbb0fd4d7141c41c93a6204dbedd4bfd
=======
>>>>>>> 346009d3cbb0fd4d7141c41c93a6204dbedd4bfd
	
	//cout << "TCS called" << endl;
	
}


extern "C" __declspec(dllexport) int getComponentLength()
{
	// get the length of the components
	return numRecords;

}

extern "C" __declspec(dllexport) const char* getObjectName(int index)
{

	if (index >= 0 && index < numRecords)
	{
		return CompsNames[index].c_str();
	}
	return nullptr; // Return null if index is invalid
}

/*
	Sending component data from c++ to c# using functions.
*/
extern "C" 
{
	__declspec(dllexport) int sendCompPID(int index)
	{
		return tcsComponents[index].ComponentPIDNumber;
	}
	__declspec(dllexport) int sendCompTemp(int index)
	{
		return tcsComponents[index].Temperature;
	}
	__declspec(dllexport) int sendCompStatus(int index)
	{
		return tcsComponents[index].Status;
	}
	__declspec(dllexport) int sendCompMaxOpTemp(int index)
	{
		return tcsComponents[index].MaxOpTemp;
	}
	__declspec(dllexport) int sendCompMinOpTemp(int index)
	{
		return tcsComponents[index].MinOpTemp;
	}
	__declspec(dllexport) int sendCompMaxNopTemp(int index)
	{
		return tcsComponents[index].MaxNopTemp;
	}
	__declspec(dllexport) int sendCompMinNopTemp(int index)
	{
		return tcsComponents[index].MinNopTemp;
	}
	__declspec(dllexport) int sendCompStartHeatOp(int index)
	{
		return tcsComponents[index].StartHeatOp;
	}
	__declspec(dllexport) int sendCompStopHeatOp(int index)
	{
		return tcsComponents[index].StopHeatOp;
	}
	__declspec(dllexport) int sendCompStartHeatNop(int index)
	{
		return tcsComponents[index].StartHeatNop;
	}
	__declspec(dllexport) int sendCompStopHeatNop(int index)
	{
		return tcsComponents[index].StopHeatNop;
	}
	__declspec(dllexport) int sendValidSensor(int index)
	{
		return tcsComponents[index].ValidSensors;
	}
	__declspec(dllexport) int sendCompValidHeater(int index)
	{
		return tcsComponents[index].ValidHeaters;
	}
	__declspec(dllexport) int sendCompOBC(int index)
	{
		return tcsComponents[index].OBCCommand;
	}
	__declspec(dllexport) int sendCompBypass(int index)
	{
		return tcsComponents[index].AlgorithmBypass;
	}
	__declspec(dllexport) int sendCompTCSState(int index)
	{
		return tcsComponents[index].TCSState;
	}
}
/*
	Sending sensor 1 data from c++ to c# 
*/
extern "C"
{
	__declspec(dllexport) int sendSens1Defined(int index)
	{
		return tcsComponents[index].sensor[0].IsDefined;
	}
	__declspec(dllexport) int sendSens1PID(int index)
	{
		return tcsComponents[index].sensor[0].SensorPIDNumber;
	}
	__declspec(dllexport) int sendSens1State(int index)
	{
		return tcsComponents[index].sensor[0].State;
	}
	__declspec(dllexport) int sendSens1OnlineVal(int index)
	{
		return tcsComponents[index].sensor[0].OnlineValue;
	}
	__declspec(dllexport) int sendSens1PrevVal(int index)
	{
		return tcsComponents[index].sensor[0].PreviousValue;
	}
	__declspec(dllexport) int sendSens1Fault(int index)
	{
		return tcsComponents[index].sensor[0].FaultRate;
	}
}

/*
	Sending sensor 2 data from c++ to c#
*/
extern "C"
{
	__declspec(dllexport) int sendSens2Defined(int index)
	{
		return tcsComponents[index].sensor[1].IsDefined;
	}
	__declspec(dllexport) int sendSens2PID(int index)
	{
		return tcsComponents[index].sensor[1].SensorPIDNumber;
	}
	__declspec(dllexport) int sendSens2State(int index)
	{
		return tcsComponents[index].sensor[1].State;
	}
	__declspec(dllexport) int sendSens2OnlineVal(int index)
	{
		return tcsComponents[index].sensor[1].OnlineValue;
	}
	__declspec(dllexport) int sendSens2PrevVal(int index)
	{
		return tcsComponents[index].sensor[1].PreviousValue;
	}
	__declspec(dllexport) int sendSens2Fault(int index)
	{
		return tcsComponents[index].sensor[1].FaultRate;
	}
}

/*
	Sending sensor 3 data from c++ to c#
*/
extern "C"
{
	__declspec(dllexport) int sendSens3Defined(int index)
	{
		return tcsComponents[index].sensor[2].IsDefined;
	}
	__declspec(dllexport) int sendSens3PID(int index)
	{
		return tcsComponents[index].sensor[2].SensorPIDNumber;
	}
	__declspec(dllexport) int sendSens3State(int index)
	{
		return tcsComponents[index].sensor[2].State;
	}
	__declspec(dllexport) int sendSens3OnlineVal(int index)
	{
		return tcsComponents[index].sensor[2].OnlineValue;
	}
	__declspec(dllexport) int sendSens3PrevVal(int index)
	{
		return tcsComponents[index].sensor[2].PreviousValue;
	}
	__declspec(dllexport) int sendSens3Fault(int index)
	{
		return tcsComponents[index].sensor[2].FaultRate;
	}
}

/*
	Sending heater 1 data from c++ to c#
*/
extern "C"
{
	__declspec(dllexport) int sendHeater1Defined(int index)
	{
		return tcsComponents[index].heater[0].IsDefined;
	}
	__declspec(dllexport) int sendHeater1PID(int index)
	{
		return tcsComponents[index].heater[0].HeaterPIDNumber;
	}
	__declspec(dllexport) int sendHeater1State(int index)
	{
		return tcsComponents[index].heater[0].State;
	}
	__declspec(dllexport) int sendHeater1Status(int index)
	{
		return tcsComponents[index].heater[0].Status;
	}
}

/*
	Sending heater 2 data from c++ to c#
*/
extern "C"
{
	__declspec(dllexport) int sendHeater2Defined(int index)
	{
		return tcsComponents[index].heater[1].IsDefined;
	}
	__declspec(dllexport) int sendHeater2PID(int index)
	{
		return tcsComponents[index].heater[1].HeaterPIDNumber;
	}
	__declspec(dllexport) int sendHeater2State(int index)
	{
		return tcsComponents[index].heater[1].State;
	}
	__declspec(dllexport) int sendHeater2Status(int index)
	{
		return tcsComponents[index].heater[1].Status;
	}

}

// Receiving data from C#
/* Check bypassing conditions
	This function check whether GUI/GS/OBC sends commands, if so, the algorithm must store new conditions
	and use those values for the next time it initializes (resets). These parameters, currently, include
	bypassing algorithm (AlgorithmBypass), obc/gs command (OBC) and heater start/stop temperatures.
*/
extern "C" __declspec(dllexport) void ReceiveDataFromGSorOBC(int Comp, bool CompStat, bool OBC, 
	bool Bypass, int Sen1, int Sen2, int Sen3, int HeatStartOp, int HeatStopOp, int HeatStartNop, int HeatStopNop)
{
	tcsComponents[Comp].Status = CompStat;
	tcsComponents[Comp].OBCCommand = OBC;
	tcsComponents[Comp].OBC = Bypass;
	tcsComponents[Comp].sensor[0].PreviousValue = tcsComponents[Comp].sensor[0].OnlineValue;
	tcsComponents[Comp].sensor[1].PreviousValue = tcsComponents[Comp].sensor[1].OnlineValue;
	tcsComponents[Comp].sensor[2].PreviousValue = tcsComponents[Comp].sensor[2].OnlineValue;
	tcsComponents[Comp].sensor[0].OnlineValue = Sen1;
	tcsComponents[Comp].sensor[1].OnlineValue = Sen2;
	tcsComponents[Comp].sensor[2].OnlineValue = Sen3;
	
	tcsComponents[Comp].StartHeatOp = HeatStartOp;
	tcsComponents[Comp].StopHeatOp = HeatStopOp;
	tcsComponents[Comp].StartHeatNop = HeatStartNop;
	tcsComponents[Comp].StopHeatNop = HeatStopNop;
	
	//backing up important data to tcsBackup
	tcsBackup[Comp].Status = CompStat;
	tcsBackup[Comp].OBCCommand = OBC;
	tcsBackup[Comp].OBC = Bypass;
	

	tcsBackup[Comp].StartHeatOp = HeatStartOp;
	tcsBackup[Comp].StopHeatOp = HeatStopOp;
	tcsBackup[Comp].StartHeatNop = HeatStartNop;
	tcsBackup[Comp].StopHeatNop = HeatStopNop;
	
}

// Sending RID size
extern "C" 
{
	__declspec(dllexport) int GetRIDSize()
	{
		int RIDSize = 0;
		for (int comps = 0; comps < numRecords; comps++)
		{
			RIDSize += 3;	// Number of RIDs per component
			for (int sensor = 0; sensor < MAX_NUMBER_SENSORS; sensor++)
			{
				if (tcsComponents[comps].sensor[sensor].IsDefined)
				{
					RIDSize += 1;	// Number of RIDs per sensor
				}
			}
			for (int heater = 0; heater < MAX_NUMBER_HEATERS; heater++)
			{
				if (tcsComponents[comps].heater[heater].IsDefined)
				{
					RIDSize += 1;	//Number of RIDs per heater
				}
			}
		}
		return (RIDSize);
	}
}
// defining RIDData vector globally
int RIDsize = GetRIDSize();
vector<int>		RIDData(RIDsize);

void InitializeRID()
{
	fill(RIDData.begin(), RIDData.end(), -1);
}

//Sending RIDs
/*
Order of sending RIDs are as follows:
1) Implausible for all components
2) Operational limits for all components
3) Non operational limits for all components
4) faults of heaters
5) faults of sensors
Please sort your RIDs in compilance to the above format or otherwise change the rules according to your format.
*/
extern "C"
{
	__declspec(dllexport) int* GetRIDData()
	{
		int counter = 0;
		for (int comps = 0; comps < numRecords; comps++)
		{
			if (tcsComponents[comps].RIDImpSend)
			{
				RIDData[counter] = tcsComponents[comps].RIDImplausible;
			}
			counter++;
		}
		
		for (int comps = 0; comps < numRecords; comps++)
		{
			if (tcsComponents[comps].RIDOpSend)
			{
				RIDData[counter] = tcsComponents[comps].RIDOpLimit;
			}
			counter++;
		}
		for (int comps = 0; comps < numRecords; comps++)
		{
			if (tcsComponents[comps].RIDNopSend)
			{
				RIDData[counter] = tcsComponents[comps].RIDNopLimit;

			}
			counter++;
		}
		for (int comps = 0; comps < numRecords; comps++)
		{
			for (int heater = 0; heater < MAX_NUMBER_HEATERS; heater++)
			{
				if (tcsComponents[comps].heater[heater].IsDefined)
				{
					if (tcsComponents[comps].heater[heater].RIDSend)
					{
						RIDData[counter] = tcsComponents[comps].heater[heater].RID;
					}
					counter++;
				}
			}
		}
		for (int comps = 0; comps < numRecords; comps++)
		{
			for (int sensor = 0; sensor < MAX_NUMBER_SENSORS; sensor++)
			{
				if (tcsComponents[comps].sensor[sensor].IsDefined)
				{
					if (tcsComponents[comps].sensor[sensor].RIDSend)
					{
						RIDData[counter] = tcsComponents[comps].sensor[sensor].RID;
					}
					counter++;
				}
			}
		}
		return RIDData.data();
	}
}




extern "C" __declspec(dllexport) int mainFunc()
{
	
	
	//Initializing components
	//cout << "initializing" << endl;
<<<<<<< HEAD
<<<<<<< HEAD
	InitializeData(tcsComponents);
	
=======
=======
>>>>>>> 346009d3cbb0fd4d7141c41c93a6204dbedd4bfd
	InitializeData(tcsBackup);
	tcsComponents = tcsBackup;
	RIDsize = GetRIDSize();
	RIDData.resize(RIDsize);
	InitializeRID();
	GetRIDData();

<<<<<<< HEAD
>>>>>>> 346009d3cbb0fd4d7141c41c93a6204dbedd4bfd
=======
>>>>>>> 346009d3cbb0fd4d7141c41c93a6204dbedd4bfd
	thread run_tcs([&]() {
		while (true) {
			
			TCS(tcsComponents);
			this_thread::sleep_for(chrono::seconds(TCS_RUN_INTERVAL));
		}
		});
	thread reset_tcs([&]() {
		while (true) {
			//cout << "TCS Reset." << endl;
<<<<<<< HEAD
<<<<<<< HEAD
			InitializeData(tcsComponents);
=======
			tcsComponents = tcsBackup;
			
>>>>>>> 346009d3cbb0fd4d7141c41c93a6204dbedd4bfd
=======
			tcsComponents = tcsBackup;
			
>>>>>>> 346009d3cbb0fd4d7141c41c93a6204dbedd4bfd
			this_thread::sleep_for(chrono::seconds(TCS_RESET_TIME));
		}
		});

	reset_tcs.join();
	run_tcs.join();
	
	
	return 0;
}
