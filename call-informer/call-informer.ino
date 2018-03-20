/**
 * ESP8266 
*/
#include "credentials.h"
#include <HashMap.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

//Output GPIO
uint8_t gpio1 = 12;
bool    gpio1On = false;

//HashMap storage for GPIOs and states
const byte HASH_SIZE = 3; //number of outputs, sattelites
// initialize storage
HashType<int, bool> hashRawArray[HASH_SIZE];
//handle storage
HashMap<int, bool> hashMap = HashMap<int,bool>( hashRawArray , HASH_SIZE );

//Init web-server
ESP8266WebServer server(80);

void setup()
{
  //assign hashmap [index](pin, state value)
  hashMap[0](12, false); //green on witty-cloud
  hashMap[1](13, false); //blue ~
  hashMap[2](15, false); //red
  
  //start serial for debug
  Serial.begin(115200);
  Serial.printf("\n\nFree memory %d\n", ESP.getFreeHeap());
  
  //Init output
  for(int index=0; index <=HASH_SIZE; index++)
  {
    pinMode(hashMap[index].getHash(), OUTPUT);
    digitalWrite(hashMap[index].getHash(), hashMap[index].getValue());
  }

  //Connect to WiFi
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  
  if(WiFi.waitForConnectResult() == WL_CONNECTED) {
    //Run Web-server
    MDNS.begin(host);
    server.on("/", HTTP_handleRoot);
    server.onNotFound(HTTP_handleRoot);
    server.begin();
    Serial.println(WiFi.localIP());
  } else {
    Serial.printf("WiFi device init failure");
  }
}

void loop()
{
  server.handleClient();
  delay(50);
}
void HTTP_handleRoot(void) { 
  String out = "";
  out =
"<html>\
  <head>\
    <meta charset=\"utf-8\" />\
    <title>WiFi semaphore</title>\
  </head>\
  <body>\
    <h1>WiFi Semaphore</h1>\
  </body>\
</html>";
   server.send ( 200, "text/html", out );   
}

