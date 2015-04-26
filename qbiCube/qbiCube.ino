#include <Suli.h>
#include "Barometer.h"
#include <Wire.h>
#include "acc_adxl345_Arduino.h"
#include <jsonlite.h>
#include <LWiFi.h>
#include <LWiFiClient.h>
#include "M2XStreamClient.h"

// WiFi
char ssid[] = "hue dev 121"; 
char pass[] = "philipshue";

// Accelerometer
Acc_Adxl345 acc;
int led = 13;
float n1 = .5;
float n2 = .2;
int currentFace = 0;

int axisReadBuf = 700;
int lastReadTime = 0;
int lastReadFace = 0;


// Temp and Barometer Sensor
float temperature;
float pressure;
float atm;
float altitude;
Barometer myBarometer;
int lastLogTempAndBar = 0;
int tempBarLogInterval = 5000;


// M2X Info
char deviceId[] = "54539104e4b7269befaa31cf08dcdfe3"; // Device you want to push to
char streamName[] = "face";                           // Stream you want to push to
char m2xKey[] = "7ab968af6a406169d7ee826240a38bca";   // Your M2X access key

char tempStreamName[] = "temperature";

LWiFiClient client;
M2XStreamClient m2xClient(&client, m2xKey);

void setup() {
    pinMode(led, OUTPUT);
    Serial.begin(9600);
    digitalWrite(led, LOW);
   /* while (!Serial)
    {
        ; // wait for serial port to connect. 
    }*/
    digitalWrite(led, HIGH);
    acc.begin();
    Serial.println("Hi, I'm Qbi :)");
    
    LWiFi.begin();

    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    if (LWiFi.connectWPA(ssid, pass) < 0) {
      Serial.println("Failed to connect to Wifi!");
      /* Loop forever */
      while (1) ;
    }
    Serial.println("Connected to wifi");
}

void loop() {
  float ax, ay, az;
  int readFace = 0;
  int lastFace;
  int startChangeTime = 0;
 
  acc.get(&ax, &ay, &az);             // get value from adxl345
  if (ax < -n1 && (abs(ay) + abs(az)) < n2) {        // X Up
    readFace = 0;
  } else if (ax > n1 && (abs(ay) + abs(az)) < n2) {  // X Down
    readFace = 5;
  } else if (ay < -n1 && (abs(az) + abs(ax)) < n2) { // Y Up
    readFace = 2;
  } else if (ay > n1 && (abs(az) + abs(ax)) < n2) {  // Y Down
    readFace = 3;
  } else if (az < -n1 && (abs(ay) + abs(ax)) < n2) { // Z Up
    readFace = 4;
  } else if (az > n1 && (abs(ay) + abs(ax)) < n2) {  // Z Down
    readFace = 1;
  }
  
  
  if (lastReadFace != readFace && readFace != currentFace) {
    lastReadTime = millis();
    lastReadFace = readFace;
  } else if ((lastReadTime +  axisReadBuf) < millis() && lastReadFace == readFace && readFace != currentFace) {
    currentFace = readFace;
    logFaceChange();
  }
  
  // Is it time to log Temp and Bar
  if (millis() > (lastLogTempAndBar + tempBarLogInterval)) {
    lastLogTempAndBar = millis();
    
    temperature = 69.2;
    /*
    temperature = myBarometer.bmp085GetTemperature(myBarometer.bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
    pressure = myBarometer.bmp085GetPressure(myBarometer.bmp085ReadUP());//Get the temperature
    altitude = myBarometer.calcAltitude(pressure); //Uncompensated caculation - in Meters
    atm = pressure / 101325;

    Serial.print("Temperature: ");
    Serial.print(temperature, 2); //display 2 decimal places
    Serial.println("deg C");

    Serial.print("Pressure: ");
    Serial.print(pressure, 0); //whole number only.
    Serial.println(" Pa");

    Serial.print("Ralated Atmosphere: ");
    Serial.println(atm, 4); //display 4 decimal places

    Serial.print("Altitude: ");
    Serial.print(altitude, 2); //display 2 decimal places
    Serial.println(" m");

    Serial.println();*/
  }

}

void  logFaceChange() {
    Serial.print("FACE: ");
    Serial.println(currentFace);
    
    int response = m2xClient.updateStreamValue(deviceId, streamName, currentFace);
    Serial.print("M2X client response code: ");
    Serial.println(response);
    
    if (response == -1) while(1) ;
} 
