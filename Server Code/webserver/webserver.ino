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
unsigned int localPort = 8888; // local port to listen for UDP packets
IPAddress ip(192, 168, 0, 2); // 10.222.1.250 is the IP address given by Ames High School Technology Director
EthernetServer server(localPort); // create a server at port 80
File webFile;
String HTTP_req;
char req_index = 0; // index into HTTP_req buffer

IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
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

typedef struct TimePair {
  time_t start;
  time_t finish;
} t_pair;

typedef struct ZoneProperties
{
  String name;
  boolean active;
  int zone;
  t_pair first;
  t_pair middle;
  t_pair last;
} zone_config;

ZoneProperties zones[NUM_ZONES + 9];

//struct LogEvent {
//   //int ID;
//   int Zone;
//   String Message;
//} logEvent;

void setup()
{
  setupSprinklers();
  checkSDCard();
  setupTimeAlarms();
  setupDatabase();

  // disable Ethernet chip
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  Serial.begin(19200); // for debugging
  Ethernet.begin(mac, ip); // initialize Ethernet device
  Udp.begin(localPort); // start to listen for clients
  server.begin(); // start the server
}

void loop()
{
  EthernetClient client = server.available();  // try to get client
  if (client) {  // got client?
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) { // client data available to read
        char c = client.read(); // read 1 byte (character) from client
        HTTP_req += c;
        Serial.print(c); // print HTTP request character to serial monitor
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n' && currentLineIsBlank) {
          webFile = determineWebPage(HTTP_req, client); // open requested web page file
          if (HTTP_req.indexOf("POST /&setup") > -1) {
            sendOkayStatus(client);
            int zone_update = handleSetupCall();
            config_DB.updateRec(zone_update, EDB_REC zones[zone_update]);
          }
          if (HTTP_req.indexOf("POST /?control") > -1) {
            sendOkayStatus(client);
            String message = parseControl();
            client.println(message);
            //log(message);
          }
          if (HTTP_req.indexOf("POST /?config") > -1) {
            sendOkayStatus(client);
            int recno = handleConfigCall();
            Serial.print("Updating record for Zone "); Serial.println(recno + 1);
            EDB_Status result = config_DB.updateRec(recno, EDB_REC zones[recno]);
            printStatus(result);
            //for testing purposes
            printRecord(recno);
          }
          if (HTTP_req.indexOf("GET /zones.png") > -1) {
            webFile = SD.open("website/zones.png");
            if (webFile) {
              client.println("HTTP/1.1 200 OK");
              client.println();
            }
          }
          if (webFile) {
            while (webFile.available()) {
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
  else {
    Serial.println("Client not available");
  }
}

File determineWebPage(String request, EthernetClient client) {
  File webFile;
  if (request.indexOf("GET / ") > -1 || request.indexOf("GET /overview.htm") > -1) {
    sendOkayStatus(client);
    webFile = SD.open("website/overview.htm");
  } else if (request.indexOf("GET /config.htm") > -1) {
    sendOkayStatus(client);
    webFile = SD.open("website/config.htm");
  } else if (request.indexOf("GET /zones.htm") > -1) {
    sendOkayStatus(client);
    webFile = SD.open("website/zones.htm");
  } else if (request.indexOf("GET /log.htm") > -1) {
    sendOkayStatus(client);
    webFile = SD.open("website/log.htm");
  } else if (request.indexOf("GET /users.htm") > -1) {
    sendOkayStatus(client);
    webFile = SD.open("website/users.htm");
  }
  return webFile;
}

void sendOkayStatus(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connnection: close");
  client.println();
}

String parseControl() {
  // "/?control" + "&zone="+zone.substring(4) + "&action=" + action

  int beginIndex = HTTP_req.indexOf("&zone=") + 6;
  int endIndex = HTTP_req.indexOf("&action=");
  int zone = HTTP_req.substring(beginIndex, endIndex).toInt();

  beginIndex = endIndex + 8;
  endIndex = HTTP_req.indexOf(" HTTP/1.1");
  String action = HTTP_req.substring(beginIndex, endIndex);
  int zonePin = getZonePin(zone);

  String message = "Zone ";
  message += zone;
  if (action.indexOf("Open") > -1) {
    sprinklerOn(zonePin);
    message += " was opened.";
  } else if (action.indexOf("Close") > -1) {
    sprinklerOff(zonePin);
    message += " was closed.";
  } else if (action.indexOf("Auto") > -1) {
    //no implementation for this yet.
    message += " was set to auto.";
  }
  HTTP_req = "";
  return message;
}

int getZonePin(int zone) {
  switch (zone) {
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
