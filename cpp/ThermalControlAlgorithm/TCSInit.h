
//#include <iostream>
//#include <string>
//#include <fstream>
#include <vector>
//#include <sstream>
#include "components.h"
#include "sensors.h"
#include "heaters.h"

constexpr auto MAX_NUMBER_SENSORS		=	3;
constexpr auto MAX_NUMBER_HEATERS		=	2;
constexpr auto MAX_SENSORS_TEMP_DIFF	=	30;
constexpr auto MAX_SENSOR_TEMPERATURE	=	1500;
constexpr auto MIN_SENSOR_TEMPERATURE	=	-500;
constexpr auto MAX_SENSOR_FLUCTUATION	=	40;
constexpr auto INVALID_TEMP				=	-2730;
constexpr auto MAX_SENSOR_FAULT			=	10;



using namespace std;

// Vector defining names of components
vector<string> CompsNames = {
	"Comp. 1", "Comp. 2", "Comp. 3", "Comp. 4",
	"Comp. 5", "Comp. 6", "Comp. 7", "Comp. 8"
};

// Vector checking which of 3 sensors has the highest and lowest value(0 lowest, 2 highest)
vector<int> SortTempValues(int sensor_0, int sensor_1, int sensor_2)
{
	vector<int>		temps(MAX_NUMBER_SENSORS*2);
	if (sensor_0 > sensor_1)
	{
		if (sensor_0 > sensor_2)
		{
			temps[2] = sensor_0;
			temps[5] = 0;
			if (sensor_1 > sensor_2)
			{
				temps[1] = sensor_1;
				temps[4] = 1;
				temps[0] = sensor_2;
				temps[3] = 2;
			}
			else
			{
				temps[1] = sensor_2;
				temps[4] = 2;
				temps[0] = sensor_1;
				temps[3] = 1;
			}
		}
		else
		{
			temps[2] = sensor_2;
			temps[1] = sensor_0;
			temps[0] = sensor_1;
			temps[3] = 1;
			temps[4] = 0;
			temps[5] = 2;
		}
	}
	else
	{
		if (sensor_1 > sensor_2)
		{
			temps[2] = sensor_1;
			temps[5] = 1;
			if (sensor_0 > sensor_2)
			{
				temps[1] = sensor_0;
				temps[0] = sensor_2;
				temps[4] = 0;
				temps[3] = 2;
			}
			else
			{
				temps[1] = sensor_2;
				temps[0] = sensor_0;
				temps[4] = 2;
				temps[3] = 0;
			}
		}
		else
		{
			temps[2] = sensor_2;
			temps[1] = sensor_1;
			temps[0] = sensor_0;
			temps[5] = 2;
			temps[4] = 1;
			temps[3] = 0;
		}
	}
	return temps;
}

// Heaters definition class
class Heaters
{
public:
	Heaters() 
	{
		HeaterPIDNumber = -1;
		State = false;
		Status = false;
		IsDefined = false;
		RID = -1;
		RIDSend = false;
	}
	//string		HeaterName; //Name		//Recheck whether it is needed
	int			HeaterPIDNumber; // PID
	//int OnTemp; //On temperature for heaters
	//int OffTemp; //Off temperature for hea
	bool		State; //health of the heater
	bool		Status; // on/off of the heater
	bool		IsDefined; // Is it defined for a certain component?
	int			RID; //RID for heater failure
	bool		RIDSend;
};

// Sensors definition class

class Sensors
{
public:
	Sensors() 
	{
		SensorPIDNumber = -1;
		OnlineValue = INVALID_TEMP;
		PreviousValue = INVALID_TEMP;
		State = true;
		FaultRate = 0;
		IsDefined = false;
		RID = -1;
		RIDSend = false;
	}
	//string		SensorName; //Name              Recheck whether it is needed
	int			SensorPIDNumber; //PID
	int			OnlineValue; // online value
	int			PreviousValue; // old value
	bool		State; // health 
	int			FaultRate; // rate of fault detection 
	bool		IsDefined; //is available for a certain component?
	int			RID; //RID for sensor fault
	bool		RIDSend;

	int SensorValidityCheck(int Fault)			// checking all conditions for the sensor to be faulty
	{
		bool	IsValid = false;
		if (abs(OnlineValue - PreviousValue) < MAX_SENSOR_FLUCTUATION)  // gradient check
		{
			IsValid = true;
		}
		else
		{
			Fault += 1;
		}
		if (!(OnlineValue > MAX_SENSOR_TEMPERATURE || OnlineValue < MIN_SENSOR_TEMPERATURE)) // limit check
		{
			IsValid = true;
		}
		else
		{
			Fault += 1;
		}
		if (Fault > 10)
		{
			IsValid = false;
		}
		return Fault;
	}

	void ResetFault()
	{
		FaultRate = 0;
		State = true;
	}
};

// Components definition class
class TCSComponents
{
public:
	TCSComponents()
	{
		ComponentName = "Null";
		ComponentPIDNumber = -1;
		Temperature = INVALID_TEMP;
		Status = false;
		MaxNopTemp = INVALID_TEMP;
		MaxOpTemp = INVALID_TEMP;
		MinOpTemp = INVALID_TEMP;
		MinNopTemp = INVALID_TEMP;
		ValidSensors = MAX_NUMBER_SENSORS;
		ValidHeaters = MAX_NUMBER_HEATERS;
		TCSState = true;
		OBCCommand = false;
		for (int i = 0; i < MAX_NUMBER_SENSORS; i++)
		{
			sensor[i].SensorPIDNumber = -1;
			sensor[i].OnlineValue = INVALID_TEMP;
			sensor[i].PreviousValue = INVALID_TEMP;
			sensor[i].State = false;
			sensor[i].FaultRate = 0;
			sensor[i].IsDefined = false;
			sensor[i].RID = -1;
			sensor[i].RIDSend = false;
		}
		for (int i = 0; i < MAX_NUMBER_HEATERS; i++)
		{
			heater[i].HeaterPIDNumber = -1;
			heater[i].State = false;
			heater[i].Status = false;
			heater[i].IsDefined = false;
			heater[i].RID = -1;
			heater[i].RIDSend = false;
		}
		OBC = false;
		RIDImplausible = -1;
		RIDNopLimit = -1;
		RIDOpLimit = -1;
		RIDImpSend = false;
		RIDOpSend = false;
		RIDNopSend = false;
		
	}
		

	string		ComponentName;
	int			ComponentPIDNumber;
	Sensors		sensor[MAX_NUMBER_SENSORS]; // By default all components have at most 3 sensors
	Heaters		heater[MAX_NUMBER_HEATERS]; // By default all components have at most 2 heaters
	int			Temperature; //Online temperature data
	bool		Status; //On or Off
	int			MaxNopTemp; // Maximum non-operational temperature
	int			MaxOpTemp; // Maximum operational temperature
	int			MinOpTemp; // Minimum operational temperature
	int			MinNopTemp; // Minimum non operational temperature
	bool		TCSState; //whether or not its temperature can be detected (valid sensor(s) are yet left)
	int			ValidSensors;
	int			ValidHeaters;
	int			StartHeatOp; // Start heater temperature when component is on
	int			StopHeatOp; // Stop heater temperature when component is on
	int			StartHeatNop; // Start heater temperature when component is off
	int			StopHeatNop; //Stop heater temperature when component is off
	bool		HasHeaters; //whether or not the component
	bool		HeaterStatus;	//whether any of the component's heater(s) are on
	bool		AlgorithmBypass;	// we can turn it on and bypass algorithm for specific component
	bool		OBCCommand;	//whether OBC asked to turn on a component or not
	bool		OBC;		//auxiliry variable for testing, consider removing it after establishing obc or ground connection to the algorithm
	int			RIDImplausible; //RID for no sensor failure
	int			RIDOpLimit;		//RID for out of operational range failure when component is on
	int			RIDNopLimit;	//RID for out of non-operational range failure when component is off
	bool		RIDOpSend;		//whether or not sending the operational RID
	bool		RIDNopSend;		//whether or not sending the non-operational RID
	bool		RIDImpSend;		//whether or not sending the implausible RID
	bool		ReceiveNewHeaterData;		// whether or not new data for heater recieved

	
	bool checkBypass()
	{
		bool Bypass = false;
		if (OBC) //After creating the method to get commands from OBC or GS, replace OBC with your own variable
		{
			Bypass = true;
		}
		else
		{
			Bypass = false;
		}
		return Bypass;
	}
	
	void getTemps()
	{
		for (int sen = 0; sen < MAX_NUMBER_SENSORS; sen++)
		{
			if (sensor[sen].IsDefined)
			{
				// Here the values should be set according totemperature sensor data or manual inputs
				//sensor[sen].PreviousValue = sensor[sen].OnlineValue;
				//sensor[sen].OnlineValue = setSensorValue();    // getting data from sensors
			}
		}
	}
	int setTemperature()
	{
		
		int				CompTemp = 250;
		if (TCSState) //if we have any valid sensor on the component
		{
			ValidSensors = MAX_NUMBER_SENSORS;
			for (int sen = 0; sen < MAX_NUMBER_SENSORS; sen++)	// Checking validity and value of each sensor for the component
			{
				
				if (sensor[sen].IsDefined)
				{
					if ((sensor[sen].SensorValidityCheck(sensor[sen].FaultRate) - sensor[sen].FaultRate) > 0 || sensor[sen].FaultRate == MAX_SENSOR_FAULT)
					{
						
						sensor[sen].FaultRate = sensor[sen].SensorValidityCheck(sensor[sen].FaultRate);
						if (sensor[sen].FaultRate > MAX_SENSOR_FAULT)
						{
							sensor[sen].FaultRate = MAX_SENSOR_FAULT;
						}
						sensor[sen].State = false;

					}
					else
					{
						sensor[sen].State = true;
						
					}
					sensor[sen].PreviousValue = sensor[sen].OnlineValue;		//updating previous value
					if (sensor[sen].State)
					{
					
						sensor[sen].ResetFault();									// if sensor is healthy, we should reset its fault rate
					}
					else if (!sensor[sen].State && sensor[sen].FaultRate == MAX_SENSOR_FAULT)
					{
						
						ValidSensors -= 1;											// reducing valid sensors by one
						//cout << "RID(4): sensor\t" << sensor[sen].RID << endl;
						sensor[sen].RIDSend = true;

					}
				}
				else
				{
					ValidSensors -= 1;
				}
			}
			if (ValidSensors == 0)
			{
				TCSState = false;
				
			}
		}
		else
		{
			//cout << "RID(3): \t"<< RIDImplausible << endl;
			RIDImpSend = true;
		}
		
		// Based on number of valid sensors, temperature is set for the component
		switch (ValidSensors)
		{
		case MAX_NUMBER_SENSORS:
		// 3 sensors
		{
			vector<int>		temps(MAX_NUMBER_SENSORS*2);		// a vector in which 0 component is the smallest, 1 is the middle, 2 is the biggest the number of sensors in order is then put in 3,4 and 5th. cells
		
			int NewTemperature = 0;
			int TotalValid = 0;
			for (int i = 0; i < MAX_NUMBER_SENSORS; i++)
			{
				if (sensor[i].State)
				{
					TotalValid += 1;
				}
			}
			
			if (TotalValid == MAX_NUMBER_SENSORS)
			{ 
				temps = SortTempValues(sensor[0].OnlineValue, sensor[1].OnlineValue, sensor[2].OnlineValue);
				if ((temps[2] - temps[0]) < MAX_SENSORS_TEMP_DIFF)
				{
					for (int counter = 0; counter < TotalValid; counter++)
					{
					
						NewTemperature += temps[counter];
					}
					NewTemperature /= TotalValid;
				}
				else
				{
					int average = (temps[0] + temps[1] + temps[2]) / MAX_NUMBER_SENSORS;
					if (average > temps[1])		// means that the two lower temperatures are closer to each other
					{
						NewTemperature = (temps[0] + temps[1]) / 2;
					}
					else						// means that the two bigger temperatures are closer to each other (average and maximum are closer to each other than average and minimum)
					{
						NewTemperature = (temps[1] + temps[2]) / 2;
					}
				}
			}
			else if (TotalValid == (MAX_NUMBER_SENSORS - 1) || (TotalValid == MAX_NUMBER_SENSORS - 2))
			{
				for (int i = 0; i < MAX_NUMBER_SENSORS; i++)
				{
					if (sensor[i].State)
					{
						NewTemperature += sensor[i].OnlineValue;
					}
				}
				NewTemperature /= TotalValid;
			}
			else
			{
				NewTemperature = Temperature;
			}
			Temperature = NewTemperature;
			break;
		}
		case MAX_NUMBER_SENSORS - 1: // 2 sensors
		{
			
			int NewTemperature = 0;
			int NumberofValid = 0;
			for (int counter = 0; counter < MAX_NUMBER_SENSORS; counter++)
			{
				if (sensor[counter].IsDefined)
				{
					if (sensor[counter].State)
					{
						NewTemperature += sensor[counter].OnlineValue;
						NumberofValid += 1;
					}
				}
				
			}
			if (!(NumberofValid == 0))
			{
				NewTemperature /= NumberofValid;
				Temperature = NewTemperature;
			}
			else
			{
				NewTemperature = Temperature;
			}
			Temperature = NewTemperature;
			break;
		}
		case MAX_NUMBER_SENSORS - 2:	// 1 sensor
		{
			int NewTemperature = 0;
			bool Valid = false;
			for (int counter = 0; counter < MAX_NUMBER_SENSORS; counter++)
			{
				if (sensor[counter].IsDefined)
				{
					if (sensor[counter].State)
					{
						NewTemperature = sensor[counter].OnlineValue;
						Valid = true;
					}
				}
				
			}
			if (Valid)
			{
				Temperature = NewTemperature;
			}
			else
			{
				NewTemperature = Temperature;
			}
			Temperature = NewTemperature;
			break;
		}
		default:
		{
			//TCSState = false;						// we have no valid sensor and can't detect temperature
			Temperature = INVALID_TEMP;					//Invalid temperature
			for (int i = 0; i < MAX_NUMBER_HEATERS; i++)
			{
				if (heater[i].IsDefined)
				{
					heater[i].State = false;
					heater[i].Status = false;//cannot determine to turn on heater so we assume everything is faulty
					heater[i].RIDSend = true;
				}
			}
			break;
		}
		}
		return Temperature;
	}
	
	void decideOnTemperature()
	{
		/*
		There are six different states based on the fact that the component is 
		on or off and its heater status (either it has heater which then, composes of
		two states (heater on or off) or it doesn't have a heater at all).
		These states are defined below:

		Component status -->		On			Off
		Heater status
			|
			v
		doesn't have				1			2
		heater is off				3			4
		heater is on				5			6


		There is also one special case where at the begining OBC asks that a component should be turned on
		so that regardless of above considerations, the algorithm tries to reach 
		*/
		if (!OBCCommand)
		{
			if (HasHeaters && TCSState) // has any heater
			{
				if (HeaterStatus) // heater is on
				{
					if (Status) //component is on
					{
						if (Temperature > StopHeatOp) //heater must be turned off
						{
							HeaterStatus = false;
						}
						else if (Temperature >= MinOpTemp && Temperature <= StopHeatOp) //normal mode
						{
							// Do nothing
						}
						else if (Temperature < MinOpTemp) // It seems that heater is faulty
						{
							HeaterStatus = false; //must turn off heaters and component and report faulty
							
						}
					}
					else //component is off
					{
						if (Temperature >= StopHeatNop) //heater must be turned off
						{
							HeaterStatus = false;
						}
						
						else if (Temperature >= MinNopTemp && Temperature <= StopHeatNop) //normal mode
						{
							// Do nothing
						}
						else if (Temperature < MinNopTemp) // It seems that heater is faulty
						{
							HeaterStatus = false;
						}
					}
				}
				else // heater is off
				{
					if (Status) //component is on
					{
						if (Temperature > StopHeatOp)
						{
							HeaterStatus = false;
						}
						
						else if (Temperature < StartHeatOp && Temperature >= MinOpTemp)
						{
							HeaterStatus = true;
						}
						else if (Temperature < MinOpTemp)
						{
							HeaterStatus = false;
						}

					}
					else //component is off
					{
						if (Temperature > StopHeatNop)
						{
							HeaterStatus = false;
						}

						else if (Temperature < StartHeatNop && Temperature >= MinNopTemp)
						{
							HeaterStatus = true;
						}
						else if (Temperature < MinNopTemp)
						{
							HeaterStatus = false;
						}

					}

				}

			}
			else if (!HasHeaters && TCSState) //doesn't have heaters
			{
				if (Status) // component is on
				{
					if (Temperature > MaxOpTemp || Temperature < MinOpTemp)
					{
						
						Status = false;
						RIDOpSend = true;
					}

				}
				else   // component is off
				{
					if (Temperature > MaxNopTemp || Temperature < MinNopTemp)
					{
						
						RIDNopSend = true;
					}

				}
			}
			else if (!TCSState) //data not available
			{
				//Do nothing
			}
		}
		else
		{
			if (Temperature < StopHeatOp && Temperature > MinNopTemp)		// tries to warm up the component
			{
				HeaterStatus = true;
			}
			else   // otherwise the heater might be faulty or tcs couldn't stop the component from freezing. In either way, OBC still can use bypass to turn on the component at its own risk.
			{
				HeaterStatus = false;
			}
		}
	}

	void checkHeater()
	{
		/* This function turns on/off the heater for those components who have at least one.
		There are two type of components with heaters, with one heater or with two heaters (hot and cold redundancy)
		Also, if the component is off, we should consider the non operational temperature rather than operational.*/
		
		if (HasHeaters) // having heater in the first place
		{
			if (HeaterStatus) // heater(s) need(s) to be turned on
			{
				int temp_split;	//variable for splitting the temperature range for two-heater components.
				if (Status) //component is on
				{
					if (ValidHeaters == MAX_NUMBER_HEATERS) // 2 heaters
					{
						temp_split = (StartHeatOp - MinOpTemp) / MAX_NUMBER_HEATERS;
						if ((StartHeatOp - Temperature) < temp_split) // heater 1 is valid
						{
					
							heater[0].Status = true;	// turn on heater 1
							heater[1].Status = false;	// make sure both heaters not turned on at the same time
						}
						else if (((StartHeatOp - Temperature) > temp_split) && (Temperature >= MinOpTemp)) // heater 1 failed
						{
							heater[0].State = false;	// heater 1 is faulty
							heater[0].Status = false;	//turn off heater 1
							
							heater[0].RIDSend = true;
							
							heater[1].Status = true;
							ValidHeaters -= 1;
						}
						else if (Temperature < MinOpTemp) // heater 2 failed
						{
							
							for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
							{
								if (heater[heat].IsDefined)
								{
									heater[heat].State = false; // heater  is faulty
									heater[heat].Status = false; //turn off heater
									heater[heat].RIDSend = true;
								}
							}
							ValidHeaters = 0;

						}
						else if (Temperature > StopHeatOp) // component is warm enough
						{
							heater[0].Status = false;	//turn off heater 1
							heater[1].Status = false;	//turn off heater 2
						}
					}
					else if (ValidHeaters == MAX_NUMBER_HEATERS - 1) // 1 heater
					{
						if (Temperature > StopHeatOp)// turning off heaters
						{
							for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
							{
								if (heater[heat].IsDefined)
								{
									heater[heat].Status = false; //turning off heaters
								}
							}
						}
						else if (Temperature < StartHeatOp && Temperature >= MinOpTemp) //turning on heaters
						{
							for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
							{
								if (heater[heat].IsDefined && heater[heat].State)
								{
									heater[heat].Status = true; //turning on heaters
									break;
								}
							}
						}
						else if (Temperature < MinOpTemp) //faulty
						{
							for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
							{
								if (heater[heat].IsDefined)
								{
									heater[heat].State = false; // heater 1 is faulty
									heater[heat].Status = false; //turn off heater
									heater[heat].RIDSend = true;

								}
							}
							
						}
					}
					
				}
				else //component is off
				{
					if (ValidHeaters == MAX_NUMBER_HEATERS) // 2 heaters
					{
						temp_split = (StartHeatNop - MinNopTemp) / 2;
						if ((StartHeatNop - Temperature) < temp_split) // heater 1 is valid
						{
							heater[0].Status = true;
							heater[1].Status = false; //make sure both heaters do not turn on at the same time
							
						}
						else if (((StartHeatNop - Temperature) > temp_split) && (Temperature >= MinNopTemp)) // heater 1 failed
						{
							heater[0].State = false;	// heater 1 is faulty
							heater[0].Status = false;	//turn off heater 1
							
							heater[0].RIDSend = true;
							
							heater[1].Status = true; //turning on heater 2
							ValidHeaters -= 1;
						}
						else if (Temperature < MinNopTemp) // heater 2 failed
						{
							for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
							{
								if (heater[heat].IsDefined)
								{
									heater[heat].State = false; // heater  is faulty
									heater[heat].Status = false; //turn off heater
									heater[heat].RIDSend = true;
								}
							}
							ValidHeaters = 0;
						}
						else if (Temperature > StopHeatNop) // component is warm enough
						{
							heater[0].Status = false;	//turn off heater 1
							heater[1].Status = false;	//turn off heater 2
						}
					}
					else if (ValidHeaters == MAX_NUMBER_HEATERS - 1) // 1 heater
					{
						if (Temperature > StopHeatNop)// turning off heaters
						{
							heater[0].Status = false;
							heater[1].Status = false;
						}
						else if (Temperature < StartHeatNop && Temperature >= MinNopTemp) //turning on heater
						{
							if (heater[0].State)
							{
								heater[0].Status = true; //turn on heater 1
							}
							else
							{
								heater[1].Status = true; //turn on heater 2
							}
						}
						else if (Temperature < MinNopTemp) //faulty
						{
							for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
							{
								if (heater[heat].IsDefined)
								{
									heater[heat].State = false; // heater  is faulty
									heater[heat].Status = false; //turn off heater
									heater[heat].RIDSend = true;
								}
							}
							
						}
					}
				}
			}
			else //heater must be turned off
			{
				if (Status) //component is on
				{
					if (Temperature > MaxOpTemp)   //turning off heaters and report faulty for component
					{
						for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
						{
							if (heater[heat].IsDefined)
							{
								heater[heat].Status = false; //turning off heaters
							}
						}
						Status = false; //component must be turned off
						RIDOpSend = true; //Operational limit exceeded
					}
					else if (Temperature > StopHeatOp && Temperature <= MaxOpTemp) //just turning off heaters
					{
						for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
						{
							if (heater[heat].IsDefined)
							{
								heater[heat].Status = false; //turning off heaters
								
							}
						}
						
					}
					else if (Temperature < MinOpTemp) // turning off heaters and report faluty for both heaters and component
					{
						for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
						{
							if (heater[heat].IsDefined)
							{
								heater[heat].Status = false; //turning off heaters
								heater[heat].State = false; // reporting faulty
								heater[heat].RIDSend = true; //heaters are reported as faulty
							}
						}
						Status = false; //component must be turned off
						RIDOpSend = true; //Operational limit exceeded
					}
				}
				else //component is off
				{
					if (Temperature > MaxNopTemp)   //turning off heaters and report faulty for component
					{
						for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
						{
							if (heater[heat].IsDefined)
							{
								heater[heat].Status = false; //turning off heaters
							}
						}
						
						RIDNopSend = true; //Non operational limit exceeded
					}
					else if (Temperature > StopHeatNop && Temperature <= MaxNopTemp) //just turning off heaters
					{
						for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
						{
							if (heater[heat].IsDefined)
							{
								heater[heat].Status = false; //turning off heaters

							}
						}

					}
					else if (Temperature < MinNopTemp) // turning off heaters and report faluty for both heaters and component
					{
						for (int heat = 0; heat < MAX_NUMBER_HEATERS; heat++)
						{
							if (heater[heat].IsDefined)
							{
								heater[heat].Status = false; //turning off heaters
								heater[heat].State = false; // reporting faulty
								heater[heat].RIDSend = true; //heaters are reported as faulty
							}
						}
						
						RIDNopSend = true; //Non operational limit exceeded
					}



				}

			}
			
		}
	}
	
	/*enabling copy paste between objects. for instance: 
	TCSComponents a;
	... some changes to a's properties take place here
	TCSComponents b;
	b = a;	values and properties are set for b from a
	*/
	TCSComponents(const TCSComponents& other) = default;
	TCSComponents& operator=(const TCSComponents& other) = default;
	
};

// Define the component structure
struct CompsData {
	int ComponentPID;
	int sensor1;
	int	sensor2;
	int	sensor3;
	int Heater1;
	int Heater2;
	int ComponentTemperature;
	int ComponentHeaterStatus;
	int	MaxOpTemp;
	int	MaxNopTemp;
	int	MinOpTemp;
	int MinNopTemp;
	int STATUS;
	int	TCSState;
	int ValidHeaters;
	int ValidSensor;
	int StartHeatOp;
	int StopHeatOp;
	int StartHeatNop;
	int StopHeatNop;
	int RIDImplausible;
	int RIDOpLimit;
	int RIDNopLimit;
};

// Define the sensors structure
struct SensData {
	int SensorPID;
	int OnlineValue;
	int	State;
	int	FaultRate;
	int PreviousValue;
	int RID;
};

// Define the heater structure
struct HeatsData {
	int HeaterPID;
	/*int OnTemp;		// Please check again. I think we don't need these two parameters while having them in the component structure
	int	OffTemp;*/
	int	State;
	int Status;
	int RID;
};

int parseSensBinaryData(TCSComponents& Component, int num, int sensor_number) {
	int recordSize = sizeof(SensData);
	int numRecords = sens_bin_len / recordSize;
	


	SensData* record = (SensData*)(sens_bin + num * recordSize);
	Component.sensor[sensor_number].OnlineValue = record->OnlineValue;					//Temperature value
	Component.sensor[sensor_number].PreviousValue = record->PreviousValue;				//Previous value
	Component.sensor[sensor_number].SensorPIDNumber = record->SensorPID;				//PID
	Component.sensor[sensor_number].State = (bool)record->State;						//validity of the sensor
	Component.sensor[sensor_number].FaultRate = record->FaultRate;						//Fault rate of the sensor
	Component.sensor[sensor_number].RID = record->RID;									//RID

	num += 1;
	return num;
}


int parseHeatsBinaryData(TCSComponents& Component, int num, int heater_number) {
	int recordSize = sizeof(HeatsData);
	int numRecords = heats_bin_len / recordSize;
	
	HeatsData* record = (HeatsData*)(heats_bin + num * recordSize);
	Component.heater[heater_number].HeaterPIDNumber = record->HeaterPID;				//PID
	Component.heater[heater_number].State = (bool)record->State;						//whether or not the heater is ok
	Component.heater[heater_number].Status = (bool)record->Status;						//on/off
	Component.heater[heater_number].RID = record->RID;									//RID
	


	num += 1;
	return num;

}

void InitializeData(vector<TCSComponents>& AllComponents) {
	int recordSize = sizeof(CompsData);
	int numRecords = comps_bin_len / recordSize;

	int recordSizeHeat		= sizeof(HeatsData);
	int numRecordsHeat		= heats_bin_len / recordSizeHeat;
	int heaterNumber		= 0;
	

	
	int sensorNumber = 0;
	
	for (int i = 0; i < numRecords; i++) {
		CompsData* record = (CompsData*)(comps_bin + i * recordSize);

		AllComponents[i].ComponentName = CompsNames[i];
		AllComponents[i].ComponentPIDNumber =	record->ComponentPID;	//PID
		AllComponents[i].sensor[0].IsDefined = (bool)record->sensor1;	//validity of sensor 1
		AllComponents[i].sensor[1].IsDefined = (bool)record->sensor2;	//validity of sensor 2
		AllComponents[i].sensor[2].IsDefined = (bool)record->sensor3;	//validity of sensor 3
		for (int sensor = 0; sensor < MAX_NUMBER_SENSORS; sensor++)
		{
			if (AllComponents[i].sensor[sensor].IsDefined)
			{
				sensorNumber = parseSensBinaryData(AllComponents[i], sensorNumber, sensor);
				
				
			}
		}
		
		AllComponents[i].heater[0].IsDefined = (bool)record->Heater1;	//validity of heater 1
		AllComponents[i].heater[1].IsDefined = (bool)record->Heater2;	//validity of heater 2
		for (int heater = 0; heater < MAX_NUMBER_HEATERS; heater++)
		{
			if (AllComponents[i].heater[heater].IsDefined)
			{
				
				heaterNumber = parseHeatsBinaryData(AllComponents[i], heaterNumber, heater);
			}
		}
		AllComponents[i].Temperature = record->ComponentTemperature;		//Online temperature
		AllComponents[i].MaxOpTemp = record->MaxOpTemp;						//maximum operational temperature
		AllComponents[i].MaxNopTemp = record->MaxNopTemp;					//maximum non operational temperature
		AllComponents[i].MinOpTemp = record->MinOpTemp;						//minimum operational temperature
		AllComponents[i].MinNopTemp = record->MinNopTemp;					//minimum non operational temperature
		AllComponents[i].Status = (bool)record->STATUS;						// On/Off
		AllComponents[i].TCSState = (bool)record->TCSState;					//whehther or not there are non faulty sensors available
		AllComponents[i].ValidHeaters = record->ValidHeaters;				//Number of valid heaters
		AllComponents[i].ValidSensors = record->ValidSensor;				//Number of valid sensors
		AllComponents[i].StartHeatOp = record->StartHeatOp;						//temperature below which heater starts (component is on)
		AllComponents[i].StopHeatOp = record->StopHeatOp;						//temperature above which heater stops (component is on)
		AllComponents[i].StartHeatNop = record->StartHeatNop;						//temperature below which heater starts (component is off)
		AllComponents[i].StopHeatNop = record->StopHeatNop;						//temperature above which heater stops (component is off)
		AllComponents[i].HeaterStatus = (bool) record ->ComponentHeaterStatus;		// whether any heater is on 
		AllComponents[i].AlgorithmBypass = false;
		AllComponents[i].RIDImplausible = record->RIDImplausible;					// RID no sensor
		AllComponents[i].RIDOpLimit = record->RIDOpLimit;							// RID operational limit
		AllComponents[i].RIDNopLimit = record->RIDNopLimit;							// RID non operational limit
			
	}

	
} 

void RuntimeInitialize(vector<TCSComponents>& New, vector<TCSComponents>& Old)
{
	Old = New;
}




// reading input data from .csv file  (Obsolete)
//class CSVReader {
//
//private:
//	string					filePath;
//	vector<string>			headers;
//	vector<vector<string>>	data;
//
//public:
//	//Constructor
//	CSVReader(const string& filePath):
//		filePath(filePath) {}
//
//	// Function to parse the CSV file
//	bool parseCSV() {
//		ifstream	file(filePath);
//		if (!file.is_open()) {
//			cerr << "Error: could not open file" << filePath << endl;
//			return false;
//		}
//		
//		string		line;
//		bool isHeader = true;
//
//		while (getline(file, line)) {
//			stringstream		lineStream(line);
//			string				cell;
//			vector<string>		row;
//
//			while (getline(lineStream, cell, ',')) {
//				row.push_back(cell);
//			}
//
//			if (isHeader) {
//				headers = row;
//				isHeader = false;
//			}
//			else {
//				data.push_back(row);
//			}
//		}
//
//		file.close();
//		return true;
//	}
//
//	//Function to get the headers
//	const vector<string>& getHeaders()
//		const {
//		return headers;
//	}
//
//	//Function to get a specific column's data
//	vector<string> getColumnData(const string& header) const {
//		vector<string>		columnData;
//		auto it = find(headers.begin(), headers.end(), header);
//
//		if (it == headers.end()) {
//			cerr << "Error: Header\"" << header << "\" not found" << endl;
//			return columnData;
//		}
//
//		size_t colIndex =
//			distance(headers.begin(), it);
//		for (const auto& row : data) {
//			if (colIndex < row.size()) {
//				columnData.push_back(row[colIndex]);
//			}
//		}
//		return columnData;
//	}
//
//	//Function to create columns with the name of the respected header
//	vector<vector<string>> getAllColumns()
//	{
//		
//		
//		size_t header_size = getHeaders().size();
//		size_t component_size = getColumnData(getHeaders()[0]).size();
//		
//		vector<vector<string>>			AllComponents(component_size, vector<string>(header_size, ""));
//		for (int comp = 0; comp < component_size; comp++)
//		{
//			for (int head = 0; head < header_size; head++)
//			{
//				AllComponents[comp][head] = getColumnData(getHeaders()[head])[comp];
//			}
//		}
//		
//		return AllComponents;
//
//	}
//
//	vector<size_t> getSize()
//	{
//		size_t header_size = getHeaders().size();
//		cout << header_size;
//		size_t component_size = getColumnData(getHeaders()[0]).size();
//		vector<size_t>		a = { component_size, header_size };
//		return a;
//	}
//
//
//
//	
//
//	//Function to display the parsed data (for debugging)
//
//	void displayData() const {
//		for (const auto& header : headers) {
//			cout << header << "\t";
//		}
//		cout << endl;
//
//		for (const auto& row : data) {
//			for (const auto& cell : row) {
//				cout << cell << "\t";
//			}
//			cout << endl;
//		}
//	}
//
//};
