#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define ACCESS_POINT_NAME "ESP-WEB-01"

int statusCode;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Startup");
  webConfigureWifi();

}


void loop() {
  webConfigureWifiHandle();
}
