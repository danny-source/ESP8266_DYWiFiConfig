
#ifndef CONFIGURE_HEADER
#define CONFIGURE_HEADER

//typedef struct WIFI_SETTINGS_s {
	//char SETTING_DATA_PREFIX[3];
	//byte NEED_FACTORY;
	//char SSID[33];
	//char SSID_PASSWORD[33];
	//char SPACE[2];
	//byte DHCPAUTO;
	//byte IP[4];
	//byte GW[4];
	//byte SNET[4];
	//byte DNS[4];
	//char HOSTNAME[50];
//}WIFI_SETTINGS,*WIFI_SETTINGS_PTR;
typedef struct DYWIFICONFIG_STRUCT_s {
	char SETTING_DATA_PREFIX[4];
	uint8_t NEED_FACTORY;
	char SSID[33];
	char SSID_PASSWORD[33];
	char SPACE[2];
	uint8_t DHCPAUTO;
	uint8_t IP[4];
	uint8_t GW[4];
	uint8_t SNET[4];
	uint8_t DNS[4];
	char HOSTNAME[33];
	char APNAME[33];
	char APPASSWORD[33];
}DYWIFICONFIG_STRUCT,*DYWIFICONFIG_STRUCT_PTR;
#endif
