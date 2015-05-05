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
IPAddress ip(10, 222, 1, 250); // 10.222.1.250 is the IP address given by Ames High School Technology Director
EthernetServer server(80);  // create a server at port 80
File webFile;
//char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
String HTTP_req;
char req_index = 0;              // index into HTTP_req buffer

unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov NTP server
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
EthernetUDP Udp; // A UDP instance to let us send and receive packets over UDP

char ZoneState[NUM_ZONES] = {'C'}; // stores zone active states
int count = 0; // for buffering packet
byte httpBuffer[BUFFER_SIZE];
File dbFile;
String Http_req_full = "";

int hours, minutes;
int time_check = 0;
char after_noon = 'A';
String time = "";

//TODO: IMPLEMENT LOG FILE AND USER LIST
//char //log_file[LOG_SIZE][LOG_MESSAGE] = {0};
//char email_list[NUM_USERS][30] = {0};

EDB config_DB(&writer, &reader);

// Define all zone structs

struct ZoneProperties
{
  String Name; 
  int Visible; 
  String Begin1; 
  String  End1;
  String Begin2; 
  String  End2;
  String Begin3; 
  String  End3;
} zone_config;

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

    if (!SD.exists("config.db")) {
        Serial.println("ERROR - Can't find config.db file!");
        return;  // can't find index file
    }
    Serial.println("Opening config.db ...");
    dbFile = SD.open("config.db", FILE_WRITE);
    
    // how do I check if the table already exists?
    if(config_DB.count() == 0) {
        config_DB.create(0, TABLE_SIZE, sizeof(zone_config));
        Serial.println("SUCCESS - Config database created.");
        
        for(int i=0; i < NUM_ZONES; i++) {
          ZoneProperties zone;
          zone.Name = "";
          zone.Visible = 1;
          zone.Begin1="";
          zone.End1 = "";
          zone.Begin2="";
          zone.End2="";
          zone.Begin3="";
          zone.End3 = "";
          config_DB.insertRec(i, EDB_REC zone); 
        }
        Serial.println("Initialized all zones in DB.");
    } else {
       Serial.println("Config database already exists. Opening config DB.");   
       config_DB.open(0); 
    }
    
    Serial.println("Reading records into local cache.");
    for(int i=0; i < config_DB.count();i++)
    {
        ZoneProperties zone;
        config_DB.readRec(i, EDB_REC zone);
        zones[i] = zone;
    }


    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Udp.begin(localPort);
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
                        int recno = parseConfig();
                        EDB_Status result = config_DB.updateRec(recno, EDB_REC zones[recno]);
                        printStatus(result);
                        //for testing purposes
                        readRecord(recno);
                        
                    } else if (HTTP_req.indexOf("POST /&setup") > -1) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Recieved config");
                        Serial.println(HTTP_req);
                        client.println("Connection: close");
                        
                      /*  int zone_update = parsed_GET[1].toInt();
                        zone_update = zone_update-1; //CONVERT TO 0 BASED NUMBERING
                        zone[zone_update].Name = parsed_GET[2];
                        zone[zone_update].Visible = parsed_GET[3].toInt();
                        config_DB.updateRec(zone_update, EDB_REC zone[zone_update]); */
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
                    } else if (HTTP_req.indexOf("GET /favicon.ico") > -1) {
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
/*
// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}
*/

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
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

/*
const int zone1 = 23;
const int zone2 = 47;
const int zone3 = 27;
const int zone4 = 29;
const int zone5 = 31;
const int zone6 = 45;
const int zone7 = 25;
*/

void readRecord(int recno) {
    
    ZoneProperties zone;
    config_DB.readRec(recno ,EDB_REC zone);
    
    Serial.println("Name: " + zone.Name);
    Serial.print("Visible: "); Serial.println(zone.Visible);
    Serial.println("Begin1: " + zone.Begin1);  
    Serial.println("End1: " + zone.End1);    
    Serial.println("Begin2: " + zone.Begin2);  
    Serial.println("End2: " + zone.End2);    
    Serial.println("Begin3: " + zone.Begin3);
    Serial.println("End3: " + zone.End3); 
}

int parseConfig() {
    
    int beginIndex = HTTP_req.indexOf("&zone=")+6;
    int endIndex = HTTP_req.indexOf("&b1=");
    int zone_update = HTTP_req.substring(beginIndex, endIndex).toInt() - 1;
    
    
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf("&e1=");
    Serial.println(HTTP_req.substring(beginIndex, endIndex));
    zones[zone_update].Begin1 = HTTP_req.substring(beginIndex, endIndex);
    
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf("&b2=");
    zones[zone_update].End1 = HTTP_req.substring(beginIndex, endIndex);
    Serial.println(zones[zone_update].End1);
    
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf("&e2=");
    Serial.println(HTTP_req.substring(beginIndex, endIndex));
    zones[zone_update].Begin2 = HTTP_req.substring(beginIndex, endIndex);
    
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf("&b3=");
    zones[zone_update].End2 = HTTP_req.substring(beginIndex, endIndex);
    Serial.println(zones[zone_update].End2);
    
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf("&e3=");
    Serial.println(HTTP_req.substring(beginIndex, endIndex));
    zones[zone_update].Begin3 = HTTP_req.substring(beginIndex, endIndex);
    
    beginIndex = endIndex + 4;
    endIndex = HTTP_req.indexOf(" HTTP/1.1");
    zones[zone_update].End3 = HTTP_req.substring(beginIndex, endIndex);
    Serial.println(zones[zone_update].End3);
    
    return zone_update;
}

void printStatus(EDB_Status result) {
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
