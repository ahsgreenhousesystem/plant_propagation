/*--------------------------------------------------------------
Webserver for ISU Senior Design - Ames High Greenhouse System
This code is built using the Arduino compiler and operates
with an Ethernet shield using the WizNet chipset.
--------------------------------------------------------------*/

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <EthernetUdp.h>
//#include <EDB.h>
#include <Time.h>
#include <TimeAlarms.h>

#define REQ_BUF_SIZE 20                                // size of buffer used to capture HTTP requests for Arduino UNO (Joe's)
//#define REQ_BUF_SIZE 60                              // size of buffer used to capture HTTP requests for Ardunio Mega 2560 (in the greenhouse)
//#define NUM_ZONES 7
//#define BUFFER_SIZE 64
//#define TABLE_SIZE 4096

const int zone1 = 23;
const int zone2 = 47;
const int zone3 = 27;
const int zone4 = 29;
const int zone5 = 31;
const int zone6 = 45;
const int zone7 = 25;                                // pin 25 wire is faulty

const int timeDelay = 1000;                          // delay in ms -- important -- relays wear out if driven too fast

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address from Ethernet shield sticker under board
IPAddress ip(192, 168, 0, 2);                        // 10.222.1.250 is the IP address given by Ames High School Technology Director
unsigned int localPort = 80;                         // local port to listen for UDP packets 8888?
EthernetServer server(localPort);                    // create a server at port 80
File webFile;
char HTTP_req[REQ_BUF_SIZE] = {0};                   // buffered HTTP request stored as null terminated string
char req_index = 0;                                  // index into HTTP_req buffer

IPAddress timeServer(132, 163, 4, 101);              // time-a.timefreq.bldrdoc.gov NTP server		
const int NTP_PACKET_SIZE= 48;                       // NTP time stamp is in the first 48 bytes of the message		
byte packetBuffer[NTP_PACKET_SIZE];                  // buffer to hold incoming and outgoing packets		
EthernetUDP Udp;                                     // A UDP instance to let us send and receive packets over UDP

void setup()
{
    // Initialize all zone pins		
    pinMode(zone1, OUTPUT);		
    sprinklerOff(zone1);		
    pinMode(zone2, OUTPUT);		
    sprinklerOff(zone2);		
    pinMode(zone3, OUTPUT);		
    sprinklerOff(zone3);		
    pinMode(zone4, OUTPUT);		
    sprinklerOff(zone4);		
    pinMode(zone5, OUTPUT);		
    sprinklerOff(zone5);		
    pinMode(zone6, OUTPUT);		
    sprinklerOff(zone6);		
    pinMode(zone7, OUTPUT);		
    sprinklerOff(zone7);  
  
    // disable Ethernet chip
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
    Serial.begin(9600); // baud rate for debugging in serial monitor     
    
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return; // init failed
    } else {
        Serial.println("SUCCESS - SD card initialized.");
    }
    
    if (!SD.exists("website/overview.htm")) {
        Serial.println("ERROR - Can't find overview.htm file!");
        return; // can't find the index file
    } else {
      Serial.println("SUCCESS - Found overview.htm file.");
    }
    
    Ethernet.begin(mac, ip);                     // initialize Ethernet device
    server.begin();                             // start to listen for clients
    Udp.begin(localPort);		
    syncNTP();                                  // this function syncs the local time with Internet time		
    digitalClockDisplay();		
    Alarm.alarmRepeat(19,34,0, sprinkler1On);   // this time every day		
    Alarm.alarmRepeat(19,35,0, sprinkler1Off);		
    Alarm.alarmRepeat(0,0,0, syncNTP);           // Syncs the time every day at midnight
}

void loop()
{
    EthernetClient client = server.available();  // try to get client
    if (client) {  
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                if (req_index < (REQ_BUF_SIZE - 1)) {
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;
                }
                Serial.print(c);
                if (c == '\n' && currentLineIsBlank) {
                    if (StrContains(HTTP_req, "GET / ") || StrContains(HTTP_req, "GET /overview.htm")) 
                    {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/overview.htm");        
                    }
                    else if (StrContains(HTTP_req, "GET /config.htm")) 
                    {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/config.htm");        
                    }
                    else if (StrContains(HTTP_req, "GET /zones.htm")) 
                    {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/zones.htm");        
                    }
                    else if (StrContains(HTTP_req, "GET /log.htm")) 
                    {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/log.htm");        
                    }
                    else if (StrContains(HTTP_req, "GET /users.htm")) 
                    {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/users.htm");        
                    }
                    else if (StrContains(HTTP_req, "GET /zones.png")) 
                    {
                        webFile = SD.open("website/zones.png");
                        if (webFile) {
                            client.println("HTTP/1.1 200 OK");
                            client.println();
                        }
                    }
                    if (webFile) {
                        while(webFile.available()) {
                            client.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
                    // reset buffer index and all buffer elements to 0
                    req_index = 0;
                    StrClear(HTTP_req, REQ_BUF_SIZE);
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

		
void sprinklerOff(int pin) {		
//  digitalWrite(pin, HIGH);
  Serial.print("Sprinkler "); Serial.print(pin); Serial.println(" off.");	
  delay(timeDelay);		
}		
void sprinklerOn(int pin) {		
//  digitalWrite(pin, LOW);
  Serial.print("Sprinkler "); Serial.print(pin); Serial.println(" on.");	
  delay(timeDelay);		
}

void sprinkler1On() { sprinklerOn(zone1); }
void sprinkler2On() { sprinklerOn(zone2); }
void sprinkler3On() { sprinklerOn(zone3); }
void sprinkler4On() { sprinklerOn(zone4); }
void sprinkler5On() { sprinklerOn(zone5); }
void sprinkler6On() { sprinklerOn(zone6); }
void sprinkler7On() { sprinklerOn(zone7); }

void sprinkler1Off() { sprinklerOff(zone1); }
void sprinkler2Off() { sprinklerOff(zone2); }
void sprinkler3Off() { sprinklerOff(zone3); }
void sprinkler4Off() { sprinklerOff(zone4); }
void sprinkler5Off() { sprinklerOff(zone5); }
void sprinkler6Off() { sprinklerOff(zone6); }
void sprinkler7Off() { sprinklerOff(zone7); }

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length) {
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

// searches for the string sfind in the string str
// returns 1 if string found, 0 if string not found
char StrContains(char *str, char *sfind) {
    char found = 0;
    char index = 0;
    char len;
    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }
    return 0;
}

// send an NTP request to the time server at the given address		
unsigned long sendNTPpacket(IPAddress& address) {		
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
