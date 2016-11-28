#include "DYStoreConfig.h"
/*


*/
DYStoreConfig::DYStoreConfig() {

}

void DYStoreConfig::begin(int allocsize,int storeaddress,DYWIFICONFIG_STRUCT *wifconfig_struct) {
	DYWIFICONFIG_DEBUG_PRINTLN("===begin===");
	_allocsize = allocsize;
	_storeaddress = storeaddress;
	_wificonfig = wifconfig_struct;
	EEPROM.begin(_allocsize);
	checkPrefix();
}

void DYStoreConfig::checkPrefix() {
	DYWIFICONFIG_DEBUG_PRINTLN("===checkPrefix===");
	DYWIFICONFIG_STRUCT wifi_check;
	int count = read(_storeaddress, wifi_check);
	description(wifi_check);
	String str(wifi_check.SETTING_DATA_PREFIX);
	if (str.indexOf(DEF_DYWIFICONFIG_PREFIX) == -1) {
		DYWIFICONFIG_DEBUG_PRINTLN("Check Prefix is fault, clear");
		clear();
		DYWIFICONFIG_DEBUG_PRINTLN("===l===");
	}
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
    *p++ = EEPROM.read(address++);
  return i;
}

template <class T> int DYStoreConfig::write(int address, const T &data) {
  const byte *p = (const byte *)(const void *)&data;
  int i, n;
  n = sizeof(data);
  DYWIFICONFIG_DEBUG_PRINT("write count:");
  DYWIFICONFIG_DEBUG_PRINTLN(n,DEC);
  for(i = 0, n = sizeof(data); i < n; i++)
    EEPROM.write(address++, *p++);
  return i;
}

void DYStoreConfig::commit() {
	commit(*_wificonfig);
}

void DYStoreConfig::commit(DYWIFICONFIG_STRUCT s) {
	//DYWIFICONFIG_DEBUG_PRINTLN("===commit===");
	//int count = EEPROM_writeAnything(_storeaddress, s);
	//description(s);
	//EEPROM.commit();
	//EEPROM.begin(_allocsize);
	commit(&s);
}

void DYStoreConfig::commit(DYWIFICONFIG_STRUCT_PTR s) {
	DYWIFICONFIG_DEBUG_PRINTLN("===commit===");
	int count = write(_storeaddress, *s);
	description(s);
	EEPROM.commit();
}


void DYStoreConfig::clear() {
	DYWIFICONFIG_DEBUG_PRINTLN("===clear===");
	DYWIFICONFIG_STRUCT wifi_clear = {0};
	strcpy(wifi_clear.SETTING_DATA_PREFIX,DEF_DYWIFICONFIG_PREFIX);
	wifi_clear.NEED_FACTORY = 1;
	memset(wifi_clear.SSID,0,33);
	memset(wifi_clear.SSID_PASSWORD,0,33);
	memset(wifi_clear.IP,0,4);
	memset(wifi_clear.GW,0,4);
	memset(wifi_clear.SNET,0,4);
	memset(wifi_clear.DNS,0,4);
	wifi_clear.DHCPAUTO = 1;
	commit(wifi_clear);
	read();
}

void DYStoreConfig::description(DYWIFICONFIG_STRUCT s) {
	//DYWIFICONFIG_DEBUG_PRINTLN("===description===");
	//DYWIFICONFIG_DEBUG_PRINT("PREFIX:");
	//DYWIFICONFIG_DEBUG_PRINTLN(String(s.SETTING_DATA_PREFIX));
	//DYWIFICONFIG_DEBUG_PRINT("SSID:");
	//DYWIFICONFIG_DEBUG_PRINTLN(String(s.SSID));
	//DYWIFICONFIG_DEBUG_PRINT("PASSWORD:");
	//DYWIFICONFIG_DEBUG_PRINTLN(String(s.SSID_PASSWORD));
	//DYWIFICONFIG_DEBUG_PRINT("DHCPAUTO:");
	//DYWIFICONFIG_DEBUG_PRINTLN(s.DHCPAUTO,DEC);
	//DYWIFICONFIG_DEBUG_PRINT("IP:");
	//DYWIFICONFIG_DEBUG_PRINT(s.IP[0],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.IP[1],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.IP[2],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.IP[3],DEC);
	//DYWIFICONFIG_DEBUG_PRINTLN(" ");
	//DYWIFICONFIG_DEBUG_PRINT("GW:");
	//DYWIFICONFIG_DEBUG_PRINT(s.GW[0],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.GW[1],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.GW[2],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.GW[3],DEC);
	//DYWIFICONFIG_DEBUG_PRINTLN(" ");
	//DYWIFICONFIG_DEBUG_PRINT("MASK:");
	//DYWIFICONFIG_DEBUG_PRINT(s.SNET[0],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.SNET[1],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.SNET[2],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.SNET[3],DEC);
	//DYWIFICONFIG_DEBUG_PRINTLN(" ");
	//DYWIFICONFIG_DEBUG_PRINT("DNS:");
	//DYWIFICONFIG_DEBUG_PRINT(s.DNS[0],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.DNS[1],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.DNS[2],DEC);
	//DYWIFICONFIG_DEBUG_PRINT(".");
	//DYWIFICONFIG_DEBUG_PRINT(s.DNS[3],DEC);
	//DYWIFICONFIG_DEBUG_PRINTLN(" ");
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
}
void DYStoreConfig::description() {
	description(_wificonfig);
}
