#include "FS.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//Include the HTML, STYLE and Script "Pages"
//#include "webpage.h"

const char *ssid = "ESPap";
const char *password = "thereisnospoon";

// Create an instance of the server specify the port to listen on as an argument
ESP8266WebServer server(80);


//----------------------------------------------------------------------- 
void handleWebsite(){
  bool exist = SPIFFS.exists("/index.html");
  if (exist) {
    Serial.println("The file exists");
    File f = SPIFFS.open("/index.html", "r");
      if(!f){
        Serial.println("/index.html failed to open");
      }
      else {
        String data = f.readString() ;
        server.send(200,"text/html",data);
        f.close();
      }
  }
  else {
    Serial.println("No such file found.");
  }
}

//----------------------------------------------------------------------- 
void loadImage(){
  bool exist = SPIFFS.exists("/smiley.png");
  if (exist) {
    Serial.println("The image exists");
    File dataFile = SPIFFS.open("/smiley.png", "r");
      if(!dataFile){
        Serial.println("/smiley.png failed to open");
      }
      else {
        if (server.streamFile(dataFile, "image/png") != dataFile.size()) {}
        dataFile.close();
      }
  }
  else {
    Serial.println("No such file found.");
  }
}

//----------------------------------------------------------------------- 
void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
        message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
    }
    server.send ( 404, "text/plain", message );
}
//----------------------------------------------------------------------- 
  
void setup () {
  Serial . begin (115200) ;

  bool ok = SPIFFS.begin(); // start the file system
  if (ok) Serial.println ( "File system OK" ) ;
  else Serial.println ( "Warning: File System did not initialise" ) ;

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
 
  // Start the server
   server.on("/",handleWebsite);
   server.on("/smiley.png",loadImage);
   
   server.on ( "/inline", []() {server.send ( 200, "text/plain", "this works as well" );} );
   server.onNotFound ( handleNotFound );
   server.begin();
   Serial.println ( "HTTP server started" );
  
}

void loop () {
  server.handleClient();
}

