# ThermalControlAlgorithm

This code is dedicated to receiving the temperature data from sensors (AD590, DS1820, etc.) and then using the data to calculate the temperature for various components. In doing so, one component may have up to three (scalable) sensors and the code should assign a reasonable temperature for the component based on the data received from sensors. Furthermore, the code can detect abnormalities in the behavior of sensors and ignore the data from those sensors. After detecting the temperature, based on the component, the algorithm detects whether or not the component is in its healthy condition and if not, tries to turn it off or turn on heaters to avoid freezing. In this code, a total of 8 components are used but you can add more components or edit the current ones according to your need. All you need to do is edit three headers: components.h, sensors.h, and heaters.h. You can specify the components' names in the TCSinit.h by changing CompsNames variable.
To add your header files, you need to change your .csv files containing items to binary and then header files.
To change from .csv to binary, you can use the following Python code:
```
import struct

# Input CSV file and output binary file
# Components
# csv_file = "tcs_component.csv"
# binary_file = "components.bin"
# Sensors
#csv_file = "tcs_sensors.csv"
#binary_file = "sensors.bin"
# Heaters
csv_file = "tcs_heaters.csv"
binary_file = "heaters.bin"

# Define the binary structure format (C-style struct)
# `50s` = 50-byte string, `i` = integer
# component
# record_format = 'iiiiiiiiiiiiiiiiiiiiiii' # PID	sensor1	sensor2	sensor3	Heater1	Heater2	ComponentTemperature	ComponentHeaterStatus	MaxOpTemp	MaxNopTemp	MinOpTemp	MinNopTemp	STATUS	TCSState	ValidHeaters	ValidSensor	StartHeatop	StopHeatop, startheatnop, startheatnop, ridim, ridop,ridnop
# sensors
#record_format = 'iiiiii'  # PID, OnlineValue, State, FaultRate, PreviousValue, RID
# heaters
record_format = 'iiii'  # PID, State, Status, RID


with open(csv_file, 'r') as csv, open(binary_file, 'wb') as bin_file:
    next(csv) # Skip header line
    for line in csv:
        # Split and parse fields
        # Component
        # PID, sensor1, sensor2, sensor3, Heater1, Heater2,ComponentTemperature,ComponentHeaterStatus, MaxOpTemp, MaxNopTemp, MinOpTemp, MinNopTemp, STATUS, State, ValidHeaters, ValidSensor, StartHeatOp, StopHeatOp, StartHeatNop, StopHeatNop, RIDImplausible, RIDOpLimit, RIDNopLimit = line.strip().split(',')
        # Sensor
        #PID, OnlineValue, State, FaultRate, PreviousValue, RID = line.strip().split(',')
        # heater
        PID, State, Status, RID = line.strip().split(',')
        
        
        # Convert fields to binary format
        # Component
        #packed_data = struct.pack(record_format, int(PID), int(sensor1), int(sensor2), 
                                  # int(sensor3), int(Heater1), int(Heater2), 
                                  # int(ComponentTemperature), int(ComponentHeaterStatus), 
                                  # int(MaxOpTemp), int(MaxNopTemp), int(MinOpTemp), 
                                  # int(MinNopTemp), int(STATUS), int(State),
                                  # int(ValidHeaters), int(ValidSensor), int(StartHeatOp),
                                  # int(StopHeatOp), int(StartHeatNop), int(StopHeatNop),
                                  # int(RIDImplausible), int(RIDOpLimit), int(RIDNopLimit))
        
        # Sensor
        #packed_data = struct.pack(record_format, int(PID), int(OnlineValue),
        #                           int(State), int(FaultRate), int(PreviousValue), int(RID))
        
        # Heater
        packed_data = struct.pack(record_format, int(PID), int(State), int(Status), int(RID))
        # Write to binary file
        bin_file.write(packed_data)

print("CSV with mixed types converted to binary successfully.")
```
For each part, the items that are needed in your .csv file are described. You can comment/uncomment each part (components, sensors, and heaters) to get your relevant binary file. In the component .csv file, each component can have up to three sensors and up to two heaters. You should use 1 for each available item (sensor1, sensor2, sensor3, heater1, and heater2) and 0 for otherwise. Status is for on/off component and state is for its health. ComponentHeaterStatus is to check whether a heater is on or off for a specific component. For sensors, the default fault rate is zero. You can use any integer ranging from -500 (-50 C) to 1500 (150 C) for previous and online values. It should be noted that to avoid double numbers, all temperatures are multiplied by 10 in the code.
After creating your binary files, you can use the following Python code to get your header files. Again, comment/uncomment each part to get your relevant headers.
```
import os
# component
#input_file = "components.bin"  # Input binary file
#output_file = "components.h"   # Output header file
#sensor
#input_file = "sensors.bin"  # Input binary file
#output_file = "sensors.h"   # Output header file
# heater
input_file = "heaters.bin"  # Input binary file
output_file = "heaters.h"   # Output header file

with open(input_file, "rb") as bin_file, open(output_file, "w") as header_file:
    data = bin_file.read()
    
    #Component
    #header_file.write("unsigned char comps_bin[] = {\n")
    #Sensors
    #header_file.write("unsigned char sens_bin[] = {\n")
    #heaters
    header_file.write("unsigned char heats_bin[] = {\n")
   
    for i, byte in enumerate(data):
        header_file.write(f"  0x{byte:02x},")
        if (i + 1) % 12 == 0:  # Break line every 12 bytes
            header_file.write("\n")
    header_file.write("\n};\n")
    
    #Component
    #header_file.write(f"unsigned int comps_bin_len = {len(data)};\n")
    #sensor
    #header_file.write(f"unsigned int sens_bin_len = {len(data)};\n")
    #heaters
    header_file.write(f"unsigned int heats_bin_len = {len(data)};\n")

print(f"Binary file converted to C++ array in {output_file}.")
```
By default, all files are located in your current directory but you can change them as you need.

A RIDcontents.csv file is needed as it is provided in the list of files. Remember that the number of RIDs must match the sum of RIDs for components, sensors, and heaters. Description about the order of RIDs are provided in the ThermalControlAlgorithm.cpp above GetRIDData() function.
