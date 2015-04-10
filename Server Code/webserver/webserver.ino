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

#define REQ_BUF_SZ 60 // size of buffer used to capture HTTP requests
#define NUM_ZONES 7
#define BUFFER_SIZE 64
#define TABLE_SIZE 16

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

//boolean LED_state[4] = {0}; // stores the states of the LEDs
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

//EDB config_DB(&writer, &reader);

// Define all zone structs
// ZoneProperties zone[NUM_ZONES];

void setup()
{
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
        Serial.println("ERROR - Can't find website/overview.html file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found website/overview.html file.");
    
    Serial.println("Opening config.db ...");
    dbFile = SD.open("config.db", FILE_WRITE);
    // how do I check if the table already exists?
//    if(config_DB.count() < 1) {
//        config_DB.create(0, TABLE_SIZE, sizeof(zone_config));
//        Serial.println("SUCCESS - Config database created.");
//        //might need to initialize all zones here
//    } else {
//       Serial.println("Config database already exists."); 
//    }
  
   /* for(int i=0; i < config_DB.count();i++)
    {
//        config_DB.readRec(i,EDB_REC zone_config);
        zone[i] = zone_config;
    }

    for(int i =0; i< config_DB.count(); i++) {
        Serial.println(zone[i].Name);
    }
    
    */
    
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
				// print HTTP request character to serial monitor
                Serial.print(c);
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
                        webFile = SD.open("overview.htm");        // open web page file
                    } else if (StrContains(HTTP_req, "GET /config.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("config.htm");        // open web page file
                    } else if (StrContains(HTTP_req, "GET /zones.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("zones.htm");        // open web page file
                    } else if (StrContains(HTTP_req, "GET /log.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("log.htm");        // open web page file
                    } else if (StrContains(HTTP_req, "GET /users.htm")) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connnection: close");
                        client.println();
                        webFile = SD.open("users.htm");        // open web page file
                    } else if (StrContains(HTTP_req, "GET /zones.png")) {
                        webFile = SD.open("zones.png");
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
    Serial.print("test ");
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    Serial.println((int)len);
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
