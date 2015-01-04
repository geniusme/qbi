#include <jsonlite.h>
#include <LWiFi.h>
#include <LWiFiClient.h>

#include "M2XStreamClient.h"

int led = 13;

// WiFi
char ssid[] = "hue dev 121"; 
char pass[] = "philipshue";

const char hueHubIP[] = "172.23.144.140";  // Hue hub IP
const char hueUsername[] = "newdeveloper";  // Hue username
const int hueHubPort = 80;

char query[] = "limit=1";

char deviceId[] = "54539104e4b7269befaa31cf08dcdfe3"; // Device you want to push to
char streamName[] = "face";                           // Stream you want to push to
char m2xKey[] = "7ab968af6a406169d7ee826240a38bca";   // Your M2X access key

String hueCmd;  // Hue command

LWiFiClient client;
M2XStreamClient m2xClient(&client, m2xKey);

void on_data_point_found(const char* at, const char* value, int index, void* context, int type) {
  if (index == 0) {
    Serial.print("Found a data point, index:");
    Serial.println(index);
    Serial.print("Type:");
    Serial.println(type);
    Serial.print("At:");
    Serial.println(at);
    Serial.print("Value:");
    Serial.println(value);
    
    Serial.print("currentVal:");
    Serial.println(value[0]);
    
    if (value[0] == '1')
      Serial1.println("h");
    else if (value[0] == '2')
      turnLightOn();
    else if (value[0] == '3')
      turnLightOff();
    else if (value[0] == '4')
      Serial1.println("c");
  }
}

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(9600);
  while (!Serial.available()) {}
  digitalWrite(led, HIGH);
  LWiFi.begin();

  Serial1.begin(9600);
  Serial1.println("o");

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  if (LWiFi.connectWPA(ssid, pass) < 0) {
    Serial.println("Failed to connect to Wifi!");
    /* Loop forever */
    while (1) ;
  }
  
  Serial.println("Connected to wifi");
  Serial1.println("g");
}

void loop() {
  int response = m2xClient.listStreamValues(deviceId, streamName, on_data_point_found, query);
  Serial.print("M2x client response code: ");
  Serial.println(response);

  if (response == -1) while(1) ;

  delay(1000);
}

void turnLightOff() {
  Serial.println("Turning off the light");
}

void turnLightOn() {
  Serial.println("Turning on the light!");
  /*hueCmd = "{\"on\": true}";
  
  if (client.connect(hueHubIP, hueHubPort)) {
    Serial.println("Connected to client");
    while (client.connected())
    {
      client.print("PUT /api/");
      client.print(hueUsername);
      client.print("/lights/");
      client.print("2");
      client.println("/state HTTP/1.1");
      client.println("Connection: keep-alive");
      client.println("Content-Type: text/plain;charset=UTF-8");
      client.print("Content-Length: ");
      client.println(hueCmd.length());
      client.println();  // blank line before body
      client.println(hueCmd);  // Hue command
    }
   
    client.stop();
    Serial.println("Command Success");  // command executed
  }
  else
    Serial.println("Command Failed");*/
}
