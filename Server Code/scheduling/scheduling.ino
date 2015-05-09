//This is an example of how we can implement the scheduling functionality
//You need to install the Time.h and TimeAlarms.h libraries
//Documentation: https://www.pjrc.com/teensy/td_libs_TimeAlarms.html (can call Alarm once, etc.)
//This sketch  triggers daily alarms at 8:30 am and 12:00 am.

#include <Time.h>
#include <TimeAlarms.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address from Ethernet shield sticker under board

unsigned int localPort = 8888;      // local port to listen for UDP packets

IPAddress ip(10, 222, 1, 250); // 10.222.1.250 is the IP address given by Ames High School Technology Director
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov NTP server
//IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov NTP server
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov NTP server

const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

EthernetUDP Udp; // A UDP instance to let us send and receive packets over UDP

void setup()
{
  Serial.begin(19200);

  Ethernet.begin(mac, ip);  // initialize Ethernet device

  Udp.begin(localPort);

  syncNTP(); //This function syncs the local time with Internet time

  // create the alarms
  Alarm.alarmRepeat(8,30,0, MorningAlarm);  // 8:30am every day
  Alarm.alarmRepeat(0,0,0,syncNTP);         // Syncs the time every day at midnight
}

void  loop(){
  digitalClockDisplay();
  Alarm.delay(1000); // wait one second between clock display
}

// functions to be called when an alarm triggers:
void MorningAlarm(){
  Serial.println("Alarm: - example");
}

// digital clock display of the time
void digitalClockDisplay()
{
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

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer,NTP_PACKET_SIZE);
  Udp.endPacket();
}

void syncNTP() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server

  // wait to see if a reply is available
  delay(1000);

  if ( Udp.parsePacket() ) {
    int hour, minute, second = 0;
   
   // We've received a packet, read the data from it
   Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

   //the timestamp starts at byte 40 of the received packet and is four bytes,
   // or two words, long. First, extract the two words:

   unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
   unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
   // combine the four bytes (two words) into a long integer
   // this is NTP time (seconds since Jan 1 1900):
   unsigned long secsSince1900 = highWord << 16 | lowWord;

   // now convert NTP time into everyday time:
   // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
   const unsigned long seventyYears = 2208988800UL;
   // subtract seventy years:
   unsigned long epoch = secsSince1900 - seventyYears;

   // Find the hour, minute and second:
   hour   = (epoch  % 86400L) / 3600 - 5; // find the hour (86400 equals secs per day)
   minute = (epoch  % 3600) / 60; // find the minute (3600 equals secs per minute)
   second = epoch %60; // find the second
  
   setTime(hour, minute, second, 1, 1, 10); // sets the time for the alarms 
  }
}
