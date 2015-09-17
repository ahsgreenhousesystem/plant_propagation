void checkSDCard() {
  // initialize SD card
  if (!SD.begin(4)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  // check for index.htm file
  if (!SD.exists("website/overview.htm")) {
    Serial.println("ERROR - Can't find website/overview.htm file!");
    return;  // can't find index file
  }
}

void setupDatabase() {
  //       SD.remove("config.db");
  if (SD.exists("config.db")) {
    dbFile = SD.open("config.db");
    config_DB.open(0);
  } else {
    Serial.println("config.db does NOT exist! Creating it...");
    dbFile = SD.open("config.db", FILE_WRITE);
    config_DB.create(0, TABLE_SIZE, sizeof(zone_config));
    Serial.println("SUCCESS: Config database created. Creating records...");
    for (int i = 1; i < NUM_ZONES + 1; i++) {
      ZoneProperties zone;
      zone.name = "";
      zone.zone = i;
      zone.active = false;
      config_DB.appendRec(EDB_REC zone);
      Serial.print("Created record for zone "); Serial.println(i);
    }
    Serial.println("Initialized all zones in DB.");
    Serial.print("Total records: "); Serial.println(config_DB.count());
  }

  Serial.print("Record Count: "); Serial.println(config_DB.count());

  if (config_DB.count() > 0)  {
    Serial.println("Reading records.");

    for (int i = 1; i < config_DB.count() + 1; i++)
    {
      //        ZoneProperties zone;
      //        config_DB.readRec(i, EDB_REC zone);
      //        zones[i-1] = zone;
      //        Serial.print("Cached zone "); Serial.println(zones[i-1].zone);
      printRecord(i); //Testing purposes
    }

  }

  dbFile.close();
}

void printRecord(int recno) {
  Serial.print("Printing record for "); Serial.println(recno);
  // ZoneProperties zone = zones[recno-1];

  ZoneProperties zone;
  EDB_Status result = config_DB.readRec(recno, EDB_REC zone);
  if (result == EDB_OK) {
    Serial.print("Name: "); Serial.println(zone.name);
    Serial.print("Zone: "); Serial.println(zone.zone);
    Serial.print("Active: "); Serial.println(zone.active);
    Serial.print("First: start -  "); Serial.println(zone.first.start);
    Serial.print("First: finish - "); Serial.println(zone.first.finish);
    Serial.print("Middle: start - "); Serial.println(zone.middle.start);
    Serial.print("Middle: finish - "); Serial.println(zone.middle.finish);
    Serial.print("Last: start - "); Serial.println(zone.last.start);
    Serial.print("Last: finish - "); Serial.println(zone.last.finish);
    Serial.println("Finished printing record.");
  } else
  {
    Serial.print("Error reading record "); Serial.println(recno);
  }
}

struct ZoneProperties getRecord(int recno)
{
  ZoneProperties zone;
  config_DB.readRec(recno, EDB_REC zone);
  return zone;
}

int handleSetupCall()
{
  //$.post("/?setup" + "&zone="+zone+"&name="+name+"&active="+active,
  int beginIndex = HTTP_req.indexOf("&zone=") + 6;
  int endIndex = HTTP_req.indexOf("&name=");
  int zone_update = HTTP_req.substring(beginIndex, endIndex).toInt() - 1;

  //grab name
  beginIndex = endIndex + 6;
  endIndex = HTTP_req.indexOf("&active=");
  Serial.println(HTTP_req.substring(beginIndex, endIndex));
  zones[zone_update].name = HTTP_req.substring(beginIndex, endIndex);

  //grab active status
  beginIndex = endIndex + 4;
  endIndex = HTTP_req.indexOf(" HTTP/1.1");
  String active = HTTP_req.substring(beginIndex, endIndex);
  if (active.equals("true"))
  {
    zones[zone_update].active = true;
  }
  else {
    zones[zone_update].active = false;
  }
  Serial.println(HTTP_req.substring(beginIndex, endIndex));

  return zone_update;
}

int handleConfigCall() {
  //grab zone that is to be updated
  int beginIndex = HTTP_req.indexOf("&zone=") + 6;
  int endIndex = HTTP_req.indexOf("&b1=");
  int zone_update = HTTP_req.substring(beginIndex, endIndex).toInt() - 1;

  //grab first.start time
  beginIndex = endIndex + 4;
  endIndex = HTTP_req.indexOf("&e1=");
  Serial.println(HTTP_req.substring(beginIndex, endIndex));
  zones[zone_update].first.start = parseTime(HTTP_req.substring(beginIndex, endIndex));

  //grab first.finish time
  beginIndex = endIndex + 4;
  endIndex = HTTP_req.indexOf("&b2=");
  zones[zone_update].first.finish = parseTime(HTTP_req.substring(beginIndex, endIndex));
  // Serial.println(zones[zone_update].End1);

  //grab middle.start time
  beginIndex = endIndex + 4;
  endIndex = HTTP_req.indexOf("&e2=");
  Serial.println(HTTP_req.substring(beginIndex, endIndex));
  zones[zone_update].middle.start = parseTime(HTTP_req.substring(beginIndex, endIndex));

  //grab middle.finish time
  beginIndex = endIndex + 4;
  endIndex = HTTP_req.indexOf("&b3=");
  zones[zone_update].middle.finish = parseTime(HTTP_req.substring(beginIndex, endIndex));
  //   Serial.println(zones[zone_update].End2);

  //grab last.start time
  beginIndex = endIndex + 4;
  endIndex = HTTP_req.indexOf("&e3=");
  // Serial.println(HTTP_req.substring(beginIndex, endIndex));
  zones[zone_update].last.start = parseTime(HTTP_req.substring(beginIndex, endIndex));

  //grab last.finish time
  beginIndex = endIndex + 4;
  endIndex = HTTP_req.indexOf(" HTTP/1.1");
  zones[zone_update].last.finish = parseTime(HTTP_req.substring(beginIndex, endIndex));
  // Serial.println(zones[zone_update].End3);

  return zone_update;
}

void printStatus(EDB_Status result) {
  if (result != EDB_OK) {
    switch (result)
    {
      case EDB_OUT_OF_RANGE:
        Serial.println("Database Status ERROR: Recno out of range!");
        break;
      case EDB_TABLE_FULL:
        Serial.println("Database Status ERROR: Table full!");
        break;
      default:
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

/* void log(String message, int Zone) {

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

*/

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
