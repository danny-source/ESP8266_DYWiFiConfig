#include <DYWiFiConfig.h>

DYWiFiConfig wificonfig;
ESP8266WebServer webserver(80);
#define DEF_WIFI_SSID				"D1"
#define DEF_WIWI_PASSWORD			"01234567890"
#define AP_NAME "DYWiFi099"	//dev

void wificb(int c) {
	Serial.print("=-=-=-=-");
	Serial.println(c);
}

void setup() {
	Serial.begin(115200);
	delay(10);
	Serial.println("Startup");
	wificonfig.begin(&webserver, "/");
    DYWIFICONFIG_STRUCT defaultConfig =  wificonfig.createConfig();
    strcpy(defaultConfig.SSID,DEF_WIFI_SSID);
    strcpy(defaultConfig.SSID_PASSWORD,DEF_WIWI_PASSWORD);
    strcpy(defaultConfig.HOSTNAME,AP_NAME);
    strcpy(defaultConfig.APNAME,AP_NAME);
    wificonfig.setDefaultConfig(defaultConfig);
	wificonfig.enableAP();

}


void loop() {
  wificonfig.handle();
}
