
#include "DYWiFiConfig.h"

#define DYEEPRO_SIZE 512

DYWiFiConfig::DYWiFiConfig() {

}

void DYWiFiConfig::begin(ESP8266WebServer *server, const char *webbase, const char *apname) {
	_server = server;
	_storeconfig.begin(DYEEPRO_SIZE, 0, &_dws);
	_storeconfig.read();
	WiFi.mode(WIFI_AP_STA);
	WiFi.disconnect();
	String _apname(apname);
	DYWIFICONFIG_DEBUG_PRINT(":apname:");
	DYWIFICONFIG_DEBUG_PRINTLN(_apname);
	_webbase = _webbase + String(webbase);
	if (_webbase.lastIndexOf("/") == -1) {
		_webbase =  _webbase + "/";
	}
	DYWIFICONFIG_DEBUG_PRINT(":1:webbase:");
	DYWIFICONFIG_DEBUG_PRINTLN(_webbase);

	WiFi.softAP(_apname.c_str(), NULL);
	_wifiStateMachine = 1;
	scanAPs();
	setupWeb();
	 _wifiReconnectCount = 0;
	 _taskState = 0;
	 _taskStartTime = millis();
	 _task10SecondBase = 10;
	 _task20SecondBase = 20;
	 autoConnectToAP();
}

 void DYWiFiConfig::handle() {
 	taskSchdule();
 	_server->handleClient();
 }

 void DYWiFiConfig::taskSchdule() {
  _taskEndTime = millis();
  long _taskTimeTemp = _taskEndTime - _taskStartTime;
 	_taskTimeTemp = (long)(_taskTimeTemp / 1000);
   if ( _taskTimeTemp > 0) {
     _taskStartTime=_taskEndTime;
     _taskTimerCounter = _taskTimerCounter + _taskTimeTemp;
     //every 1 second
     taskSchdule01Second();
     //every 2 second
     if ( _taskTimerCounter >= _task10SecondBase) {
         _task10SecondBase += 10;
         taskSchdule10Second();
     }
     if ( _taskTimerCounter >= _task20SecondBase) {
         _task20SecondBase += 20;
         taskSchdule20Second();
     }
     //clear counter
     if (_taskTimerCounter >= 60) {
      _taskTimerCounter = 0;
      _task10SecondBase = 10;
      _task20SecondBase = 20;
     }
   }

 }

 void DYWiFiConfig::taskSchdule01Second() {
   //DYWEB_DEBUG_PRINTLN("DYWEB:01Sedond:");
	if (_taskState == DYWIFI_STATE_DISCONNECT) {
		DYWIFICONFIG_DEBUG_PRINTLN(":disconnect");
		_wifiReconnectCount = 0;
		WiFi.disconnect();
   }
   if (_taskState == DYWIFI_STATE_REDISCONNECT) {
     DYWIFICONFIG_DEBUG_PRINTLN(":Reconnect");
     if (autoConnectToAP()) {
     }
   }
   _taskState = _nextTaskState;
   _nextTaskState = 0;
 }

 void DYWiFiConfig::taskSchdule10Second() {
     DYWIFICONFIG_DEBUG_PRINT(":10Sedond:");
     DYWIFICONFIG_DEBUG_PRINTLN(_taskTimerCounter,DEC);
 		autoConnectToAP();
 }

 void DYWiFiConfig::taskSchdule20Second() {
	DYWIFICONFIG_DEBUG_PRINT(":20Sedond:");
	DYWIFICONFIG_DEBUG_PRINTLN(_taskTimerCounter,DEC);
	scanAPs();
 }

void DYWiFiConfig::scanAPs(void) {
  delay(100);
  DYWIFICONFIG_DEBUG_PRINTLN(":Scan networks");
  int n = WiFi.scanNetworks();
  if (n == 0) {
    DYWIFICONFIG_DEBUG_PRINTLN(":no networks found");
  } else {
		_scanAPsWebOptionCache = "";
		DYWIFICONFIG_DEBUG_PRINT(":");
		DYWIFICONFIG_DEBUG_PRINT(n);
		DYWIFICONFIG_DEBUG_PRINTLN(" networks found");
		for (int i = 0; i < n; ++i)
		{
			_scanAPsWebOptionCache +="<option value=\"" + WiFi.SSID(i) + "\">" + ((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*") + WiFi.SSID(i) + "(RSSI:" + WiFi.RSSI(i) + ")</option>";
		}
  }
  DYWIFICONFIG_DEBUG_PRINTLN("");
  delay(100);
}


 void DYWiFiConfig::setupWeb() {
   _taskStartTime = millis();
   DYWIFICONFIG_DEBUG_PRINTLN("");
   DYWIFICONFIG_DEBUG_PRINTLN(":WiFi connected");
   DYWIFICONFIG_DEBUG_PRINT(":Local IP: ");
   DYWIFICONFIG_DEBUG_PRINTLN(WiFi.localIP());
   DYWIFICONFIG_DEBUG_PRINT(":SoftAP IP: ");
   DYWIFICONFIG_DEBUG_PRINTLN(WiFi.softAPIP());
   createWebServer();
   // Start the server
   _server->begin();
   DYWIFICONFIG_DEBUG_PRINTLN("DYWEB:Server started");
 }

 void DYWiFiConfig::createWebServer() {
 	String _websettingpath = _webbase + "setting";
 	String _webreconnectpath = _webbase + "reconnect";
    _server->on(_webbase.c_str(), std::bind(&DYWiFiConfig::pageOfAdmin, this));
    _server->on(_websettingpath.c_str(),std::bind(&DYWiFiConfig::pageOfSetting, this));
    _server->on(_webreconnectpath.c_str(), std::bind(&DYWiFiConfig::pageOfReconnect, this));
	DYWIFICONFIG_DEBUG_PRINT(":webbase:");
	DYWIFICONFIG_DEBUG_PRINTLN(_webbase);
	DYWIFICONFIG_DEBUG_PRINT(":setting:");
	DYWIFICONFIG_DEBUG_PRINTLN(_websettingpath);
	DYWIFICONFIG_DEBUG_PRINT(":reconnect:");
	DYWIFICONFIG_DEBUG_PRINTLN(_webreconnectpath);
 }

 void DYWiFiConfig::pageOfAdmin() {
         String _wifiSettingPage = FPSTR(PAGE_IndexPage);
         IPAddress aip = WiFi.softAPIP();
         String aipStr = String(aip[0]) + '.' + String(aip[1]) + '.' + String(aip[2]) + '.' + String(aip[3]);
         IPAddress sip = WiFi.localIP();
         String sipStr = String(sip[0]) + '.' + String(sip[1]) + '.' + String(sip[2]) + '.' + String(sip[3]);
         IPAddress sgw = WiFi.gatewayIP();
         String sgwStr = String(sgw[0]) + '.' + String(sgw[1]) + '.' + String(sgw[2]) + '.' + String(sgw[3]);
         IPAddress sdns1 = WiFi.dnsIP(0);
         String sdns1Str = String(sdns1[0]) + '.' + String(sdns1[1]) + '.' + String(sdns1[2]) + '.' + String(sdns1[3]);
 		//apply template
         _wifiSettingPage.replace("{A-IP}",aipStr);
         //My Setting
         _wifiSettingPage.replace("{S-SSID}",String(this->_dws.SSID));
         _wifiSettingPage.replace("{S-PWD}",String(this->_dws.SSID_PASSWORD));

         if (_dws.DHCPAUTO == 1) {
 			       _wifiSettingPage.replace("{S-DHCP}",String("Auto"));
 		}else {
 			   _wifiSettingPage.replace("{S-DHCP}",String("Static"));
 		}
         _wifiSettingPage.replace("{S-IP}",sipStr);
         _wifiSettingPage.replace("{S-GW}",sgwStr);
         _wifiSettingPage.replace("{S-DNS}",sdns1Str);
         if (WiFi.status() == WL_CONNECTED) {
 			_wifiSettingPage.replace("{S-STATUS}","CONNECTED");
 		}else {
 			_wifiSettingPage.replace("{S-STATUS}","DISCONNECTED");
 		}
 		//Change Setting
         _wifiSettingPage.replace("{C-SSIDOPT}",_scanAPsWebOptionCache);


         DYWIFICONFIG_DEBUG_PRINTLN(_server->uri());
         _server->send(200, "text/html", _wifiSettingPage);
 }

 void DYWiFiConfig::pageOfSetting() {
		//
 		int count = _server->args();
 		if (count != 20) {
       String _wifiSettingPage = FPSTR(PAGE_404Page);
       DYWIFICONFIG_DEBUG_PRINTLN("Sending 404");
       _server->send(200, "text/html", _wifiSettingPage);
       return;
 		}
		DYWIFICONFIG_DEBUG_PRINTLN(":pageOfSetting");
 		memset(_dws.SSID,0,33);
        strcpy(_dws.SSID,_server->arg("ssid").c_str());
        memset(_dws.SSID_PASSWORD,0,33);
 		strcpy(_dws.SSID_PASSWORD,_server->arg("pass").c_str());

 		_dws.DHCPAUTO = (byte)_server->arg("dhcpAuto").toInt();
 		_dws.IP[0] = (byte)_server->arg("ip1").toInt();
 		_dws.IP[1] = (byte)_server->arg("ip2").toInt();
 		_dws.IP[2] = (byte)_server->arg("ip3").toInt();
 		_dws.IP[3] = (byte)_server->arg("ip4").toInt();
 		_dws.GW[0] = (byte)_server->arg("gw1").toInt();
 		_dws.GW[1] = (byte)_server->arg("gw2").toInt();
 		_dws.GW[2] = (byte)_server->arg("gw3").toInt();
 		_dws.GW[3] = (byte)_server->arg("gw4").toInt();
 		_dws.SNET[0] = (byte)_server->arg("sn1").toInt();
 		_dws.SNET[1] = (byte)_server->arg("sn2").toInt();
 		_dws.SNET[2] = (byte)_server->arg("sn3").toInt();
 		_dws.SNET[3] = (byte)_server->arg("sn4").toInt();
 		_dws.DNS[0] = (byte)_server->arg("dns1").toInt();
 		_dws.DNS[1] = (byte)_server->arg("dns2").toInt();
 		_dws.DNS[2] = (byte)_server->arg("dns3").toInt();
 		_dws.DNS[3] = (byte)_server->arg("dns4").toInt();
 		_taskState = DYWIFI_STATE_DISCONNECT;
 		_nextTaskState = DYWIFI_STATE_REDISCONNECT;
 		_storeconfig.commit();
 		_storeconfig.read();
 		sendHtmlPageWithRedirectByTimer(_webbase,"done");
 }

 void DYWiFiConfig::pageOfReconnect() {
		//
 		int count = _server->args();
 		if (count == 1){
 			String reconnectStr = _server->arg("reconnect");
 			if (reconnectStr.indexOf("1") > -1) {
         _wifiReconnectCount = 0;
         _taskState = DYWIFI_STATE_DISCONNECT;
         _nextTaskState = DYWIFI_STATE_REDISCONNECT;
         sendHtmlPageWithRedirectByTimer(_webbase,"reconnect");
         return;
 			}
 		}else {
       String _wifiSettingPage = FPSTR(PAGE_404Page);
       DYWIFICONFIG_DEBUG_PRINTLN("Sending 404");
       _server->send(200, "text/html", _wifiSettingPage);
     }
 }
 void DYWiFiConfig::sendHtmlPageWithRedirectByTimer(String gotoUrl,String Message) {
 	String _redirectContent  = FPSTR(PAGE_RedirectPage);
 	_redirectContent.replace("{R-DIRECT}",gotoUrl);
 	_redirectContent.replace("{R-MSG}",Message);
 	_server->send(200, "text/html", _redirectContent);
 }

 int DYWiFiConfig::setWifi(String essid, String epassword) {
   //0=fail,1=success,2=none
   int c = 0;
   if (essid.length()<=0) {
 	  return 0;
   }

   if (WiFi.status() == WL_CONNECTED) {
 	  if (WiFi.SSID().indexOf(essid) >=0) {
 		  return 2;
 	  }
   }
 	//
 	if ((epassword == "") || epassword.length() <=0) {
 		WiFi.begin(essid.c_str(), NULL);
 	}else {
 		WiFi.begin(essid.c_str(), epassword.c_str());
 	}
   DYWIFICONFIG_DEBUG_PRINTLN("DYWEB:Connect to AP");
   while ( c < 30 ) {
     if (WiFi.status() == WL_CONNECTED) {
         IPAddress sip = WiFi.localIP();
         String sipStr = String(sip[0]) + '.' + String(sip[1]) + '.' + String(sip[2]) + '.' + String(sip[3]);
         DYWIFICONFIG_DEBUG_PRINTLN("");
         DYWIFICONFIG_DEBUG_PRINTLN(sipStr);
 		return 1;
 	}
     delay(500);
     DYWIFICONFIG_DEBUG_PRINT(WiFi.status());
     c++;
   }
   DYWIFICONFIG_DEBUG_PRINTLN("");
   DYWIFICONFIG_DEBUG_PRINTLN("DYWEB:Connection is timed out");
   return 0;
 }

 bool DYWiFiConfig::setDHCP(byte isAuto) {
 	if (isAuto == 1) {
 		WiFi.config(IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0));
 		DYWIFICONFIG_DEBUG_PRINTLN(":DHCP IP");
 	}else {
 		WiFi.config(IPAddress(_dws.IP[0],_dws.IP[1],_dws.IP[2],_dws.IP[3]),IPAddress(_dws.GW[0],_dws.GW[1],_dws.GW[2],_dws.GW[3]),IPAddress(_dws.SNET[0],_dws.SNET[1],_dws.SNET[2],_dws.SNET[3]),IPAddress(_dws.DNS[0],_dws.DNS[1],_dws.DNS[2],_dws.DNS[3]),IPAddress(_dws.DNS[0],_dws.DNS[1],_dws.DNS[2],_dws.DNS[3]));
 		DYWIFICONFIG_DEBUG_PRINTLN(":MANUAL IP");
 	}
 }

 bool DYWiFiConfig::autoConnectToAP() {
 	if (_wifiReconnectCount > 3) {
 		DYWIFICONFIG_DEBUG_PRINTLN(":stop connect (3)");
 		return false;
 	}
   int state = setWifi(String(_dws.SSID), String(_dws.SSID_PASSWORD));
   //0=fail,1=success,2=none
 	if (state == 1) {
     //configurePrints(ws);
 		setDHCP(_dws.DHCPAUTO);
 		if (!MDNS.begin(_apname.c_str())) {
 			DYWIFICONFIG_DEBUG_PRINTLN(":mDNS fail");
 		}else {
 			DYWIFICONFIG_DEBUG_PRINTLN(":mDNS started");
 		}
     _wifiReconnectCount = 0;
 		return true;
 	} else if(state == 0){
 		_wifiReconnectCount++;
 		if (_wifiReconnectCount > 3) {
 			WiFi.disconnect();
 		}else {
       _nextTaskState = DYWIFI_STATE_REDISCONNECT;
		}
 		return false;
 	}
   return true;
 }
template <class T> int DYWiFiConfig::read(int address, T &data) {
	return _storeconfig.read(address + 200, *data);
}

template <class T> int DYWiFiConfig::write(int address, const T &data) {
	return _storeconfig.write(address + 200, *data);

}

void DYWiFiConfig::commit() {
	_storeconfig.commit();
}
