#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

ESP8266WebServer Pserver(80);
#define Configure_Server_P Pserver
#define ACCESS_POINT_NAME "ESP-WEB-01"

int statusCode;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Startup");
  webConfigureWifi();

}


void loop() {
  Configure_Server_P.handleClient();
}
