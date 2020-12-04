/********************************************************
   PID RelayTempOutput Example
   Same as basic example, except that this time, the TempOutput
   is going to a digital pin which (we presume) is controlling
   a relay.  The pid is designed to TempOutput an analog value,
   but the relay can only be On/Off.

     To connect them together we use "time proportioning
   control"  Tt's essentially a really slow version of PWM.
   First we decide on a window size (5000mS say.) We then
   set the pid to adjust its TempOutput between 0 and that window
   size.  Lastly, we add some logic that translates the PID
   TempOutput into "Relay On Time" with the remainder of the
   window being "Relay Off Time"
 ********************************************************/

// Libraries for the Adafruit RGB/LCD Shield
#include <Wire.h>
// #include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
// SD card shield library
#include <SD.h>
#include <SPI.h>
// PID Library
#include <PID_v1.h>
//#include <PID_AutoTune_v0.h>
// So we can save and retrieve settings
#include <EEPROM.h>
// Libraries for the DS18B20 Temperature Sensor
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DS1307.h>
#include "settings.h"

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
#define GREEN 0x2
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

// I assume you know how to connect the DS1307.
// DS1307:  SDA pin   -> Arduino Digital 19
//          SCL pin   -> Arduino Digital 18
DS1307  rtc(19, 18);
Time  t;

// Make two files, one for logging the data and one for reading in the setpoints
File logFile;
File setpoints;
String printdate = "CRII_OCT2020"; // No spaces, dashes, or underscores   **** generated Automaticaly ****
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(TempSensors);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
// arrays to hold device addresses
DeviceAddress T1Thermometer, T2Thermometer, T3Thermometer, T4Thermometer;

//Define Variables we'll Need
//Temperature Variables
double tempT1, tempT2, tempT3, tempT4; //defining variable for handling errors
double T1SetPoint,T1TempInput,T1TempOutput,T1Correction,T1offT,T2SetPoint,T2TempInput,T2TempOutput,T2Correction,T2offT;
double T3SetPoint,T3TempInput,T3TempOutput,T3Correction,T3offT,T4SetPoint,T4TempInput,T4TempOutput,T4Correction,T4offT,ChillOffset; // With temp probe correction values
// Time Windows: Update LCD 2/sec; Serial, Ramp Status 1/sec, TPC 1/2 sec
unsigned int LCDwindow = 500, SERIALwindow = 1000, STEPwindow = 1000, i;
// misc.
//int NegIfRamping = -1, TPCwindow = KP;
int NegIfRamping = -1, TPCwindow = 10000;
unsigned int numberOfSensors = 0, SerialOutCount = 101;

// Display Conversion Strings
char T1SetPointStr[5], T2SetPointStr[5], T1TempInputStr[5], T2TempInputStr[5], T1TempOutputStr[4], T2TempOutputStr[4], ErrStr[4];
char T3SetPointStr[5], T4SetPointStr[5], T3TempInputStr[5], T4TempInputStr[5], T3TempOutputStr[4], T4TempOutputStr[4];
char hrsStr[3] = "0", minsStr[3] = "0", secsStr[3] = "0";
char T1RelayStateStr[4] = "OFF", T2RelayStateStr[4] = "OFF", T3RelayStateStr[4] = "OFF", T4RelayStateStr[4] = "OFF";


//Specify the links and initial tuning parameters
double kp = KP, ki = KI, kd = KD; //kp=350,ki= 300,kd=50;

// EEPROM addresses for persisted data
const int SpAddress = 0;
const int KpAddress = 8;
const int KiAddress = 16;
const int KdAddress = 24;

// PID Controllers
PID T1_PID(&T1TempInput, &T1TempOutput, &T1SetPoint, kp, ki, kd, DIRECT);
PID T2_PID(&T2TempInput, &T2TempOutput, &T2SetPoint, kp, ki, kd, DIRECT);
PID T3_PID(&T3TempInput, &T3TempOutput, &T3SetPoint, kp, ki, kd, DIRECT);
PID T4_PID(&T4TempInput, &T4TempOutput, &T4SetPoint, kp, ki, kd, DIRECT);

// 10 second Time Proportional Output window
int WindowSize = 10000;
unsigned long windowStartTime;

//TimeKeepers
unsigned long now_ms = millis(), hrs = 0, mins = 0, secs = 0, SERIALt, LCDt, TPCt, STEP1t, STEP2t, HOLD1t, HOLD2t;

// ************************************************
// States for state machine
// ************************************************
enum operatingState { OFF = 0, SETP, RUN, TUNE_P, TUNE_I, TUNE_D, AUTO};
operatingState opState = OFF;


void setup()
{

  RelaysInit();
  delay(2000); //Check that all relays are inactive at Reset

  // ***** INITALIZE OUTPUT *****`
  lcd.begin(16, 2);              // start the library
  lcd.setBacklight(WHITE);
  Serial.begin(9600);          //  setup serial for sensor

  PIDinit();

  sensorsInit();

  checkTime();
SDinit();
  if (ReadSettings(1440))
  {
    RampSet();
    ShowRampInfo();
  }
  else
  {
  
    Serial.println(F("No settings for this time period"));
  }



  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PrintDate is:");
  lcd.setCursor(0, 1);
  lcd.print(printdate);
  Serial.println("PrintDate is:");
  Serial.println(printdate);
  delay(3000);
   // display current time
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print(rtc.getTimeStr());
  
  // Display abbreviated Day-of-Week in the lower left corner
  lcd.setCursor(0, 1);
  lcd.print(rtc.getDOWStr(FORMAT_SHORT));
  
  // Display date in the lower right corner
  lcd.setCursor(6, 1);
  lcd.print(rtc.getDateStr());

  // Wait one second before repeating :)
  delay (1000);
  lcd.print("1. 2s Pause...");
  Serial.println();
  Serial.println();
  Serial.print("Initialization sequence.");
  Serial.println();
  Serial.print("1. 2s Pause...");
  Serial.println();
  delay(2000);
  Serial.println();

  // Relay Tests
  lcd.clear();
  lcd.setCursor(0, 0);
  //Test Relay 1 "T1 Heater"
  lcd.print("1. T1Heatr");
  Serial.print("1. T1Heatr");
  Serial.println();
  digitalWrite(T1HeaterRelay, RELAY_ON);
  delay(2000);
  digitalWrite(T1HeaterRelay, RELAY_OFF);
  //Test Relay 2 "T1 Chiller"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("2. T1Chillr");
  Serial.print("2. T1Chillr");
  Serial.println();
  digitalWrite(T1ChillRelay, RELAY_ON);
  delay(2000);
  digitalWrite(T1ChillRelay, RELAY_OFF);
  //Test Relay 3 "T2 Heater"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("3. T2Heatr");
  Serial.print("3. T2Heatr");
  Serial.println();
  digitalWrite(T2HeaterRelay, RELAY_ON);
  delay(2000);
  digitalWrite(T2HeaterRelay, RELAY_OFF);
  //Test Relay 4 "T2 Chiller"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("4. T2Chillr");
  Serial.print("4. T2Chillr");
  Serial.println();
  digitalWrite(T2ChillRelay, RELAY_ON);
  delay(2000);
  digitalWrite(T2ChillRelay, RELAY_OFF);
  Serial.println();
  lcd.clear();
  lcd.setCursor(0, 0);
  //Test Relay 5 "T3 Heater"
  lcd.print("5. T3Heatr");
  Serial.print("5. T3Heatr");
  Serial.println();
  digitalWrite(T3HeaterRelay, RELAY_ON);
  delay(2000);
  digitalWrite(T3HeaterRelay, RELAY_OFF);
  //Test Relay 6 "T3 Chiller"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("6. T3Chillr");
  Serial.print("6. T3Chillr");
  Serial.println();
  digitalWrite(T3ChillRelay, RELAY_ON);
  delay(2000);
  digitalWrite(T3ChillRelay, RELAY_OFF);
  //Test Relay 7 "T4 Heater"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("7. T4Heatr");
  Serial.print("7. T4Heatr");
  Serial.println();
  digitalWrite(T4HeaterRelay, RELAY_ON);
  delay(2000);
  digitalWrite(T4HeaterRelay, RELAY_OFF);
  //Test Relay 8 "T4 Chiller"
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("8. T4Chillr");
  Serial.print("8. T4Chillr");
  Serial.println();
  digitalWrite(T4ChillRelay, RELAY_ON);
  delay(2000);
  digitalWrite(T4ChillRelay, RELAY_OFF);
  Serial.println();
  Serial.print("PrintDate,Date,N_ms,Th,Tm,Ts,");
  Serial.print("T1SP,T1inT,T1RelayState,");
  Serial.print("T2SP,T2inT,T2RelayState,");
  Serial.print("T3SP,T3inT,T3RelayState,");
  Serial.print("T4SP,T4inT,T4RelayState,");
  Serial.println();

}


unsigned long timer24h = 0;

void loop()
{
  // ***** Time Keeping *****
  now_ms = millis();
  // Make Milliseconds into sane time
  hrs = now_ms / 3600000;
  mins = (now_ms - (hrs * 3600000)) / 60000;
  secs = (now_ms - (hrs * 3600000 + mins * 60000)) / 1000;

 // ***** INPUT FROM TEMPERATURE SENSORS *****
  sensors.requestTemperatures(); // Send the command to get temperatures
  //HTempInput=sensors.getTempCByIndex(HIndex); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  //CTempInput=sensors.getTempCByIndex(CIndex); // Why "byIndex"? You can have more than one IC on the same bus. 1 refers to the second IC on the wire
  tempT1 = sensors.getTempCByIndex(T1Index) - T1Correction;
  if (0.0 < tempT1 && tempT1 < 80.0)  T1TempInput = tempT1;

  tempT2 = sensors.getTempCByIndex(T2Index) - T2Correction;
  if (0.0 < tempT2 && tempT2 < 80.0)  T2TempInput = tempT2;

  tempT3 = sensors.getTempCByIndex(T3Index) - T3Correction;
  if (0.0 < tempT3 && tempT3 < 80.0)  T3TempInput = tempT3; // Above plus probe correction value

  tempT4 = sensors.getTempCByIndex(T4Index) - T4Correction;
  if (0.0 < tempT4 && tempT4 < 50.0)  T4TempInput = tempT4; // Above plus probe correction value

  if ((now_ms - timer24h) > 60000)
{
    checkTime();
    timer24h = now_ms;
    Serial.println("Checking settings");
    if (ReadSettings(5))
    {
      RampSet();
      ShowRampInfo();
      Serial.println("Settings loaded");
    }
    else
    {
      Serial.println("No new Settings found");
    }
  }

  // ***** UPDATE PIDs *****
  T1_PID.Compute();
  T2_PID.Compute();
  T3_PID.Compute();
  T4_PID.Compute();

  //***** UPDATE TIME WINDOW for TIME PROPORTIONAL CONTROL *****
  if (now_ms - TPCt > TPCwindow) {
//    Serial.println("UpdatedTPCt");
//    Serial.println(TPCt); 
    TPCt += TPCwindow;   //time to shift the Relay Window
  }

  //***** UPDATE RELAY STATE for TIME PROPORTIONAL CONTROL *****
  // Tank1
  if(T1TempOutput <0){//Chilling
    if(T1TempInput > T1SetPoint - ChillOffset) {digitalWrite(T1ChillRelay,RELAY_ON);digitalWrite(T1HeaterRelay,RELAY_OFF);strcpy(T1RelayStateStr,"CHL");} 
    else {digitalWrite(T1ChillRelay,RELAY_OFF);digitalWrite(T1HeaterRelay,RELAY_OFF);strcpy(T1RelayStateStr,"OFF");}
  }else{//Heating
    if(T1TempOutput > 0) {
    if(T1TempInput > T1SetPoint - ChillOffset) {digitalWrite(T1HeaterRelay,RELAY_ON);digitalWrite(T1ChillRelay,RELAY_ON);strcpy(T1RelayStateStr,"HTR");} 
    else {digitalWrite(T1HeaterRelay,RELAY_ON);digitalWrite(T1ChillRelay,RELAY_OFF);strcpy(T1RelayStateStr,"HTR");}
    }
    else {
    if(T1TempInput > T1SetPoint - ChillOffset) {digitalWrite(T1HeaterRelay,RELAY_ON);digitalWrite(T1ChillRelay,RELAY_ON);strcpy(T1RelayStateStr,"HTR");} 
    else {digitalWrite(T1HeaterRelay,RELAY_OFF);digitalWrite(T1ChillRelay,RELAY_ON);strcpy(T1RelayStateStr,"OFF");}
    }
  }
  // Tank2
  if(T2TempOutput <0){//Chilling
    if(T2TempInput > T2SetPoint - ChillOffset) {digitalWrite(T2ChillRelay,RELAY_ON);digitalWrite(T2HeaterRelay,RELAY_OFF);strcpy(T2RelayStateStr,"CHL");} 
    else {digitalWrite(T2ChillRelay,RELAY_OFF);digitalWrite(T2HeaterRelay,RELAY_OFF);strcpy(T2RelayStateStr,"OFF");}
  }else{//Heating
    if(T2TempOutput > 0) {
    if(T2TempInput > T2SetPoint - ChillOffset) {digitalWrite(T2HeaterRelay,RELAY_ON);digitalWrite(T2ChillRelay,RELAY_ON);strcpy(T2RelayStateStr,"HTR");} 
    else {digitalWrite(T2HeaterRelay,RELAY_ON);digitalWrite(T2ChillRelay,RELAY_OFF);strcpy(T2RelayStateStr,"HTR");}
    }
    else {
    if(T2TempInput > T2SetPoint - ChillOffset) {digitalWrite(T2HeaterRelay,RELAY_ON);digitalWrite(T2ChillRelay,RELAY_ON);strcpy(T2RelayStateStr,"HTR");} 
    else {digitalWrite(T2HeaterRelay,RELAY_OFF);digitalWrite(T2ChillRelay,RELAY_ON);strcpy(T2RelayStateStr,"OFF");}
    }
  }
  // Tank3
  if(T3TempOutput <0){//Chilling
    if(T3TempInput > T3SetPoint - ChillOffset) {digitalWrite(T3ChillRelay,RELAY_ON);digitalWrite(T3HeaterRelay,RELAY_OFF);strcpy(T3RelayStateStr,"CHL");} 
    else {digitalWrite(T3ChillRelay,RELAY_OFF);digitalWrite(T3HeaterRelay,RELAY_OFF);strcpy(T3RelayStateStr,"OFF");}
  }else{//Heating
    if(T3TempOutput > 0) {
    if(T3TempInput > T3SetPoint - ChillOffset) {digitalWrite(T3HeaterRelay,RELAY_ON);digitalWrite(T3ChillRelay,RELAY_ON);strcpy(T3RelayStateStr,"HTR");} 
    else {digitalWrite(T3HeaterRelay,RELAY_ON);digitalWrite(T3ChillRelay,RELAY_OFF);strcpy(T3RelayStateStr,"HTR");}
    }
    else {
    if(T3TempInput > T3SetPoint - ChillOffset) {digitalWrite(T3HeaterRelay,RELAY_ON);digitalWrite(T3ChillRelay,RELAY_ON);strcpy(T3RelayStateStr,"HTR");} 
    else {digitalWrite(T3HeaterRelay,RELAY_OFF);digitalWrite(T3ChillRelay,RELAY_ON);strcpy(T3RelayStateStr,"OFF");}
    }
  }
  // Tank4
  if(T4TempOutput <0){//Chilling
    if(T4TempInput > T4SetPoint - ChillOffset) {digitalWrite(T4ChillRelay,RELAY_ON);digitalWrite(T4HeaterRelay,RELAY_OFF);strcpy(T4RelayStateStr,"CHL");} 
    else {digitalWrite(T4ChillRelay,RELAY_OFF);digitalWrite(T4HeaterRelay,RELAY_OFF);strcpy(T4RelayStateStr,"OFF");}
  }else{//Heating
    if(T4TempOutput > 0) {
    if(T4TempInput > T4SetPoint - ChillOffset) {digitalWrite(T4HeaterRelay,RELAY_ON);digitalWrite(T4ChillRelay,RELAY_ON);strcpy(T4RelayStateStr,"HTR");} 
    else {digitalWrite(T4HeaterRelay,RELAY_ON);digitalWrite(T4ChillRelay,RELAY_OFF);strcpy(T4RelayStateStr,"HTR");}
    }
    else {
    if(T4TempInput > T4SetPoint - ChillOffset) {digitalWrite(T4HeaterRelay,RELAY_ON);digitalWrite(T4ChillRelay,RELAY_ON);strcpy(T4RelayStateStr,"HTR");} 
    else {digitalWrite(T4HeaterRelay,RELAY_OFF);digitalWrite(T4ChillRelay,RELAY_ON);strcpy(T4RelayStateStr,"OFF");}
    }
  }



  //***** UPDATE SERIAL *****
  if (now_ms - SERIALt > SERIALwindow) {
    SerialReceive();
    SerialSend();
    SERIALt += SERIALwindow;
  }

  //***** UPDATE LCD *****
  if ((now_ms - LCDt) > LCDwindow)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T1SP ");
    dtostrf(T1SetPoint, 4, 1, T1SetPointStr);
    lcd.print(T1SetPointStr);
    lcd.print("T1 ");
    dtostrf(T1TempInput, 4, 1, T1TempInputStr);
    lcd.print(T1TempInputStr);

    lcd.setCursor(0, 1);
    lcd.print("T2SP ");
    dtostrf(T2SetPoint, 4, 1, T2SetPointStr);
    lcd.print(T2SetPointStr);
    lcd.print("T2 ");
    dtostrf(T2TempInput, 4, 1, T2TempInputStr);
    lcd.print(T2TempInputStr);
    delay(2000);
    
    lcd.setCursor(0, 0);
    lcd.print("T3SP ");
    dtostrf(T3SetPoint, 4, 1, T3SetPointStr);
    lcd.print(T3SetPointStr);
    lcd.print("T3 ");
    dtostrf(T3TempInput, 4, 1, T3TempInputStr);
    lcd.print(T3TempInputStr);

    lcd.setCursor(0, 1);
    lcd.print("T4SP ");
    dtostrf(T4SetPoint, 4, 1, T4SetPointStr);
    lcd.print(T4SetPointStr);
    lcd.print("T4 ");
    dtostrf(T4TempInput, 4, 1, T4TempInputStr);
    lcd.print(T4TempInputStr);
    delay(2000);
    
    LCDt += LCDwindow;
  }
}

void SerialSend()
{ 
  if (SerialOutCount>100){
    Serial.print("PrintDate,Date,N_ms,Th,Tm,Ts,");
    Serial.print("T1SP,T1inT,TempT1,T1RelayState,");
    Serial.print("T2SP,T2inT,TempT2,T2RelayState,");
    Serial.print("T3SP,T3inT,TempT3,T3RelayState,");
    Serial.print("T4SP,T4inT,TempT4,T4RelayState,");
    Serial.println();
    logFile = SD.open("LOG.txt", FILE_WRITE);
    if (logFile) {
    logFile.print("PrintDate,Date,N_ms,Th,Tm,Ts,");
    logFile.print("T1SP,T1inT,TempT1,T1RelayState,");
    logFile.print("T2SP,T2inT,TempT2,T2RelayState,");
    logFile.print("T3SP,T3inT,TempT3,T3RelayState,");
    logFile.print("T4SP,T4inT,TempT4,T4RelayState,");
    logFile.println();
    }
  logFile.close();
  SerialOutCount=0; 
  }
  Serial.print(printdate), Serial.print(","), Serial.print(getdate()), Serial.print(","), Serial.print(now_ms), Serial.print(","), Serial.print(t.hour, DEC), Serial.print(","), Serial.print(t.min, DEC), Serial.print(","), Serial.print(t.sec, DEC), Serial.print(",");
  Serial.print(T1SetPoint), Serial.print(","), Serial.print(T1TempInput), Serial.print(","), Serial.print(tempT1), Serial.print(","), Serial.print(T1TempOutput),Serial.print(","), Serial.print(T1RelayStateStr), Serial.print(",");
  Serial.print(T2SetPoint), Serial.print(","), Serial.print(T2TempInput), Serial.print(","), Serial.print(tempT2), Serial.print(","), Serial.print(T2TempOutput),Serial.print(","), Serial.print(T2RelayStateStr), Serial.print(",");
  Serial.print(T3SetPoint), Serial.print(","), Serial.print(T3TempInput), Serial.print(","), Serial.print(tempT3), Serial.print(","), Serial.print(T3TempOutput),Serial.print(","), Serial.print(T3RelayStateStr), Serial.print(",");
  Serial.print(T4SetPoint), Serial.print(","), Serial.print(T4TempInput), Serial.print(","), Serial.print(tempT4), Serial.print(","), Serial.print(T4TempOutput),Serial.print(","), Serial.print(T4RelayStateStr), Serial.print(",");
  Serial.println();
  logFile = SD.open("log.txt", FILE_WRITE);
  if (logFile) {
    logFile.print(printdate), logFile.print(","), logFile.print(getdate()), logFile.print(","), logFile.print(now_ms), logFile.print(","), logFile.print(t.hour, DEC), logFile.print(","), logFile.print(t.min, DEC), logFile.print(","), logFile.print(t.sec, DEC), logFile.print(",");
    logFile.print(T1SetPoint), logFile.print(","), logFile.print(T1TempInput), logFile.print(","), logFile.print(tempT1), logFile.print(","), logFile.print(T1RelayStateStr), logFile.print(",");
    logFile.print(T2SetPoint), logFile.print(","), logFile.print(T2TempInput), logFile.print(","), logFile.print(tempT2), logFile.print(","), logFile.print(T2RelayStateStr), logFile.print(",");
    logFile.print(T3SetPoint), logFile.print(","), logFile.print(T3TempInput), logFile.print(","), logFile.print(tempT3), logFile.print(","), logFile.print(T3RelayStateStr), logFile.print(",");
    logFile.print(T4SetPoint), logFile.print(","), logFile.print(T4TempInput), logFile.print(","), logFile.print(tempT4), logFile.print(","), logFile.print(T4RelayStateStr), logFile.print(",");
    logFile.println();

  }
  logFile.close();
  SerialOutCount+=1;
}

void SerialReceive()
{
  if (Serial.available())
  {
    char b = Serial.read();
    Serial.flush();
  }
}
