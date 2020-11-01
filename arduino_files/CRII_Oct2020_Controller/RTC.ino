
String getdate()
{
   t = rtc.getTime();
   String dateStr =  String(t.year, DEC) + "_" + String(rtc.getMonthStr())  + "_" + String(t.date, DEC);
   return dateStr;
}


void checkTime()
{
  t = rtc.getTime();
  Serial.print("Time: ");
  Serial.print(t.hour, DEC);
  Serial.print(":");
  Serial.println(t.min, DEC);
}
