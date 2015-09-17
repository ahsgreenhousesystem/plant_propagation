void setupTimeAlarms() {
  syncNTP(); // This function syncs the local time with Internet time
  digitalClockDisplay();
  Alarm.alarmRepeat(0, 0, 0, syncNTP); // Syncs the time every day at midnight
}

/* Something to think about : What happens after we set an alarm for a certain time and
    then the user changes that configuration.  How do we 'clear' old alarms so they don't
    keep happening?

     * Possibilites:
           - Storing current alarms and verifying at a certain point ?
           - Other suggestions?
*/
void setWaterAlarm(int zone, int setting)
{
  time_t start;
  time_t finish;

  switch (setting) {
    case 1:
      start = zones[zone - 1].first.start;
      finish = zones[zone - 1].first.finish;
      break;
    case 2:
      start = zones[zone - 1].middle.start;
      finish = zones[zone - 1].middle.finish;
      break;
    case 3:
      start = zones[zone - 1].last.start;
      finish = zones[zone - 1].last.finish;
      break;
  }

  setAlarm(zone, hour(start), minute(start), true);
  setAlarm(zone, hour(finish), minute(finish), false);
}

void setAlarm(int zone, int hr, int mins, boolean water) {
  switch (zone) {
    case 1: if (water) Alarm.alarmRepeat(hr, mins, 0, sprinkler1On);
      else Alarm.alarmRepeat(hr, mins, 0, sprinkler1Off);
      break;
      /* continue cases.
         .
         .
         .
        */
  }
}

time_t parseTime(String in) {
  //String form: 10:30 PM or 10:30 AM or 10:30%20PM - TESTING NEEDED, Assuming first scenario
  tmElements_t tm;
  tm.Second = 0;

  int beginIndex = 0;
  int endIndex = in.indexOf(":");
  int hrs = in.substring(beginIndex, endIndex).toInt();

  //this area is questionable.
  beginIndex = endIndex + 1;
  endIndex = in.indexOf(" ");
  int mins = in.substring(beginIndex, endIndex).toInt();

  boolean evening = false;
  beginIndex = endIndex + 1;
  endIndex = in.length();
  String meridiem = in.substring(beginIndex, endIndex);
  if (meridiem.equals("PM")) {
    evening = true;
  }

  if (evening && hrs != 12) {
    hrs += 12;
  } else if (hrs == 12) {
    hrs = 0;
  }

  tm.Hour = hrs;
  tm.Minute = mins;

  return makeTime(tm);
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
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void syncNTP() {
  sendNTPpacket(timeServer); // send an NTP packet to a time server

  // wait to see if a reply is available
  delay(1000);

  if ( Udp.parsePacket() ) {
    int hour, minute, second = 0;

    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

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
    second = epoch % 60; // find the second

    setTime(hour, minute, second, 1, 1, 10); // sets the time for the alarms
  }
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
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
