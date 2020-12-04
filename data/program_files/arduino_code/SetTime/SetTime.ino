
// Libraries for the Adafruit RGB/LCD Shield
#include <Wire.h>
// #include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <DS1307.h>


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

void checkTime()
{
  t = rtc.getTime();
  Serial.print("Time: ");
  Serial.print(t.hour, DEC);
  Serial.print(":");
  Serial.println(t.min, DEC);
}

void setup() {
  // put your setup code here, to run once:
  rtc.halt(false);

  //Comment out to set the time:
  rtc.setDOW(SUNDAY);        // Set Day-of-Week
  rtc.setTime(13, 16, 0);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(11, 10, 2020);   // Set the date (DD, MM, YYYY)
  lcd.begin(16, 2);              // start the library
  lcd.setBacklight(WHITE);
  Serial.begin(9600);          //  setup serial for sensor

}

void loop() {
  // put your main code here, to run repeatedly:
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

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("No Zeroes for single digits");

  delay(1000);

  
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Hello Ross");
  lcd.setCursor(1,1);
  lcd.print("From Rich");

  delay(1000);

}
