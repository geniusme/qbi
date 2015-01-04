#include <Suli.h>
#include <Wire.h>
#include "acc_adxl345_Arduino.h"
#include <jsonlite.h>
#include <LWiFi.h>
#include <LWiFiClient.h>
#include "M2XStreamClient.h"

char ssid[] = "hodor929"; //  your network SSID (name)
char pass[] = "hodorhodor";    // your network password (use for WPA, or use as key for WEP)

Acc_Adxl345 acc;
int led = 13;
float n1 = .5;
float n2 = .2;
int currentFace = 0;

int axisReadBuf = 700;
int lastReadTime = 0;
int lastReadFace = 0;

char deviceId[] = "54539104e4b7269befaa31cf08dcdfe3"; // Device you want to push to
char streamName[] = "face";                           // Stream you want to push to
char m2xKey[] = "7ab968af6a406169d7ee826240a38bca";   // Your M2X access key

LWiFiClient client;
M2XStreamClient m2xClient(&client, m2xKey);

void setup() {
    pinMode(led, OUTPUT);
    Serial.begin(9600);
    digitalWrite(led, LOW);
    while (!Serial)
    {
        ; // wait for serial port to connect. 
    }
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
    readFace = 1;
  } else if (ay < -n1 && (abs(az) + abs(az)) < n2) { // Y Up
    readFace = 2;
  } else if (ay > n1 && (abs(az) + abs(az)) < n2) {  // Y Down
    readFace = 3;
  } else if (az < -n1 && (abs(ay) + abs(ax)) < n2) { // Z Up
    readFace = 4;
  } else if (az > n1 && (abs(ay) + abs(ax)) < n2) {  // Z Down
    readFace = 5;
  }
  
  
  if (lastReadFace != readFace && readFace != currentFace) {
    lastReadTime = millis();
    lastReadFace = readFace;
  } else if ((lastReadTime +  axisReadBuf) < millis() && lastReadFace == readFace && readFace != currentFace) {
    currentFace = readFace;
    logFaceChange();
  }
  
  /*
  Serial.print(ax);                   // print it out
  Serial.print('\t');
  Serial.print(ay);
  Serial.print('\t');
  Serial.print(az);
  Serial.print('\t');
  Serial.println(face);
  */
  
  delay(100);                         // delay
}

void  logFaceChange() {
    Serial.print("FACE: ");
    Serial.println(currentFace);
    
    int response = m2xClient.updateStreamValue(deviceId, streamName, currentFace);
    Serial.print("M2X client response code: ");
    Serial.println(response);
    
    if (response == -1) while(1) ;
}
