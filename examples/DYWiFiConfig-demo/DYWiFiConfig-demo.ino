#include <DYWiFiConfig.h>

DYWiFiConfig wificonfig;
ESP8266WebServer webserver(80);

void wificb(int c) {
	Serial.print("=-=-=-=-");
	Serial.println(c);
}

void setup() {
	Serial.begin(115200);
	delay(10);
	Serial.println("Startup");
	wificonfig.begin(&webserver, "/");
	wificonfig.enableAP("DYWiFiConfig-1","01234567890");
	//wificonfig.setAP("DYWiFiConfig-1","01234567890");
	//wificonfig.autoEnableAP(D5);

}


void loop() {
  wificonfig.handle();
}
