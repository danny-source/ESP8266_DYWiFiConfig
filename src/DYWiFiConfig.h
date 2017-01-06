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
#define DYWIFI_STATE_REDISCONNECT 2
//callback

typedef void (*DYWIFI_STATECB)(int);

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
	void setWifiStateCB(DYWIFI_STATECB cb);
	private:
	DYStoreConfig _storeconfig;
	ESP8266WebServer *_server;
	String _scanAPsWebOptionCache;
	String _apname;
	String _appassword;
	String _webbase;
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
	int _wifiReconnectCount = 0;
	//
	int _autoEnableAPPin = -1;
	int _wifiStatus = WL_IDLE_STATUS;
	DYWIFI_STATECB wifiStateCB = NULL;
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
	 //
	void pageOfAdmin();
	void pageOfSetting();
	void pageOfReconnect();

};
#endif
