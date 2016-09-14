
#include <EEPROM.h>

typedef struct WIFI_SETTINGS_s {
	char SETTING_DATA_PREFIX[3];
	byte NEED_FACTORY;
	char SSID[33];
	char SSID_PASSWORD[33];
	char SPACE[2];
	byte DHCPAUTO;
	byte IP[4];
	byte GW[4];
	byte DNS[4];
}WIFI_SETTINGS;


WIFI_SETTINGS wifi_settings;

#define DEF_WIFI_SETTING_START_ADDRESS 200
#define DEF_WIFI_SETTING_PREFIX "W-B"

void configureBegin() {
	EEPROM.begin(512);
	strcpy(wifi_settings.SETTING_DATA_PREFIX,DEF_WIFI_SETTING_PREFIX);
	checkPrefix();
}


void configureCommit() {
	EEPROM.commit();
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
	WIFI_SETTINGS wifi_clear;
	strcpy(wifi_clear.SETTING_DATA_PREFIX,DEF_WIFI_SETTING_PREFIX);
	wifi_clear.NEED_FACTORY = 1;
	memset(wifi_clear.SSID,0,33);
	memset(wifi_clear.SSID_PASSWORD,0,33);
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
