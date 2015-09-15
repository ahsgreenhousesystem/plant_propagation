/*--------------------------------------------------------------

Webserver for ISU Senior Design - Ames High Greenhouse System

This code is built using the Arduino compiler and operates
with an Ethernet shield using the WizNet chipset.
--------------------------------------------------------------*/

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <EthernetUdp.h>
#include <EDB.h>
#include <Time.h>
#include <TimeAlarms.h>

#define REQ_BUF_SZ 60 // size of buffer used to capture HTTP requests
#define NUM_ZONES 7
#define BUFFER_SIZE 64
#define TABLE_SIZE 4096

const int zone1 = 23;
const int zone2 = 47;
const int zone3 = 27;
const int zone4 = 29;
const int zone5 = 31;
const int zone6 = 45;
const int zone7 = 25; // pin 25 wire is faulty

const int timeDelay = 1000; // delay in ms -- important -- relays wear out if driven too fast

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address from Ethernet shield sticker under board
unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress ip(192, 168, 0, 1); // 10.222.1.250 is the IP address given by Ames High School Technology Director
EthernetServer server(localPort);  // create a server at port 80
File webFile;
//char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
String HTTP_req;
char req_index = 0;              // index into HTTP_req buffer

IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov NTP server
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
EthernetUDP Udp; // A UDP instance to let us send and receive packets over UDP

char ZoneState[NUM_ZONES] = {'C'}; // stores zone active states
int count = 0; // for buffering packet
byte httpBuffer[BUFFER_SIZE];
File dbFile;
String Http_req_full = "";

//TODO: IMPLEMENT LOG FILE AND USER LIST
//char //log_file[LOG_SIZE][LOG_MESSAGE] = {0};
//char email_list[NUM_USERS][30] = {0};

EDB config_DB(&writer, &reader);

// Define all zone structs

struct ZoneProperties
{
  String Name; 
  boolean Active; 
  int Zone;
  String Begin1; 
  String  End1;
  String Begin2; 
  String  End2;
  String Begin3; 
  String  End3;
} zone_config;

struct LogEvent {
   //int ID;
   int Zone;
   String Message;  
} logEvent;

ZoneProperties zones[NUM_ZONES];

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

    Serial.begin(19200);       // for debugging

    // initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("website/overview.htm")) {
        Serial.println("ERROR - Can't find website/overview.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found website/overview.htm file.");
    
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Udp.begin(localPort);
    syncNTP(); //This function syncs the local time with Internet time
    digitalClockDisplay();
    Alarm.alarmRepeat(17,22,0, sprinkler1On);  // 8:30am every day
    Alarm.alarmRepeat(17,25,0, sprinkler1Off);
    Alarm.alarmRepeat(0,0,0,syncNTP);         // Syncs the time every day at midnight

    if (SD.exists("config.db")) {
      Serial.println("Opening config.db ...");
      dbFile = SD.open("config.db");
      config_DB.open(0);
    } else {
        Serial.println("config.db does NOT exist! Creating it...");
        dbFile = SD.open("config.db", FILE_WRITE);
        config_DB.create(0, TABLE_SIZE, sizeof(zone_config));
        Serial.println("SUCCESS - Config database created."); 
        Serial.println("Creating records...");
        for(int i=1; i < NUM_ZONES + 1; i++) {
          ZoneProperties zone;
          zone.Name = "";
          zone.Zone = i;
          zone.Active = false;
          zone.Begin1="";
          zone.End1 = "";
          zone.Begin2="";
          zone.End2="";
          zone.Begin3="";
          zone.End3 = "";
          config_DB.appendRec(EDB_REC zone); 
        }
        Serial.println("Initialized all zones in DB.");
        Serial.print("Total records: "); Serial.println(config_DB.count());
    }
    
    Serial.println("Reading records into local cache.");
    for(int i=1; i < config_DB.count() + 1; i++)
    {
        ZoneProperties zone;
        config_DB.readRec(i, EDB_REC zone);
        zones[i] = zone;
        Serial.println(zones[i].Zone);
        printRecord(i); //Testing purposes
    }
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // limit the size of the stored received HTTP request
                // buffer first part of HTTP request in HTTP_req array (string)
                // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
                HTTP_req += c;
              /*  if (req_index < (REQ_BUF_SZ - 1)) {
                    Http_req_full += c;
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;

                } */
                Serial.print(c); // print HTTP request character to serial monitor
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // open requested web page file
                    if (HTTP_req.indexOf("GET / ") > -1
                                 || HTTP_req.indexOf("GET /overview.htm") > -1) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/overview.htm");        // open overview page
                    } else if (HTTP_req.indexOf("GET /config.htm") > -1) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();                  
                        webFile = SD.open("website/config.htm");        // open web page file
                    } else if(HTTP_req.indexOf("POST /?config") > -1) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Recieved config");
                        Serial.println("Recieved config post");
                        client.println("Connnection: close");          
                        Serial.println(HTTP_req);
                        int recno = handleConfigCall();
                        Serial.print("Updating record for Zone "); Serial.println(recno + 1);
                        EDB_Status result = config_DB.updateRec(recno, EDB_REC zones[recno]);
                        printStatus(result);
                        //for testing purposes
                        printRecord(recno);                       
                    } else if (HTTP_req.indexOf("POST /&setup") > -1) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");            
                        client.println("Connection: close");          
                        int zone_update = handleSetupCall();
                        config_DB.updateRec(zone_update, EDB_REC zones[zone_update]);
                    } else if (HTTP_req.indexOf("GET /zones.htm") > -1) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/zones.htm"); 
                           // open zones page
                   } else if (HTTP_req.indexOf("GET /log.htm") > -1) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/log.htm");        // open log page
                    } else if (HTTP_req.indexOf("GET /users.htm") > -1) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/users.htm");        // open users page
                    } else if (HTTP_req.indexOf("POST /?control") > -1) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        String message = parseControl();
                        client.println(message);
                        //log(message);
                    } else if (HTTP_req.indexOf("GET /zones.png") > -1) {
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
                    HTTP_req = "";
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
        Serial.println("WARNING - Client disconnected.");
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

void log(String message, int Zone) {
     
    char logs[] = {'l', 'o','g','s',Zone,'.','h'};
  
        if(!SD.exists(logs))
        {
            Serial.println("Creating a new log file for zone " + Zone);
        }        
        File logFile = SD.open(logs, FILE_WRITE);
          
        //need to install time library 
       if(logFile) {
        // logFile.println(now() + ": " + message);
         logFile.close();
       }
 
}

/*String readLine(int Zone) {
   char logs[] = {'l', 'o','g','s',Zone,'.','h'};
    if(!SD.exists(logs)) {
        Serial.println("Could not find log file.");
        return "Could not find log file.";
    }
    
    File logFile = SD.open(logs);
    
    String line = "Could not read line in log file.";
    if(logFile.available()) {
       // line = logFile.read();
    }
    
    return line;
}*/

/*void readLog(int Zone) {
  
    //change this to return a large string to display whole log?
    char logs[] = {'l', 'o','g','s',Zone,'.','h'};
    if(!SD.exists(logs)) {
        Serial.println("Could not find log file.");
        return;
    }
    
    File logFile = SD.open("logs" + Zone + ".h");
    
    
    while(logFile.available()) {
        Serial.println(logFile.read());
    }
  
}*/

//void logMessage(int zone, String message)
//{
//    LogEvent l;
//    l.Zone = zone;
//    l.Message = message;
//    Serial.println(message);
//    
//    //store in DB table or log files? 
//     EDB_Status result = log_DB.insertRec(EDB_REC l);
//     printStatus(result);
//    
//    
//}

void setWaterAlarm(int zone)
{
   //grab time and set alarm(s)
   String time1 = zones[zone-1].Begin1;
   String time2 = zones[zone-1].End2;
   int hour;
   int minutes;
   if(time1) {
     int beginIndex = 0;
     int endIndex = time1.indexOf(":");
     hour = time1.substring(beginIndex, endIndex).toInt();
   }
   
    
}

String parseControl() {
  // "/?control" + "&zone="+zone.substring(4) + "&action=" + action 
  
    int beginIndex = HTTP_req.indexOf("&zone=")+6;
    int endIndex = HTTP_req.indexOf("&action=");
    int zone = HTTP_req.substring(beginIndex, endIndex).toInt();
    
    beginIndex = endIndex + 8;
    endIndex = HTTP_req.indexOf(" HTTP/1.1");
    String action = HTTP_req.substring(beginIndex, endIndex);
    int zonePin = getZonePin(zone);
    
    String message = "Zone ";
    message += zone;
    if(action.indexOf("Open") > -1) {
      sprinklerOn(zonePin);
      message += " was opened.";
    } else if(action.indexOf("Close") > -1) {
      sprinklerOff(zonePin);
      message += " was closed.";
    } else if(action.indexOf("Auto") > -1) {
      //no implementation for this yet.
      message += " was set to auto.";
    } 
    HTTP_req = "";
    return message;   
}

int getZonePin(int zone) {
   switch(zone) {
     case 1 : return zone1;
     case 2 : return zone2;
     case 3 : return zone3;
     case 4 : return zone4;
     case 5 : return zone5;
     case 6 : return zone6;
     case 7 : return zone7;
     default: return -1;
   } 
}

void printRecord(int recno) {  
    Serial.print("Printing record for "); Serial.println(recno);
   // ZoneProperties zone = zones[recno-1];
    
    ZoneProperties zone;
    config_DB.readRec(recno, EDB_REC zone);
    
    Serial.print("Name: "); Serial.println(zone.Name);
    Serial.print("Zone: "); Serial.println(zone.Zone);
    Serial.print("Active: "); Serial.println(zone.Active);
    Serial.print("Begin1: "); Serial.println(zone.Begin1);  
    Serial.print("End1: "); Serial.println(zone.End1);    
    Serial.print("Begin2: "); Serial.println(zone.Begin2);  
    Serial.print("End2: "); Serial.println(zone.End2);    
    Serial.print("Begin3: "); Serial.println(zone.Begin3);
    Serial.print("End3: "); Serial.println(zone.End3);     
    Serial.println("Finished printing record.");
}

int handleSetupCall() 
{
    //$.post("/?setup" + "&zone="+zone+"&name="+name+"&active="+active,
    int beginIndex = HTTP_req.indexOf("&zone=")+6;
    int endIndex = HTTP_req.indexOf("&name=");
    int zone_update = HTTP_req.substring(beginIndex, endIndex).toInt() - 1;
    
    //grab name
    beginIndex = endIndex + 6;
    endIndex = HTTP_req.indexOf("&active=");
    Serial.println(HTTP_req.substring(beginIndex, endIndex));
    zones[zone_update].Name = HTTP_req.substring(beginIndex, endIndex);
    
    //grab active status
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf(" HTTP/1.1");
    String active = HTTP_req.substring(beginIndex, endIndex);
    if(active.equals("true"))
    {
      zones[zone_update].Active = true;
    }
    else {
      zones[zone_update].Active = false;
    }
    Serial.println(HTTP_req.substring(beginIndex, endIndex));
    
    return zone_update;
    
}

int handleConfigCall() {    
    //grab zone that is to be updated
    int beginIndex = HTTP_req.indexOf("&zone=")+6;
    int endIndex = HTTP_req.indexOf("&b1=");
    int zone_update = HTTP_req.substring(beginIndex, endIndex).toInt() - 1;
    
    //grab Begin1 time
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf("&e1=");
    Serial.println(HTTP_req.substring(beginIndex, endIndex));
    zones[zone_update].Begin1 = HTTP_req.substring(beginIndex, endIndex);
    
    //grab End1 time
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf("&b2=");
    zones[zone_update].End1 = HTTP_req.substring(beginIndex, endIndex);
    Serial.println(zones[zone_update].End1);
    
    //grab Begin2 time
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf("&e2=");
    Serial.println(HTTP_req.substring(beginIndex, endIndex));
    zones[zone_update].Begin2 = HTTP_req.substring(beginIndex, endIndex);
    
    //grab End2 time
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf("&b3=");
    zones[zone_update].End2 = HTTP_req.substring(beginIndex, endIndex);
    Serial.println(zones[zone_update].End2);
    
    //grab Begin3 time
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf("&e3=");
    Serial.println(HTTP_req.substring(beginIndex, endIndex));
    zones[zone_update].Begin3 = HTTP_req.substring(beginIndex, endIndex);
    
    //grab End3 time
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf(" HTTP/1.1");
    zones[zone_update].End3 = HTTP_req.substring(beginIndex, endIndex);
    Serial.println(zones[zone_update].End3);
    
    return zone_update;
}

void printStatus(EDB_Status result) {
    Serial.println("Printing database status.");
    if (result != EDB_OK) {
      Serial.print("ERROR: ");
      switch (result)
      {
        case EDB_OUT_OF_RANGE:
          Serial.println("Recno out of range");
          break;
        case EDB_TABLE_FULL:
          Serial.println("Table full");
          break;
        default:
          Serial.println("OK.");
          break;
      } 
  } 
}
void writer(unsigned long address, byte data)
{
  dbFile.seek(address);
  dbFile.write(data);
  dbFile.flush();
}

byte reader(unsigned long address)
{
  dbFile.seek(address);
  return dbFile.read();
}

void sprinklerOff(int pin) {
  digitalWrite(pin, HIGH);
  delay(timeDelay);
}

void sprinklerOn(int pin) {
  digitalWrite(pin, LOW);
  delay(timeDelay);
}

void sprinkler1On() {
  sprinklerOn(zone1);
}

void sprinkler1Off() {
  sprinklerOff(zone1);
}

void sprinkler2On() {
  Serial.println("sprinkler2On");
//  sprinklerOn(zone2);
}

void sprinkler2Off() {
  sprinklerOff(zone2);
}

void sprinkler3On() {
  sprinklerOn(zone3);
}

void sprinkler3Off() {
  sprinklerOff(zone3);
}

void sprinkler4On() {
  sprinklerOn(zone4);
}

void sprinkler4Off() {
  sprinklerOff(zone4);
}

void sprinkler5On() {
  sprinklerOn(zone5);
}

void sprinkler5Off() {
  sprinklerOff(zone5);
}

void sprinkler6On() {
  sprinklerOn(zone6);
}

void sprinkler6Off() {
  sprinklerOff(zone6);
}

void sprinkler7On() {
  sprinklerOn(zone7);
}

void sprinkler7Off() {
  sprinklerOff(zone7);
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
