/*
 * The purrpose of this script is to provide an updated version of the systems software, which is centered around eliminating the SD card reader.
 * This removal is being conducted based on the idea that the transmission and receiving of data from the sensors in the system will occur 
 * wirelessly. Other than removing the SD card related lines of code, the code will remain about the same as the previous version of this 
 * software.
 * 
 * Due to the lack of a telemetry system, additional software was added to save the data collected to a file on the computer instead
 * of the SD card.
 * 
 * Author: Christopher Swinford
 * Date: August 25, 2018
 * Version: 1.1.0
 */

#include <SPI.h> //Serial data protocol used by microcontrollers for communicating with one or more peripheral devices quickly over short distances.
#include <SoftwareSerial.h> //Allow serial communication on other digital pins of the Arduino, using software to replicate the functionality

//Initialization of variables
SoftwareSerial GPSModule(3, 1); //Rx, Tx //Sets up a software serial object
int pos;
int stringplace = 0;
const int humidPin = A1;
const int tempPin = A2;
String nmea[15];
char degreeSymb = 176;

void setup() {
  Serial.begin(9600); //Sets the data rate in bits per second for serial data transmission. Rate = 9600 bits per second
  GPSModule.begin(9600); //Sets the speed (baud rate) for the serial communication.
  Serial.print("Initializing Telemetry..."); //Prints to computer screen an initialization message

  //Add initialization of Telemetry here
  Serial.println("Telemetry Initialization Complete"); //Print success message to the computer
  
  String header = "Time, Latitude, Hemisphere, Longitude, Hemishpere, Fix, Altitude(MSL/M), Altitude(WGS84/M), Temperature [" + String(degreeSymb) + "C], Humidity"; //Define the string for the header of the file
  Serial.println(header);  //Prints the header to the computer monitor
  

}

void loop() {
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
    
//Temperature Sensor Reading
    int tempvalue, ftemp; //Declare integer variables
    float Voltage, temp; //Declare float variables
    tempvalue = analogRead(tempPin); //Read the analog port 2 and store the value in tempval
    //Conversion for temperature based on the voltage from sensor
    Voltage = tempvalue*((5.0/1023)*10);
    temp = (-40+(Voltage*10)); //Temperature in C
    ftemp = ((temp*1.8)+32); //Temperature in F
    
//Humidity Sensor Reading
    int humvalue = analogRead(humidPin);; //Create an integer variable for humidity and read the analog port 1 and store the value in var
    float HVoltage = humvalue*((5.0/1023)*100); //Conversion for humidity based on the voltage from sensor

//Display the collected data to the serial monitor
    //Data format
    String dataString = nmea[0] + ", " + nmea[1] + ", " + nmea[2] + ", " + nmea[3] + ", " + nmea[4] + ", " + nmea[5] + ", " + nmea[8] + ", " + nmea[10] + ", " + String(temp) + ", " + String(HVoltage);
    Serial.println(dataString); //Print data to the serial monitor
    delay(500); //Brief delay
  }
}
