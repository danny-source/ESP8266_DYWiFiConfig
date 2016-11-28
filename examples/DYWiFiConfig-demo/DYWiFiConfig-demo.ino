#include <DYWiFiConfig.h>

DYWiFiConfig wificonfig;
ESP8266WebServer webserver(80);

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("Startup");
  wificonfig.begin(&webserver, "/", "DYWiFiConfig");


}


void loop() {
  wificonfig.handle();
}
