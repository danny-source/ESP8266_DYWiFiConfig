#include "DYStoreConfig.h"
#include <EEPROM.h>

 extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "spi_flash.h"
}

extern "C" uint32_t _SPIFFS_end;
//Use last SPIFFS 4kbytes to store Config
EEPROMClass DYEEPROM((((uint32_t)&_SPIFFS_end - 0x40200000) / SPI_FLASH_SEC_SIZE) - 1);

DYStoreConfig::DYStoreConfig() {

}

void DYStoreConfig::begin(int allocsize,int storeaddress,DYWIFICONFIG_STRUCT *wifconfig_struct) {
	DYWIFICONFIG_DEBUG_PRINTLN("===begin===");
	DYWIFICONFIG_DEBUG_PRINTLN((((uint32_t)&_SPIFFS_end - 0x40200000) / SPI_FLASH_SEC_SIZE) - 1);
	_allocsize = allocsize;
	_storeaddress = storeaddress;
	_wificonfig = wifconfig_struct;
	DYEEPROM.begin(_allocsize);

}

void DYStoreConfig::read() {
	DYWIFICONFIG_DEBUG_PRINTLN("===read===");
	int count = read(_storeaddress, *_wificonfig);
	description();
}

template <class T> int DYStoreConfig::read(int address, T &data) {
  byte *p = (byte *)(void *)&data;
  int i, n;
  n = sizeof(data);
  DYWIFICONFIG_DEBUG_PRINT("read count:");
  DYWIFICONFIG_DEBUG_PRINTLN(n,DEC);
  for(i = 0, n = sizeof(data); i < n; i++)
    *p++ = DYEEPROM.read(address++);
  return i;
}

template <class T> int DYStoreConfig::write(int address, const T &data) {
  const byte *p = (const byte *)(const void *)&data;
  int i, n;
  n = sizeof(data);
  DYWIFICONFIG_DEBUG_PRINT("write count:");
  DYWIFICONFIG_DEBUG_PRINTLN(n,DEC);
  for(i = 0, n = sizeof(data); i < n; i++)
    DYEEPROM.write(address++, *p++);
  return i;
}

void DYStoreConfig::commit() {
	commit(*_wificonfig);
}

void DYStoreConfig::commit(DYWIFICONFIG_STRUCT s) {
	commit(&s);
}

void DYStoreConfig::commit(DYWIFICONFIG_STRUCT_PTR s) {
	DYWIFICONFIG_DEBUG_PRINT("===commit===");
	//strcpy(s->SETTING_DATA_PREFIX,DEF_DYWIFICONFIG_PREFIX);
	int count = write(_storeaddress, *s);
	description(s);
	bool y = DYEEPROM.commit();
	DYWIFICONFIG_DEBUG_PRINTLN((y?"YES":"NO"));
	DYEEPROM.begin(_allocsize);
}


void DYStoreConfig::clear() {
	DYWIFICONFIG_DEBUG_PRINTLN("===clear===");
	DYWIFICONFIG_STRUCT wifi_clear = {0};
	//strcpy(wifi_clear.SETTING_DATA_PREFIX,DEF_DYWIFICONFIG_PREFIX);
	wifi_clear.NEED_FACTORY = 0;
	memset(wifi_clear.SSID,0,33);
	memset(wifi_clear.SSID_PASSWORD,0,33);
	memset(wifi_clear.IP,0,4);
	memset(wifi_clear.GW,0,4);
	memset(wifi_clear.SNET,0,4);
	memset(wifi_clear.DNS,0,4);
	memset(wifi_clear.HOSTNAME,0,33);
	memset(wifi_clear.APNAME,0,33);
	memset(wifi_clear.APPASSWORD,0,33);
	wifi_clear.DHCPAUTO = 1;
	commit(wifi_clear);
	read();
}

void DYStoreConfig::description(DYWIFICONFIG_STRUCT s) {
	description(&s);
}

void DYStoreConfig::description(DYWIFICONFIG_STRUCT_PTR s) {
	DYWIFICONFIG_DEBUG_PRINTLN("===description===");
	DYWIFICONFIG_DEBUG_PRINT("PREFIX:");
	DYWIFICONFIG_DEBUG_PRINTLN(String(s->SETTING_DATA_PREFIX));
	DYWIFICONFIG_DEBUG_PRINT("SSID:");
	DYWIFICONFIG_DEBUG_PRINTLN(String(s->SSID));
	DYWIFICONFIG_DEBUG_PRINT("PASSWORD:");
	DYWIFICONFIG_DEBUG_PRINTLN(String(s->SSID_PASSWORD));
	DYWIFICONFIG_DEBUG_PRINT("DHCPAUTO:");
	DYWIFICONFIG_DEBUG_PRINTLN(s->DHCPAUTO,DEC);
	DYWIFICONFIG_DEBUG_PRINT("IP:");
	DYWIFICONFIG_DEBUG_PRINT(s->IP[0],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->IP[1],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->IP[2],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->IP[3],DEC);
	DYWIFICONFIG_DEBUG_PRINTLN(" ");
	DYWIFICONFIG_DEBUG_PRINT("GW:");
	DYWIFICONFIG_DEBUG_PRINT(s->GW[0],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->GW[1],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->GW[2],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->GW[3],DEC);
	DYWIFICONFIG_DEBUG_PRINTLN(" ");
	DYWIFICONFIG_DEBUG_PRINT("MASK:");
	DYWIFICONFIG_DEBUG_PRINT(s->SNET[0],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->SNET[1],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->SNET[2],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->SNET[3],DEC);
	DYWIFICONFIG_DEBUG_PRINTLN(" ");
	DYWIFICONFIG_DEBUG_PRINT("DNS:");
	DYWIFICONFIG_DEBUG_PRINT(s->DNS[0],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->DNS[1],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->DNS[2],DEC);
	DYWIFICONFIG_DEBUG_PRINT(".");
	DYWIFICONFIG_DEBUG_PRINT(s->DNS[3],DEC);
	DYWIFICONFIG_DEBUG_PRINTLN(" ");
	DYWIFICONFIG_DEBUG_PRINT("HOSTNAME:");
	DYWIFICONFIG_DEBUG_PRINTLN(String(s->HOSTNAME));
	DYWIFICONFIG_DEBUG_PRINT("AP NAME:");
	DYWIFICONFIG_DEBUG_PRINTLN(String(s->APNAME));
	DYWIFICONFIG_DEBUG_PRINT("AP PASSWORD:");
	DYWIFICONFIG_DEBUG_PRINTLN(String(s->APPASSWORD));
}
void DYStoreConfig::description() {
	description(_wificonfig);
}
//
//19*1024
