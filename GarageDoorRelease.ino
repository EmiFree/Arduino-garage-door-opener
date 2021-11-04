  int upSensor = 14;
  int led = 13;
  int doorTrigger = 12;
  int upVal = 0;


// Uncomment the following line to enable serial debug output
//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG_ESP_PORT Serial
#define NODEBUG_WEBSOCKETS
#define NDEBUG
#endif 

#include <Arduino.h>
#ifdef ESP8266 
#include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
#include <WiFi.h>
#endif

#include "SinricPro.h"
#include "SinricProGarageDoor.h"

    #define WIFI_SSID         "Wifi name"                       //should be a 2.4ghz network, not 5ghz
    #define WIFI_PASS         "Wifi password"
    #define APP_KEY           "get from https://sinric.pro"     // Should look like "de0bxxxx-1x3x-4x3x-ax2x-5dabxxxxxxxx"
    #define APP_SECRET        "get from https://sinric.pro"     // Should look like "5f36xxxx-x3x7-4x3x-xexe-e86724a9xxxx-4c4axxxx-3x3x-x5xe-x9x3-333d65xxxxxx"
    #define GARAGEDOOR_ID     "get from https://sinric.pro"     // Should look like "5dc1564130xxxxxxxxxxxxxx"
    #define BAUD_RATE         115200                            // Change baudrate to your need, currently set to be used with keeyees esp8266



    bool onDoorState(const String& deviceId, bool &doorState) {
  
      Serial.printf("Garagedoor is %s now.\r\n", doorState?"closed":"open");
      
        switch (doorState)
        {
          case true:                              //case true is a request to close the door
            if (upVal == LOW)                     //only close the door if the proximity sensor has something in front of it (the door is open)
            {
              digitalWrite(doorTrigger, HIGH);    //simulate a press on the garage remote button
              delay(510);
              digitalWrite(doorTrigger, LOW);
              return true;
            } 
            
            if (upVal == HIGH)
            {
              return false;                       //send back false if door is requested to close but it is already closed. Alexa will interperet this as an error, however this is something you can just ignore
            }
            
          break;
      
          case false:                             //case false is a request to open the door
            if (upVal == HIGH)                    //only open the door if the proximity sensor has nothing in front of it (the door is closed)
            {
              digitalWrite(doorTrigger, HIGH);    //simulate a press on the garage remote button
              delay(510);
              digitalWrite(doorTrigger, LOW);
              return true; 
            }
            
            if (upVal == LOW)
            {
              return false;                       //send back false if door is requested to open but it is already open. Alexa will interperet this as an error, however this is something you can just ignore
            }
  
          break;
        }
    }



void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

void setupSinricPro() {
  SinricProGarageDoor &myGarageDoor = SinricPro[GARAGEDOOR_ID];
  myGarageDoor.onDoorState(onDoorState);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void setup() {
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
  pinMode(led, OUTPUT);                     //set up all inputs and outputs
  pinMode(upSensor, INPUT);
  pinMode(doorTrigger, OUTPUT);
}

    void loop() { 
      upVal = digitalRead(upSensor);
      digitalWrite(led, !upVal);            //continuously set the indicator light to reflect what the proximity sensor sees.  
      SinricPro.handle();
    }

/*
 * Made from garage door sample code
 * These comments were included in the sample code
 * If you encounter any issues:
 * - check the readme.md at https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md
 * - ensure all dependent libraries are installed
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#arduinoide
 *   - see https://github.com/sinricpro/esp8266-esp32-sdk/blob/master/README.md#dependencies
 * - open serial monitor and check whats happening
 * - check full user documentation at https://sinricpro.github.io/esp8266-esp32-sdk
 * - visit https://github.com/sinricpro/esp8266-esp32-sdk/issues and check for existing issues or open a new one
 */
