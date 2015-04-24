//This is an example of how we can implement the scheduling functionality
//You need to install the Time.h and TimeAlarms.h libraries
//Documentation: https://www.pjrc.com/teensy/td_libs_TimeAlarms.html (can call Alarm once, etc.)
//This sketch  triggers daily alarms at 8:30 am and 17:45 pm.

#include <Time.h>
#include <TimeAlarms.h>

void setup()
{
 setTime(8,29,0,1,1,10); // set time to 8:29:00am Jan 1 2010 (We are going to need to set the time via an NTP server)
 // create the alarms
 Alarm.alarmRepeat(8,30,0, MorningAlarm);  // 8:30am every day
 Alarm.alarmRepeat(17,45,0,EveningAlarm);  // 5:45pm every day
}

void  loop(){
 digitalClockDisplay();
 Alarm.delay(1000); // wait one second between clock display
}

// functions to be called when an alarm triggers:
void MorningAlarm(){
 Serial.println("Alarm: - turn lights off");
}

void EveningAlarm(){
 Serial.println("Alarm: - turn lights on");
}


void digitalClockDisplay()
{
 // digital clock display of the time
 Serial.print(hour());
 printDigits(minute());
 printDigits(second());
 Serial.println();
}

void printDigits(int digits)
{
 Serial.print(":");
 if(digits < 10)
   Serial.print('0');
 Serial.print(digits);
}
