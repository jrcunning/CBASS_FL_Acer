
// ***** Temp Program Inputs *****
double RAMP1_START_TEMP = 30.5;
double RAMP2_START_TEMP = 30.75;
double RAMP3_START_TEMP = 30.5;
double RAMP4_START_TEMP = 30.75;

#define CHILLER_OFFSET 0.20
#define TANK1_TEMP_CORRECTION 0 // Is a temperature correction for the temp sensor, the program subtracts this from the temp readout e.g. if the sensor reads low, this should be a negative number
#define TANK2_TEMP_CORRECTION 0 // Is a temperature correction for the temp sensor, the program subtracts this from the temp readout e.g. if the sensor reads low, this should be a negative number
#define TANK3_TEMP_CORRECTION 0 // Is a temperature correction for the temp sensor, the program subtracts this from the temp readout e.g. if the sensor reads low, this should be a negative number
#define TANK4_TEMP_CORRECTION 0 // Is a temperature correction for the temp sensor, the program subtracts this from the temp readout e.g. if the sensor reads low, this should be a negative number

// ***** PID TUNING CONSTANTS ****
#define KP 2000//5000//600 //IN FIELD - Chillers had higher lag, so I adjusted the TPCwindow and KP to 20 secs, kept all proportional
#define KI 10//KP/100//27417.54//240 // March 20 IN FIELD - with 1 deg steps, no momentum to take past P control, so doubled I. (10->40)
#define KD 1000//40  //

#define RELAY_ON 0
#define RELAY_OFF 1
#define TempSensors 14
#define T1Index 0
#define T2Index 1
#define T3Index 2
#define T4Index 3
#define T1HeaterRelay 22  // Yellow T1 Heat 28  Arduino Digital I/O pin number
#define T1ChillRelay  23  // Orange T1 Chill 29 Arduino Digital I/O pin number
#define T2HeaterRelay  24  // Blue T2 Heat 26 Arduino Digital I/O pin number
#define T2ChillRelay  25  // Green T2 Chill 27 Arduino Digital I/O pin number
#define T3HeaterRelay 26 // White T3 Heat 23 Arduino Digital I/O pin number 
#define T3ChillRelay  27  // Black T3 Chill 22 Arduino Digital I/O pin number
#define T4HeaterRelay 28 // Purple T4 Heat 25 Digital I/O pin number
#define T4ChillRelay  29  // Grey T4 Chill 24 Arduino Digital I/O pin number 



byte degree[8] = // define the degree symbol
{
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000
};
