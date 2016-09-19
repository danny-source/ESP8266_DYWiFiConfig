
#include <EEPROM.h>
#include "Configure_header.h"

WIFI_SETTINGS wifi_settings;

#define DEF_WIFI_SETTING_START_ADDRESS 200
#define DEF_WIFI_SETTING_PREFIX "W-A"

void configureBegin() {
	EEPROM.begin(512);
	checkPrefix();
}


void configureCommit() {
	EEPROM.commit();
	configurePrints(wifi_settings);
}

struct  WIFI_SETTINGS_s getConfigureStruct() {
	int count = EEPROM_readAnything(DEF_WIFI_SETTING_START_ADDRESS, wifi_settings);
	return wifi_settings;
}

void configureStruct(WIFI_SETTINGS_s &ws) {
	wifi_settings = ws;
	int count = EEPROM_writeAnything(DEF_WIFI_SETTING_START_ADDRESS, ws);
}

void configurePrints(WIFI_SETTINGS_s &ws) {
	Serial.println("==configurePrints==");
	Serial.print("SSID:");
	Serial.println(String(ws.SSID));
	Serial.print("PASSWORD:");
	Serial.println(String(ws.SSID_PASSWORD));
	Serial.print("DHCPAUTO:");
	Serial.println(ws.DHCPAUTO,DEC);
	Serial.print("IP:");
	Serial.print(ws.IP[0],DEC);
	Serial.print(".");
	Serial.print(ws.IP[1],DEC);
	Serial.print(".");
	Serial.print(ws.IP[2],DEC);
	Serial.print(".");
	Serial.print(ws.IP[3],DEC);
	Serial.println(" ");
	Serial.print("GW:");
	Serial.print(ws.GW[0],DEC);
	Serial.print(".");
	Serial.print(ws.GW[1],DEC);
	Serial.print(".");
	Serial.print(ws.GW[2],DEC);
	Serial.print(".");
	Serial.print(ws.GW[3],DEC);
	Serial.println(" ");
	Serial.print("MASK:");
	Serial.print(ws.SNET[0],DEC);
	Serial.print(".");
	Serial.print(ws.SNET[1],DEC);
	Serial.print(".");
	Serial.print(ws.SNET[2],DEC);
	Serial.print(".");
	Serial.print(ws.SNET[3],DEC);
	Serial.println(" ");
	Serial.print("DNS:");
	Serial.print(ws.DNS[0],DEC);
	Serial.print(".");
	Serial.print(ws.DNS[1],DEC);
	Serial.print(".");
	Serial.print(ws.DNS[2],DEC);
	Serial.print(".");
	Serial.print(ws.DNS[3],DEC);
	Serial.println(" ");

}

void checkPrefix() {
	WIFI_SETTINGS wifi_check;
	int count = EEPROM_readAnything(DEF_WIFI_SETTING_START_ADDRESS, wifi_check);
	String str = String(wifi_check.SETTING_DATA_PREFIX);
	if (str.indexOf(DEF_WIFI_SETTING_PREFIX) == -1) {
		configureClear();
		configureCommit();
		Serial.println("Check Prefix is fault, clear");
	}
}

void configureClear() {
	WIFI_SETTINGS wifi_clear = {0};
	strcpy(wifi_clear.SETTING_DATA_PREFIX,DEF_WIFI_SETTING_PREFIX);
	wifi_clear.NEED_FACTORY = 1;
	memset(wifi_clear.SSID,0,33);
	memset(wifi_clear.SSID_PASSWORD,0,33);
	memset(wifi_clear.IP,0,4);
	memset(wifi_clear.GW,0,4);
	memset(wifi_clear.SNET,0,4);
	memset(wifi_clear.DNS,0,4);
	wifi_clear.DHCPAUTO = 1;
	int count = EEPROM_writeAnything(DEF_WIFI_SETTING_START_ADDRESS, wifi_clear);
}

byte configureReadStateForFactory()
{
  Serial.print("Reading EEPROM Factory:");
  int count = EEPROM_readAnything(DEF_WIFI_SETTING_START_ADDRESS, wifi_settings);
  Serial.println(wifi_settings.NEED_FACTORY);
  return wifi_settings.NEED_FACTORY;
}

void configureWriteStateForFactory(byte state)
{
	Serial.print("Writing EEPROM Factory:");
	wifi_settings.NEED_FACTORY = state;
	int count = EEPROM_writeAnything(DEF_WIFI_SETTING_START_ADDRESS,wifi_settings);
	Serial.println(state);
}


void configureWriteSSIDName(String ssid)
{
	Serial.print("Writing EEPROM SSID:");
	memset(wifi_settings.SSID,0,33);
	strcpy(wifi_settings.SSID,ssid.c_str());
	int count = EEPROM_writeAnything(DEF_WIFI_SETTING_START_ADDRESS,wifi_settings);
	Serial.println(wifi_settings.SSID);
}

String configureReadSSIDName()
{
  Serial.print("Reading EEPROM SSID:");
  int count = EEPROM_readAnything(DEF_WIFI_SETTING_START_ADDRESS, wifi_settings);
  String esid = String(wifi_settings.SSID);
  Serial.println(esid);
  return esid;
}

String configureReadSSIDPassword()
{
  Serial.print("Reading EEPROM SSID Password:");
  int count = EEPROM_readAnything(DEF_WIFI_SETTING_START_ADDRESS, wifi_settings);
  String epass = String(wifi_settings.SSID_PASSWORD);
  Serial.println(epass);
  return epass;
}

void configureWriteSSIDPassword(String password)
{
	Serial.print("Writing EEPROM SSID Password:");
	memset(wifi_settings.SSID_PASSWORD,0,33);
	strcpy(wifi_settings.SSID_PASSWORD,password.c_str());
	int count = EEPROM_writeAnything(DEF_WIFI_SETTING_START_ADDRESS,wifi_settings);
	Serial.println(wifi_settings.SSID_PASSWORD);
}


//http://yehnan.blogspot.tw/2014/03/arduinoeeprom.html
template <class T> int EEPROM_writeAnything(int address, const T &data)
{
  const byte *p = (const byte *)(const void *)&data;
  int i, n;
  for(i = 0, n = sizeof(data); i < n; i++)
    EEPROM.write(address++, *p++);
  return i;
}
template <class T> int EEPROM_readAnything(int address, T &data)
{
  byte *p = (byte *)(void *)&data;
  int i, n;
  for(i = 0, n = sizeof(data); i < n; i++)
    *p++ = EEPROM.read(address++);
  return i;
}
