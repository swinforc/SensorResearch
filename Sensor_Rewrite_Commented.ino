/*
 * The purpose of this code is to provide a clean and easily readable prgram for collecting both temperature and hunidity data from 
 * sensors. This data will eventually be transmitted to a ground station computer, but for now the data needed to be saved on an SD card.
 * 
 * Author: Christopher Swinford
 * Date: May 29, 2018
 * Version: 1.0.0
 */

#include <SD.h> //Allows for reading and writing to SD cards 
#include <SPI.h> //Serial data protocol used by microcontrollers for communicating with one or more peripheral devices quickly over short distances.
#include <SoftwareSerial.h> //Allow serial communication on other digital pins of the Arduino, using software to replicate the functionality

//Initialization of variables
SoftwareSerial GPSModule(3, 1); //Rx, Tx //Sets up a software serial object
int pos;
int stringplace = 0;
const int ledPin = 13;
String nmea[15];

void setup() {
  Serial.begin(9600); //Sets the data rate in bits per second for serial data transmission. Rate = 9600 bits per second
  GPSModule.begin(9600); //Sets the speed (baud rate) for the serial communication.
  Serial.print("Initializing SD card..."); //Prints to computer screen

  //If the SD card is not initialized for pin 4 show error 
  if (!SD.begin(4)){ 
    Serial.println("initialization failed!"); //Print failed message to the computer
    return;
    }
    
  Serial.println("initialization done."); //Print success message to the computer
  File logFile = SD.open("LOG.csv", FILE_WRITE); //Opens a file on the SD card.
  String header = "Time, Latitude, Hemisphere, Longitude, Hemishpere, Fix, Altitude(MSL/M), Altitude(WGS84/M), Temperature, Humidity"; //Define the string for the header of the file
  
  //If the file exisits print info to both serial monitor and file 
  if (logFile){
    logFile.println(", , ,");
    logFile.println(header); //Print the header on the first line of the document
    logFile.close(); //Close the file on the SD card
    Serial.println(header);  //Prints the header to the computer monitor
    pinMode(ledPin, OUTPUT); //Set the mode of the ledPin to output
    pinMode(7, INPUT); //Set the mode of pin 7 to input
  }
}
  
void loop(){  
  GPSModule.flush(); //Waits for the transmission of outgoing serial data to complete
  
  //Get the number of bytes (characters) available for reading from the serial port.
  while (GPSModule.available() > 0){ 
    GPSModule.read(); //Reads incoming serial data
  }
  
//GPS gathering time
//Reads data from the serial buffer until the target string of given length is found
  if (GPSModule.find("$GPGGA,")){ 
      String tempMsg = GPSModule.readStringUntil('\n'); //Reads until new line
      for (int i = 0; i < tempMsg.length(); i++){
        //Looks for an instance of a particular substring within a given String from the first parameter to the second i to i + 1
        if (tempMsg.substring(i, i + 1) == ","){ 
          nmea[pos] = tempMsg.substring(stringplace, i); ////Looks for an instance of a particular substring within a given String 
          stringplace = i + 1; //Incriment the stringplace by 1
          pos++; //incriment the position by 1
          }

        if (i == tempMsg.length() - 1){
          nmea[pos] = tempMsg.substring(stringplace, i); 
          }
      }   
    stringplace = 0; //Reset string position to 0
    pos = 0; //Reset position to 0
    
//Temperature Reading
    int tempvalue; //Create an integer variable for the temperature value
    int ftemp;
    tempvalue=analogRead(A2); //Read the analog port 2 and store the value in tempval
    //Conversion for temperature based on the voltage from sensor
    float Voltage = tempvalue*((5.0/1023)*10);
    float temp = (-40+(Voltage*10));
    ftemp = ((temp*1.8)+32);
    
//Humidity Reading
    int humvalue; //Create an integer variable for temperature
    int htemp;
    humvalue=analogRead(A1); //Read the analog port 1 and store the value in val
    float HVoltage = humvalue*((5.0/1023)*100); //Conversion for humidity based on the voltage from sensor

//Create data string for storing to SD card
//CSV format
    String dataString = String(nmea[0]) + ", " + String(nmea[1]) + ", " + String(nmea[2]) + ", " + String(nmea[3]) + ", " + String(nmea[4]) + ", " + String(nmea[5]) + ", " + String(nmea[8]) + ", " + String(nmea[10]) + ", " + String(temp) + ", " + String(HVoltage);

//Open a file to write to 
    File logFile = SD.open("LOG.csv", FILE_WRITE); //Opens a file to write to
    //If the file exisits do the following
    if (logFile){
        logFile.println(dataString); //Print data to the file
        logFile.close(); //Close the file
      }
    Serial.println(dataString); //Print data to the serial monitor
    delay(500); //Brief delay 
  }
}
