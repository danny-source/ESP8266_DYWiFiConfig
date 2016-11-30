#ifndef _DYSTORECONFIG_H_
#define _DYSTORECONFIG_H_
#include <Arduino.h>
#include <EEPROM.h>

#define DEF_DYWIFICONFIG_START_ADDRESS 200
#define DEF_DYWIFICONFIG_PREFIX "W-C"

#define DYWIFICONFIG_DEBUG
#define DYWIFICONFIG_DEBUG_SERIAL Serial

#ifdef DYWIFICONFIG_DEBUG
  #define DYWIFICONFIG_DEBUG_PRINT(...) { DYWIFICONFIG_DEBUG_SERIAL.print(__VA_ARGS__); }
  #define DYWIFICONFIG_DEBUG_PRINTLN(...) { DYWIFICONFIG_DEBUG_SERIAL.println(__VA_ARGS__); }
#else
  #define DYWIFICONFIG_DEBUG_PRINT(...) {}
  #define DYWIFICONFIG_DEBUG_PRINTLN(...) {}
#endif

typedef struct DYWIFICONFIG_STRUCT_s {
	char SETTING_DATA_PREFIX[4];
	byte NEED_FACTORY;
	char SSID[33];
	char SSID_PASSWORD[33];
	char SPACE[2];
	byte DHCPAUTO;
	byte IP[4];
	byte GW[4];
	byte SNET[4];
	byte DNS[4];
}DYWIFICONFIG_STRUCT,*DYWIFICONFIG_STRUCT_PTR;

class DYStoreConfig {
	public:
	DYStoreConfig();
	virtual ~DYStoreConfig() {}
	void begin(int allocsize, int storeaddress, DYWIFICONFIG_STRUCT *wifconfig_struct);
	void read();
	template <class T> int read(int address, T &data);
	template <class T> int write(int address, const T &data);
	void commit();
	void commit(DYWIFICONFIG_STRUCT s);
	void commit(DYWIFICONFIG_STRUCT_PTR s);
	void description();
	void description(DYWIFICONFIG_STRUCT s);
	void description(DYWIFICONFIG_STRUCT_PTR s);
	private:
	DYWIFICONFIG_STRUCT *_wificonfig;
	int _storeaddress;
	int _allocsize;
	void checkPrefix();
	void clear();
};
//template <class T> int EEPROM_writeAnything(int address, const T &data);
//template <class T> int EEPROM_readAnything(int address, T &data);
#endif
