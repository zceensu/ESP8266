#include <Arduino.h>
#include "FS.h"
#include "webSocketFn.h"
#include "proximityFn.h"

#define RIGHT 15    // Corresponds to GPIO5 labelled pin D1 on NodeMCU board
#define LEFT 5    // Corresponds to GPIO15 labelled pin D8 on NodeMCU board
#define REDLED 16     // Corresponds to GPIO16 labelled pin D0 on NodeMCU board this pin is also connected to the LED cathode on the NodeMCU board
#define IRTX 0    // Corresponds to GPIO0 labelled pin D3 on NodeMCU board
#define IRTXBACK 4    // Corresponds to GPIO4 labelled pin D2 on NodeMCU board
#define BLUELED 2    // Corresponds to GPIO2 labelled pin D4 on NodeMCU board
#define REDLEDBACK 10    // Corresponds to GPIO10 labelled pin SD3 on NodeMCU board

#define IRRXR 12    // Corresponds to GPIO12 labelled pin D6 on NodeMCU board
#define IRRXL 14    // Corresponds to GPIO14 labelled pin D5 on NodeMCU board


const char *ssid = "ESPap";
const char *password = "thereisnospoon";

void setup() {
   
    USE_SERIAL.begin(115200);
    pinMode(RIGHT,OUTPUT);
    pinMode(LEFT,OUTPUT);
    pinMode(REDLED,OUTPUT);
    pinMode(BLUELED,OUTPUT);
    pinMode(IRTX,OUTPUT);
    pinMode(IRTXBACK,OUTPUT);
    pinMode(REDLEDBACK,OUTPUT);
    
    digitalWrite(RIGHT, LOW);
    digitalWrite(LEFT, LOW);
    digitalWrite(IRTX, HIGH);
    digitalWrite(IRTXBACK, HIGH);
    digitalWrite(REDLED, HIGH);
    digitalWrite(REDLEDBACK, HIGH);
    digitalWrite(BLUELED, LOW);

    pinMode(IRRXL, INPUT_PULLUP);
    analogWriteFreq(400);
    
/* Start File System      */
  bool ok = SPIFFS.begin();
  if (ok) Serial.println ( "File system OK" ) ;
  else Serial.println ( "Warning: File System did not initialise" ) ;

/* Create Access point on ESP8266     */ 
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  USE_SERIAL.print("AP IP address: ");
  USE_SERIAL.println(myIP);
  
/* Connect to external Access point     */
//    WiFiMulti.addAP("SSID", "passpasspass");
//    while(WiFiMulti.run() != WL_CONNECTED) {
//        delay(100);
//    }

/* Start the HTTP server      */
  server.on("/",handleWebsite);
  server.on("/logo.png",loadImage);
  server.on("/start.html",handleStart);
  server.on("/prox_sensor.html",handleProx);
  server.on("/websocketserver.html",handleWeb);
  server.on("/accelerometer_mozilla.html",handleAccel);
  server.on ( "/inline", []() {server.send ( 200, "text/plain", "this works as well" );} );
  server.onNotFound ( handleNotFound );
  server.begin();
  USE_SERIAL.println ( "HTTP server started" );

 /* Start the Web Socket server      */ 
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  USE_SERIAL.println ( "Web Socket server started" );
}

void loop() {
    server.handleClient();
    proximityRead();
    webSocket.loop();

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= PROXIMITY_INTERVAL) {
      previousMillis = currentMillis;
      if (front) {
        digitalWrite(BLUELED, LOW);
        IRmod(IRTX, 10000); 
        digitalWrite(BLUELED, HIGH);
      }
      else {
        digitalWrite(REDLEDBACK, LOW);
        IRmod(IRTXBACK, 10000);
        digitalWrite(REDLEDBACK, HIGH);
      }   
     }

     if (frontdet ) {
      
        distance = "Front Dist. L: " + String(pulselengthL) + " R:" + String(pulselengthR);
        //USE_SERIAL.println(distance);
        
     }
     
}
