#ifndef _DYWIFICONFIG_H_
#define _DYWIFICONFIG_H_
#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "Configure_index.h"
#include "Configure_header.h"
#include "Configure_redirect.h"
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif
#include "DYStoreConfig.h"

#define DYWIFI_STATE_DISCONNECT 1
#define DYWIFI_STATE_RECONNECT 2
//callback

typedef void (*DYWifiStateCallback)(int);

class DYWiFiConfig {
	public:
	DYWiFiConfig();
	virtual ~DYWiFiConfig() {}
	void begin(ESP8266WebServer *server, const char *webbase);
	void handle();
 	template <class T> int read(int address, T &data);			//address of 0 ~ 300
	template <class T> int write(int address, const T &data);	//address of 0 ~ 300
	void commit();
	void setAP(const char *name,const char *password);
	void enableAP();
	void disableAP();
	void enableAP(const char *name,const char *password);
	void autoEnableAP(int pin);
	void reConnect();									//set pin to -1 to disable
	void setWifiStateCallback(DYWifiStateCallback cb);
	void setWebReturnPath(const char *path);
	bool setDefaultConfig(DYWIFICONFIG_STRUCT s);
	DYWIFICONFIG_STRUCT createConfig();
	private:
	DYStoreConfig _storeconfig;
	DYWIFICONFIG_STRUCT_PTR _defaultconfig;
	ESP8266WebServer *_server;
	String _scanAPsWebOptionCache;
	String _apname;
	String _appassword;
	String _webPath;
	String _webReturnPath;
	DYWIFICONFIG_STRUCT _dws;
	int _wifiStateMachine = 0;
	//
	long _taskStartTime = 0;
	long _taskEndTime = 0;
	int _taskTimerCounter = 0;
	int _taskState = 0;
	int _nextTaskState = 0;
	long _task10SecondBase = 10;
	long _task20SecondBase = 20;
	long _task40SecondBase = 40;
	long _taskClearCounter;
	int _wifiReconnectCount = 0;
	//
	int _autoEnableAPPin = -1;
	int _wifiStatus = WL_IDLE_STATUS;
	DYWifiStateCallback wifiStateCB;
	//
	bool autoConnectToAP();
	// void createWebServer(const char *webbase);
	void scanAPs(void);
	 void sendHtmlPageWithRedirectByTimer(String gotoUrl,String Message);
	bool setDHCP(byte isAuto);
	int setWifi(String essid, String epassword);
	void setupWeb();
	void createWebServer();
	void taskSchdule();
	void taskSchdule01Second();
	void taskSchdule10Second();
	void taskSchdule20Second();
	void taskSchdule40Second();
	 //
	void pageOfAdmin();
	void pageOfSetting();
	void pageOfReconnect();
	//
	int mathGCD(int m, int n);
	int mathLCM(int m, int n);

};
#endif
