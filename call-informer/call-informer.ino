/**
   ESP8266 WiFi Controlled semaphore
*/
#include "credentials.h"
#include "HashMap.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

//HashMap storage for GPIOs and states
const byte HASH_SIZE = 3; //number of outputs, sattelites
// initialize storage
HashType<int, bool> hashRawArray[HASH_SIZE];
//handle storage
HashMap<int, bool> hashMap = HashMap<int, bool>( hashRawArray , HASH_SIZE );

//Init web-server
ESP8266WebServer server(80);

void setup()
{
  //assign hashmap [index](pin, state value)
  hashMap[0](12, false); //green on witty-cloud
  hashMap[1](13, false); //blue ~
  hashMap[2](15, false); //red ~

  //start serial for debug
  Serial.begin(9600);
  Serial.printf("\n\nFree memory %d\n", ESP.getFreeHeap());

  //Init output
  for (int index = 0; index < HASH_SIZE; index++)
  {
    pinMode(hashMap[index].getHash(), OUTPUT);
    digitalWrite(hashMap[index].getHash(), hashMap[index].getValue());
  }

  //Connect to WiFi
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    //Run Web-server
    MDNS.begin(host);
    server.on("/", HTTP_handleRoot);
    server.onNotFound(HTTP_handleRoot);
    server.begin();
    Serial.println(WiFi.localIP());
    blinkGreen();
  } else {
    Serial.printf("WiFi device init failure");
    blinkRed();
  }
}

void loop()
{
  server.handleClient();
  delay(50);
}

void blinkGreen()
{
  digitalWrite(12, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(12, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}

void blinkRed()
{
  digitalWrite(15, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(15, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);               // wait for a second
}

void HTTP_handleRoot(void) {
  //local reflection of statuses from hashMap or server.args()
  bool statuses[HASH_SIZE];

  for (uint8_t i = 0; i < HASH_SIZE; i++) {
    if (strncmp(server.arg(i).c_str(), "1", 1) == 0) {
      statuses[i] = true;
    } else {
      statuses[i] = false;
    }
  }

  String out = "";
  out =
    "<html>\
      <head>\
        <meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width, initial-scale=1\" />\
        <title>WiFi semaphore</title>\
        <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\" integrity=\"sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm\" crossorigin=\"anonymous\">\
      </head>\
      <body>\
        <div class=\"container\">\
          <div class=\"row\">\
            <h1>WiFi Semaphore</h1>\
          </div>\
          <div class=\"container-fluid\"> ";
  for (uint8_t i = 0; i < HASH_SIZE; i++)
  {
    //string-casted value of status for current cell
    String status = statuses[i] ? "On" : "Off";
    //string-casted number of current cell
    String cellNumber = String(hashMap.getIndexHash(i));

    out += "<div class=\"row\">\
              <a class=\"btn " + String(statuses[i] ? "btn-danger" : "btn-success") + " \" href=\"?";
    for (uint8_t j = 0; j < HASH_SIZE; j++)
    {
      if (i == j)
      {
        out += String(j) + "=" + String(statuses[j] ? "0" : "1");
      } else {
        out += String(j) + "=" + String(statuses[j] ? "1" : "0");
      }

      if (j < HASH_SIZE)
      {
        out += "&";
      }
    }
    out += "\">Cell #" + cellNumber + " is " + status + "</a></div><br/>";
  }
  out += "</div>\
        </div>\
      </body>\
    </html>";
  server.send ( 200, "text/html", out );
  for (uint8_t i = 0; i < HASH_SIZE; i++)
  {
    bool status = statuses[i];
    hashMap[i].setValue(status);
    digitalWrite(hashMap[i].getHash(), status);
  }
}

