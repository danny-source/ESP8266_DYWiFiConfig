
#ifndef CONFIGURE_HEADER
#define CONFIGURE_HEADER

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
}WIFI_SETTINGS,*WIFI_SETTINGS_PTR;

#endif
