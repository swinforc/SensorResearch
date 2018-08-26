/*
 * The purpose of this code is to provide a clean and easily readable prgram for collecting both temperature and hunidity data from 
 * sensors. This data will eventually be transmitted to a ground station computer, but for now the data needed to be saved on an SD card.
 * 
 * This copy of the rewrite is uncommented for better readabillity of the executable lines of code
 * 
 * Author: Christopher Swinford
 * Date: May 29, 2018
 * Version: 1.0.1
 */

#include <SD.h>
#include <SPI.h>
#include <SoftwareSerial.h>

SoftwareSerial GPSModule(3, 1); //Rx, Tx //Sets up a software serial object
int pos;
int stringplace = 0;
const int ledPin = 13;
String nmea[15];

void setup() {
  Serial.begin(9600); 
  GPSModule.begin(9600);
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)){ 
    Serial.println("initialization failed!");
    return;
    }
    
  Serial.println("initialization done.");
  File logFile = SD.open("LOG.csv", FILE_WRITE);
  String header = "Time, Latitude, Hemisphere, Longitude, Hemishpere, Fix, Altitude(MSL/M), Altitude(WGS84/M), Temperature, Humidity";
   
  if (logFile){
    logFile.println(", , ,");
    logFile.println(header);
    logFile.close();
    Serial.println(header);
    pinMode(ledPin, OUTPUT);
    pinMode(7, INPUT);
  }
}
  
void loop(){  
  GPSModule.flush();
  
  while (GPSModule.available() > 0){ 
    GPSModule.read();
  }
  
  if (GPSModule.find("$GPGGA,")){ 
      String tempMsg = GPSModule.readStringUntil('\n');
      for (int i = 0; i < tempMsg.length(); i++){
        if (tempMsg.substring(i, i + 1) == ","){ 
          nmea[pos] = tempMsg.substring(stringplace, i); 
          stringplace = i + 1;
          pos++;
          }

        if (i == tempMsg.length() - 1){
          nmea[pos] = tempMsg.substring(stringplace, i); 
          }
      }   
    stringplace = 0;
    pos = 0;
    
    int tempvalue;
    int ftemp;
    tempvalue=analogRead(A2);
    float Voltage = tempvalue*((5.0/1023)*10);
    float temp = (-40+(Voltage*10));
    ftemp = ((temp*1.8)+32);
    
    int humvalue; 
    int htemp;
    humvalue=analogRead(A1);
    float HVoltage = humvalue*((5.0/1023)*100);

    String dataString = String(nmea[0]) + ", " + String(nmea[1]) + ", " + String(nmea[2]) + ", " + String(nmea[3]) + ", " + String(nmea[4]) + ", " + String(nmea[5]) + ", " + String(nmea[8]) + ", " + String(nmea[10]) + ", " + String(temp) + ", " + String(HVoltage);

 
    File logFile = SD.open("LOG.csv", FILE_WRITE); 
    if (logFile){
        logFile.println(dataString);
        logFile.close();
      }
    Serial.println(dataString);
    delay(500);
  }
}
