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
#define TABLE_SIZE 16

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
char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
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
String Time1; 
int  duration1;
String Time2; 
int  duration2;
String Time3; 
int  duration3;
} zone_config;

ZoneProperties zone[NUM_ZONES];

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
    if(config_DB.count() < 1) {
        config_DB.create(0, TABLE_SIZE, sizeof(zone_config));
        Serial.println("SUCCESS - Config database created.");
        //might need to initialize all zones here
    } else {
       Serial.println("Config database already exists.");
    }

    for(int i=0; i < config_DB.count();i++)
    {
        config_DB.readRec(i,EDB_REC zone_config);
        zone[i] = zone_config;
    }

    for(int i =0; i< config_DB.count(); i++) {
        Serial.println(zone[i].Name);
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
                if (req_index < (REQ_BUF_SZ - 1)) {
                    Http_req_full += c;
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;

                }
                Serial.print(c); // print HTTP request character to serial monitor
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // open requested web page file
                    if (StrContains(HTTP_req, "GET / ")
                                 || StrContains(HTTP_req, "GET /overview.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/overview.htm");        // open overview page
                    } else if (StrContains(HTTP_req, "GET /config.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();                  
                        webFile = SD.open("website/config.htm");        // open web page file
                    } else if(StrContains(HTTP_req, "POST /?config")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Recieved config");
                        Serial.println(HTTP_req);
                        client.println("Connnection: close");
                       /* int zone_update = parsed_GET[1].toInt();
                        zone_update = zone_update-1; //CONVERT TO 0 BASED NUMBERING
                        zone[zone_update].Time1 = parsed_GET[2];
                        zone[zone_update].duration1 = parsed_GET[3].toInt();
                        zone[zone_update].Time2 = parsed_GET[4];
                        zone[zone_update].duration2 = parsed_GET[5].toInt();
                        zone[zone_update].Time3 = parsed_GET[6];
                        zone[zone_update].duration3 = parsed_GET[7].toInt();
                        config_DB.updateRec(zone_update, EDB_REC zone[zone_update]); */
                        client.println();
                        //webFile = SD.open("website/config.htm");        // open config page
                    } else if (StrContains(HTTP_req, "POST /&setup")) {
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
                    } else if (StrContains(HTTP_req, "GET /zones.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/zones.htm");        // open zones page
                    } else if (StrContains(HTTP_req, "GET /log.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/log.htm");        // open log page
                    } else if (StrContains(HTTP_req, "GET /users.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("website/users.htm");        // open users page
                    } else if (StrContains(HTTP_req, "POST /?zone1=Open")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOn(zone1);
                        Serial.print("Zone 1 was opened.");
                        log("Zone 1 was opened.", 1);
                        webFile = SD.open("website/overview.htm");        // open web page file
                    } else if (StrContains(HTTP_req, "POST /?zone1=Close")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOff(zone1);
                        Serial.print("Zone 1 was closed.");
                        log("Zone 1 was closed.", 1);
                        webFile = SD.open("website/overview.htm");        // open web page file
                    } else if (StrContains(HTTP_req, "POST /?zone1=Auto")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        Serial.print("Zone 1 was set to auto.");
                        log("Zone 1 was set to auto.", 1);
                        webFile = SD.open("website/overview.htm");        // open web page file
                    } else if (StrContains(HTTP_req, "POST /?zone2=Open")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOn(zone2);
                        Serial.print("Zone 2 was opened.");
                        log("Zone 2 was opened.", 2);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 2 was opened.");
                    } else if (StrContains(HTTP_req, "POST /?zone2=Close")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOff(zone2);
                        Serial.print("Zone 2 was closed.");
                        log("Zone 2 was closed.", 2);
                        webFile = SD.open("website/overview.htm");        // open web page file
                    } else if (StrContains(HTTP_req, "GET /?zone2=Auto")) {
                        client.println("Zone 2 was closed.");
                    } else if (StrContains(HTTP_req, "POST /?zone2=Auto")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        Serial.print("Zone 2 was set to auto.");
                        log("Zone 2 was set to auto.", 2);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 2 was set to auto.");
                    } else if (StrContains(HTTP_req, "POST /?zone3=Open")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOn(zone3);
                        Serial.print("Zone 3 was opened.");
                        log("Zone 3 was opened.", 3);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 3 was opened.");
                    } else if (StrContains(HTTP_req, "POST /?zone3=Close")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOff(zone3);
                        Serial.print("Zone 3 was closed.");
                        log("Zone 3 was closed.", 3);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 3 was closed.");
                    } else if (StrContains(HTTP_req, "POST /?zone3=Auto")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        Serial.print("Zone 3 was set to auto.");
                        log("Zone 3 was set to auto.", 3);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 3 was set to auto.");
                    } else if (StrContains(HTTP_req, "POST /?zone4=Open")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOn(zone4);
                        Serial.print("Zone 4 was opened.");
                        log("Zone 4 was opened.", 4);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 4 was opened.");
                    } else if (StrContains(HTTP_req, "POST /?zone4=Close")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOff(zone4);
                        log("Zone 4 was closed.", 4);
                        Serial.print("Zone 4 was closed.");
                        client.println("Zone 4 was closed.");
                    } else if (StrContains(HTTP_req, "POST /?zone4=Auto")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        Serial.print("Zone 4 was set to auto.");
                        log("Zone 4 was set to auto.", 4);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 4 was set to auto.");
                    } else if (StrContains(HTTP_req, "POST /?zone5=Open")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOn(zone5);
                        Serial.print("Zone 5 was opened.");
                        log("Zone 5 was opened.", 5);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 5 was opened.");
                    } else if (StrContains(HTTP_req, "POST /?zone5=Close")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOff(zone5);
                        Serial.print("Zone 5 was closed.");
                        log("Zone 5 was closed.", 5);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 5 was closed.");
                    } else if (StrContains(HTTP_req, "POST /?zone5=Auto")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        Serial.print("Zone 5 was set to auto.");
                        log("Zone 5 was set to auto.", 5);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 5 was set to auto.");
                    } else if (StrContains(HTTP_req, "POST /?zone6=Open")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOn(zone6);
                        Serial.print("Zone 6 was opened.");
                        log("Zone 6 was opened.", 6);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 6 was opened.");
                    } else if (StrContains(HTTP_req, "POST /?zone6=Close")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOff(zone6);
                        Serial.print("Zone 6 was closed.");
                        log("Zone 6 was closed.", 6);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 6 was closed.");
                    } else if (StrContains(HTTP_req, "POST /?zone6=Auto")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        Serial.print("Zone 6 was set to auto.");
                        log("Zone 6 was set to auto.", 6);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 6 was set to auto.");
                    } else if (StrContains(HTTP_req, "POST /?zone7=Open")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOn(zone7);
                        Serial.print("Zone 7 was opened.");
                        log("Zone 7 was opened.", 7);
                        webFile = SD.open("website/overview.htm");        // open web page file
                    } else if (StrContains(HTTP_req, "POST /?zone7=Close")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        sprinklerOff(zone7);
                        Serial.print("Zone 7 was closed.");
                        log("Zone 7 was closed.", 7);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 7 was closed.");
                    } else if (StrContains(HTTP_req, "POST /?zone7=Auto")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        Serial.print("Zone 7 was set to auto.");
                        log("Zone 7 was set to auto.", 7);
                        webFile = SD.open("website/overview.htm");        // open web page file
                        client.println("Zone 7 was set to auto.");
                    } else if (StrContains(HTTP_req, "GET /zones.png")) {
                        webFile = SD.open("website/zones.png");
                        if (webFile) {
                            client.println("HTTP/1.1 200 OK");
                            client.println();
                        }
                    } else if (StrContains(HTTP_req, "GET /favicon.ico")) {
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
                    StrClear(HTTP_req, REQ_BUF_SZ);
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

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

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
