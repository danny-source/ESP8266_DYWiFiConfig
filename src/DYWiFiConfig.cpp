
#include "DYWiFiConfig.h"

#define DYEEPRO_SIZE 512

DYWiFiConfig::DYWiFiConfig() {

}

void DYWiFiConfig::begin(ESP8266WebServer *server, const char *webPath) {
    wifiStateCB = 0;
    _defaultconfig = 0;
    _server = server;
    _storeconfig.begin(DYEEPRO_SIZE, 0, &_dws);
    _storeconfig.read();
    _webPath = _webPath + String(webPath);
    if (_webPath.lastIndexOf("/") == -1) {
        _webPath =  _webPath + "/";
    }
    _webReturnPath = String(_webPath);
    DYWIFICONFIG_DEBUG_PRINT(":1:webbase:");
    DYWIFICONFIG_DEBUG_PRINTLN(_webPath);
    disableAP();
    WiFi.disconnect();
    _apname = String("DYWiFi-") + String(ESP.getChipId());
    _appassword = "";
    _wifiStateMachine = 1;
    scanAPs();
    setupWeb();
    _wifiReconnectCount = 0;
    _taskState = 0;
    _taskStartTime = millis();
    _task10SecondBase = 10;
    _task20SecondBase = 20;
    _task40SecondBase = 40;
    _taskClearCounter = (int)(mathLCM(mathLCM(_task10SecondBase, _task20SecondBase), _task40SecondBase));
    _nextTaskState = DYWIFI_STATE_RECONNECT;
    _autoEnableAPPin = -1;
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
        //per 1 second
        taskSchdule01Second();
        //per 2 second
        if ( _taskTimerCounter >= _task10SecondBase) {
            _task10SecondBase += 10;
            taskSchdule10Second();
        }
        //per 20 second
        if ( _taskTimerCounter >= _task20SecondBase) {
            _task20SecondBase += 20;
            taskSchdule20Second();
        }
        //per 40 second
        if ( _taskTimerCounter >= _task40SecondBase) {
            _task40SecondBase += 40;
            taskSchdule40Second();
        }
        //clear counter
        if (_taskTimerCounter >= _taskClearCounter) {
            _taskTimerCounter = 0;
            _task10SecondBase = 10;
            _task20SecondBase = 20;
            _task40SecondBase = 40;
        }
    }

}

void DYWiFiConfig::setWifiStateCallback(DYWifiStateCallback cb) {
    wifiStateCB = cb;
}

void DYWiFiConfig::taskSchdule01Second() {
    //DYWEB_DEBUG_PRINTLN("DYWEB:01Sedond:");
    if (_taskState == DYWIFI_STATE_DISCONNECT) {
        DYWIFICONFIG_DEBUG_PRINTLN(":disconnect");
        _wifiReconnectCount = 0;
        WiFi.disconnect();
    }
    if (_taskState == DYWIFI_STATE_RECONNECT) {
        DYWIFICONFIG_DEBUG_PRINTLN(":Reconnect");
        if (autoConnectToAP()) {
        }
    }
    if (_autoEnableAPPin >= 0) {
        int state = digitalRead(_autoEnableAPPin);
        if (state == LOW) {
            enableAP();
        }

    }
    //
    int _s = status();
    if (_wifiStatus != _s) {
		printStatus();
        _wifiStatus = _s;
        if (_wifiStatus == DW_CONNECTED) {
            setDHCP(_dws.DHCPAUTO);
            if (!MDNS.begin(_apname.c_str())) {
                DYWIFICONFIG_DEBUG_PRINTLN(":mDNS fail");
            } else {
                DYWIFICONFIG_DEBUG_PRINTLN(":mDNS started");
            }
			DYWIFICONFIG_DEBUG_PRINT(":apname:");
			DYWIFICONFIG_DEBUG_PRINTLN(_apname);
			IPAddress sip = WiFi.localIP();
			String sipStr = String(sip[0]) + '.' + String(sip[1]) + '.' + String(sip[2]) + '.' + String(sip[3]);
			DYWIFICONFIG_DEBUG_PRINTLN("IP:");
			DYWIFICONFIG_DEBUG_PRINTLN(sipStr);
        } else {

        }
        if (wifiStateCB != NULL) {
            wifiStateCB (_wifiStatus);
        }
    }
    _taskState = _nextTaskState;
    _nextTaskState = 0;
}

void DYWiFiConfig::taskSchdule10Second() {
    DYWIFICONFIG_DEBUG_PRINT(":10Sedond:");
    DYWIFICONFIG_DEBUG_PRINTLN(_taskTimerCounter,DEC);
	if (_wifiReconnectCount <= 1) {
		DYWIFICONFIG_DEBUG_PRINTLN("autoConnectToAP");
		autoConnectToAP();
	}
}

void DYWiFiConfig::taskSchdule20Second() {
    DYWIFICONFIG_DEBUG_PRINT(":20Sedond:");
    DYWIFICONFIG_DEBUG_PRINTLN(_taskTimerCounter,DEC);
	if (_wifiReconnectCount >= 2) {
		DYWIFICONFIG_DEBUG_PRINTLN("autoConnectToAP");
		autoConnectToAP();
	}
}

void DYWiFiConfig::taskSchdule40Second() {
    DYWIFICONFIG_DEBUG_PRINT(":40Sedond:");
    DYWIFICONFIG_DEBUG_PRINTLN(_taskTimerCounter,DEC);
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
    String _websettingpath = _webPath + "setting";
    String _webreconnectpath = _webPath + "reconnect";
    _server->on(_webPath.c_str(), std::bind(&DYWiFiConfig::pageOfAdmin, this));
    _server->on(_websettingpath.c_str(),std::bind(&DYWiFiConfig::pageOfSetting, this));
    _server->on(_webreconnectpath.c_str(), std::bind(&DYWiFiConfig::pageOfReconnect, this));
    DYWIFICONFIG_DEBUG_PRINT(":webbase:");
    DYWIFICONFIG_DEBUG_PRINTLN(_webPath);
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
    //scan networks
    scanAPs();
    //apply template
    _wifiSettingPage.replace("{A-IP}",aipStr);
    //My Setting
    _wifiSettingPage.replace("{S-SSID}",String(this->_dws.SSID));
    _wifiSettingPage.replace("{S-PWD}",String(this->_dws.SSID_PASSWORD));

    if (_dws.DHCPAUTO == 1) {
        _wifiSettingPage.replace("{S-DHCP}",String("Auto"));
    } else {
        _wifiSettingPage.replace("{S-DHCP}",String("Static"));
    }
    _wifiSettingPage.replace("{S-IP}",sipStr);
    _wifiSettingPage.replace("{S-GW}",sgwStr);
    _wifiSettingPage.replace("{S-DNS}",sdns1Str);
    if (WiFi.status() == WL_CONNECTED) {
        _wifiSettingPage.replace("{S-STATUS}","CONNECTED");
    } else {
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
    _nextTaskState = DYWIFI_STATE_RECONNECT;
    _storeconfig.commit();
    _storeconfig.read();
    sendHtmlPageWithRedirectByTimer(_webReturnPath,"done");
}

void DYWiFiConfig::pageOfReconnect() {
    //
    int count = _server->args();
    if (count == 1) {
        String reconnectStr = _server->arg("reconnect");
        if (reconnectStr.indexOf("1") > -1) {
            _wifiReconnectCount = 0;
            _taskState = DYWIFI_STATE_DISCONNECT;
            _nextTaskState = DYWIFI_STATE_RECONNECT;
            sendHtmlPageWithRedirectByTimer(_webPath,"reconnect");
            return;
        }
    } else {
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
    //0=fail,1=success,2=connecting,3=none
    if (essid.length()<=0) {
        return 0;
    }
    if (status() == DW_CONNECTED) {
        if (WiFi.SSID().indexOf(essid) >=0) {
			_wifiReconnectCount = 0;
			if (_autoEnableAPPin >= 0) {
				disableAP();
			}
            return 1;
        }
    }
    //
    if (status() == DW_CONNECTING) {
		return 2;
	}

	if ((status() == DW_NO_SSID_AVAIL) || (status() == DW_CONNECT_FAILED)) {
		_wifiReconnectCount++;
        if (_wifiReconnectCount >= 1) {
            enableAP();
        }
	}

    if ((epassword == "") || epassword.length() <=0){
	    DYWIFICONFIG_DEBUG_PRINTLN("DYWEB:Connect to AP");
        WiFi.begin(essid.c_str(), NULL);
        return 2;
    } else {
	    DYWIFICONFIG_DEBUG_PRINTLN("DYWEB:Connect to AP");
        WiFi.begin(essid.c_str(), epassword.c_str());
        return 2;
    }
}

bool DYWiFiConfig::setDHCP(byte isAuto) {
    if (isAuto == 1) {
        WiFi.config(IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0));
        DYWIFICONFIG_DEBUG_PRINTLN(":DHCP IP");
    } else {
        WiFi.config(IPAddress(_dws.IP[0],_dws.IP[1],_dws.IP[2],_dws.IP[3]),IPAddress(_dws.GW[0],_dws.GW[1],_dws.GW[2],_dws.GW[3]),IPAddress(_dws.SNET[0],_dws.SNET[1],_dws.SNET[2],_dws.SNET[3]),IPAddress(_dws.DNS[0],_dws.DNS[1],_dws.DNS[2],_dws.DNS[3]),IPAddress(_dws.DNS[0],_dws.DNS[1],_dws.DNS[2],_dws.DNS[3]));
        DYWIFICONFIG_DEBUG_PRINTLN(":MANUAL IP");
    }
}

bool DYWiFiConfig::autoConnectToAP() {
    int state = setWifi(String(_dws.SSID), String(_dws.SSID_PASSWORD));
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

void DYWiFiConfig::enableAP(const char *name,const char *password) {
    setAP(name,password);
    enableAP();
}

void DYWiFiConfig::enableAP() {
    if (_appassword == "") {
        WiFi.softAP(_apname.c_str(), NULL);
    } else {
        WiFi.softAP(_apname.c_str(), _appassword.c_str());
    }
    //WiFi.mode(WIFI_AP_STA);
}

void DYWiFiConfig::disableAP() {
    WiFi.mode(WIFI_STA);
}

void DYWiFiConfig::autoEnableAP(int pin) {
    _autoEnableAPPin = pin;
    if (pin > 0) {
        pinMode(pin,INPUT_PULLUP);
    }
}

void DYWiFiConfig::setAP(const char *name,const char *password) {
    _apname = String(name);
    if (password == NULL) {
        _appassword = String("");
    } else {
        _appassword = String(password);
    }
}

void DYWiFiConfig::reConnect() {
    WiFi.disconnect();
    _nextTaskState = DYWIFI_STATE_RECONNECT;
}

void DYWiFiConfig::setWebReturnPath(const char *path) {
    _webReturnPath = String(path);
}

bool DYWiFiConfig::setDefaultConfig(DYWIFICONFIG_STRUCT s) {
    String cPrefix = String(_dws.SETTING_DATA_PREFIX);
    if ((cPrefix.indexOf(s.SETTING_DATA_PREFIX) <= -1) || (strlen(_dws.SSID) <=0)) {
        _storeconfig.commit(s);
        _storeconfig.read();
        return true;
    }
    return false;
}

DYWIFICONFIG_STRUCT DYWiFiConfig::createConfig() {
    DYWIFICONFIG_STRUCT s = {0};
    strcpy(s.SETTING_DATA_PREFIX,DEF_DYWIFICONFIG_PREFIX);
    s.NEED_FACTORY = 1;
    memset(s.SSID,0,33);
    memset(s.SSID_PASSWORD,0,33);
    memset(s.IP,0,4);
    memset(s.GW,0,4);
    memset(s.SNET,0,4);
    memset(s.DNS,0,4);
    s.DHCPAUTO = 1;
    return s;
}


int DYWiFiConfig::mathGCD(int m, int n) {
    while(n != 0) {
        int r = m % n;
        m = n;
        n = r;
    }
    return m;
}

int DYWiFiConfig::mathLCM(int m, int n) {
    return m * n / mathGCD(m, n);
}

dw_status_t DYWiFiConfig::status() {
    station_status_t status = wifi_station_get_connect_status();
	//1:connected
    switch(status) {
        case STATION_GOT_IP:
            return DW_CONNECTED;
        case STATION_NO_AP_FOUND:
            return DW_NO_SSID_AVAIL;
        case STATION_CONNECT_FAIL:
        case STATION_WRONG_PASSWORD:
            return DW_CONNECT_FAILED;
        case STATION_IDLE:
            return DW_IDLE_STATUS;
        case STATION_CONNECTING:
			return DW_CONNECTING;
        default:
            return DW_DISCONNECTED;
    }
}


void DYWiFiConfig::printStatus() {
	dw_status_t s = status();
	switch(s) {
    case DW_NO_SHIELD:
		DYWIFICONFIG_DEBUG_PRINTLN("DW_NO_SHIELD");
		return;
    case DW_IDLE_STATUS:
		DYWIFICONFIG_DEBUG_PRINTLN("DW_IDLE_STATUS");
		return;
    case DW_NO_SSID_AVAIL:
		DYWIFICONFIG_DEBUG_PRINTLN("DW_NO_SSID_AVAIL");
		return;
    case DW_SCAN_COMPLETED:
		DYWIFICONFIG_DEBUG_PRINTLN("DW_SCAN_COMPLETED");
		return;
    case DW_CONNECTED:
		DYWIFICONFIG_DEBUG_PRINTLN("DW_CONNECTED");
		return;
    case DW_CONNECT_FAILED:
		DYWIFICONFIG_DEBUG_PRINTLN("DW_CONNECT_FAILED");
		return;
    case DW_CONNECTION_LOST:
		DYWIFICONFIG_DEBUG_PRINTLN("DW_CONNECTION_LOST");
		return;
    case DW_DISCONNECTED:
		DYWIFICONFIG_DEBUG_PRINTLN("DW_DISCONNECTED");
		return;
    case DW_CONNECTING:
		DYWIFICONFIG_DEBUG_PRINTLN("DW_CONNECTING");
		return;
	default:
		return;
	}
}
