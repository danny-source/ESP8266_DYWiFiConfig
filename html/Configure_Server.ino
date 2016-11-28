
#include <ESP8266WebServer.h>
#include "Configure_index.h"
#include "Configure_header.h"
#include "Configure_redirect.h"
#include <ESP8266mDNS.h>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif

#define DYWEB_DEBUG
#define DYWEB_DEBUG_SERIAL Serial
// If using something like Zero or Due, change the above to SerialUSB

// Define actual debug output functions when necessary.
#ifdef DYWEB_DEBUG
  #define DYWEB_DEBUG_PRINT(...) { DYWEB_DEBUG_SERIAL.print(__VA_ARGS__); }
  #define DYWEB_DEBUG_PRINTLN(...) { DYWEB_DEBUG_SERIAL.println(__VA_ARGS__); }
#else
  #define DYWEB_DEBUG_PRINT(...) {}
  #define DYWEB_DEBUG_PRINTLN(...) {}
#endif

#define STATE_DISCONNECT 1
#define STATE_REDISCONNECT 2

String _scanAPsWebOptionCache;
ESP8266WebServer Pserver(80);
#define Configure_Server_P Pserver
//external Configure_Server_P
WIFI_SETTINGS ws;
int WIFI_STATE_MACHINE = 0;
int _webWIFIReconnectCount = 0;
//
long _webTaskStartTime = 0;
long _webTaskEndTime = 0;
int _webTaskTimerCounter = 0;
int _webTaskState = 0;
int _webNextTaskState = 0;
long _webTask10SecondBase = 10;
long _webTask20SecondBase = 20;


void webConfigureTaskSchdule();
bool webConfigureAutoConnectToAP();
void webConfigureTaskSchdule01Second();
void webConfigureTaskSchdule10Second();
void webConfigureTaskSchdule20Second();
void webConfigureScanAPs(void);
void webConfigureSetupWeb();
void webConfigureCreateWebServer();
void sendHtmlPageWithRedirectByTimer(String gotoUrl,String Message);
int webConfigureSetWifi(String essid, String epassword);
bool webConfigureSetDHCP(byte isAuto);
bool webConfigureAutoConnectToAP();


void webConfigureWifi() {
configureBegin();
ws = getConfigureStruct();
//
WiFi.mode(WIFI_AP_STA);
WiFi.disconnect();
WiFi.softAP(ACCESS_POINT_NAME, NULL);
WIFI_STATE_MACHINE = 1;
webConfigureScanAPs();
webConfigureSetupWeb();
_webWIFIReconnectCount = 0;
_webTaskState = 0;
_webTaskStartTime = millis();
_webTask10SecondBase = 10;
_webTask20SecondBase = 20;
webConfigureAutoConnectToAP();
}

void webConfigureWifiHandle() {
	webConfigureTaskSchdule();
	Configure_Server_P.handleClient();
}

void webConfigureTaskSchdule()
{
 _webTaskEndTime = millis();
 long _webTaskTimeTemp = _webTaskEndTime - _webTaskStartTime;
	_webTaskTimeTemp = (long)(_webTaskTimeTemp / 1000);
  if ( _webTaskTimeTemp > 0) {
    _webTaskStartTime=_webTaskEndTime;
    _webTaskTimerCounter = _webTaskTimerCounter + _webTaskTimeTemp;
    //every 1 second
    webConfigureTaskSchdule01Second();
    //every 2 second
    if ( _webTaskTimerCounter >= _webTask10SecondBase) {
        _webTask10SecondBase += 10;
        webConfigureTaskSchdule10Second();
    }
    if ( _webTaskTimerCounter >= _webTask20SecondBase) {
        _webTask20SecondBase += 20;
        webConfigureTaskSchdule20Second();
    }
    //clear counter
    if (_webTaskTimerCounter >= 60) {
     _webTaskTimerCounter = 0;
     _webTask10SecondBase = 10;
     _webTask20SecondBase = 20;
    }
  }

}

void webConfigureTaskSchdule01Second() {
  //DYWEB_DEBUG_PRINTLN("DYWEB:01Sedond:");
  //DYWEB_DEBUG_PRINTLN(_webTaskTimerCounter,DEC);
  if (_webTaskState == 1) {
    DYWEB_DEBUG_PRINTLN("DYWEB:disconnect");
    WiFi.disconnect();
  }
  if (_webTaskState == 2) {
    DYWEB_DEBUG_PRINTLN("DYWEB:Reconnect");
    if (webConfigureAutoConnectToAP()) {
    }
  }
  _webTaskState = _webNextTaskState;
  _webNextTaskState = 0;
}

void webConfigureTaskSchdule10Second() {
    DYWEB_DEBUG_PRINTLN("DYWEB:10Sedond:");
    DYWEB_DEBUG_PRINTLN(_webTaskTimerCounter,DEC);
		webConfigureAutoConnectToAP();
}

void webConfigureTaskSchdule20Second() {
  DYWEB_DEBUG_PRINTLN("DYWEB:20Sedond:");
  DYWEB_DEBUG_PRINTLN(_webTaskTimerCounter,DEC);
		webConfigureScanAPs();
}

void webConfigureScanAPs(void) {
  delay(100);
  DYWEB_DEBUG_PRINTLN("DYWEB:Scan networks");
  int n = WiFi.scanNetworks();
  if (n == 0) {
    DYWEB_DEBUG_PRINTLN("DYWEB:no networks found");
  } else {
		_scanAPsWebOptionCache = "";
		DYWEB_DEBUG_PRINT("DYWEB:");
    DYWEB_DEBUG_PRINT(n);
    DYWEB_DEBUG_PRINTLN(" networks found");
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      /*
      DYWEB_DEBUG_PRINT(i + 1);
      DYWEB_DEBUG_PRINT(": ");
      DYWEB_DEBUG_PRINT(WiFi.SSID(i));
      DYWEB_DEBUG_PRINT(" (");
      DYWEB_DEBUG_PRINT(WiFi.RSSI(i));
      DYWEB_DEBUG_PRINT(")");
      DYWEB_DEBUG_PRINTLN((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      */
			//store APs for web Configure
			_scanAPsWebOptionCache +="<option value=\"" + WiFi.SSID(i) + "\">" + ((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*") + WiFi.SSID(i) + "(RSSI:" + WiFi.RSSI(i) + ")</option>";
      delay(10);
     }
  }
  DYWEB_DEBUG_PRINTLN("");
  delay(100);
}


void webConfigureSetupWeb() {
  _webTaskStartTime = millis();
  DYWEB_DEBUG_PRINTLN("");
  DYWEB_DEBUG_PRINTLN("DYWEB:WiFi connected");
  DYWEB_DEBUG_PRINT("DYWEB:Local IP: ");
  DYWEB_DEBUG_PRINTLN(WiFi.localIP());
  DYWEB_DEBUG_PRINT("DYWEB:SoftAP IP: ");
  DYWEB_DEBUG_PRINTLN(WiFi.softAPIP());
  webConfigureCreateWebServer();
  // Start the server
  Configure_Server_P.begin();
  DYWEB_DEBUG_PRINTLN("DYWEB:Server started");
}

void webConfigureCreateWebServer()
{

    Configure_Server_P.on("/", []() {
        String _webSettingPage = FPSTR(PAGE_IndexPage);
        IPAddress aip = WiFi.softAPIP();
        String aipStr = String(aip[0]) + '.' + String(aip[1]) + '.' + String(aip[2]) + '.' + String(aip[3]);
        IPAddress sip = WiFi.localIP();
        String sipStr = String(sip[0]) + '.' + String(sip[1]) + '.' + String(sip[2]) + '.' + String(sip[3]);
        IPAddress sgw = WiFi.gatewayIP();
        String sgwStr = String(sgw[0]) + '.' + String(sgw[1]) + '.' + String(sgw[2]) + '.' + String(sgw[3]);
        IPAddress sdns1 = WiFi.dnsIP(0);
        String sdns1Str = String(sdns1[0]) + '.' + String(sdns1[1]) + '.' + String(sdns1[2]) + '.' + String(sdns1[3]);
		//apply template
        _webSettingPage.replace("{A-IP}",aipStr);
        //My Setting
        _webSettingPage.replace("{S-SSID}",ws.SSID);
        _webSettingPage.replace("{S-PWD}",ws.SSID_PASSWORD);

        if (ws.DHCPAUTO == 1) {
			       _webSettingPage.replace("{S-DHCP}",String("Auto"));
		}else {
			   _webSettingPage.replace("{S-DHCP}",String("Static"));
		}
        _webSettingPage.replace("{S-IP}",sipStr);
        _webSettingPage.replace("{S-GW}",sgwStr);
        _webSettingPage.replace("{S-DNS}",sdns1Str);
        if (WiFi.status() == WL_CONNECTED) {
			_webSettingPage.replace("{S-STATUS}","CONNECTED");
		}else {
			_webSettingPage.replace("{S-STATUS}","DISCONNECTED");
		}
		//Change Setting
        _webSettingPage.replace("{C-SSIDOPT}",_scanAPsWebOptionCache);


        DYWEB_DEBUG_PRINTLN(Configure_Server_P.uri());
        Configure_Server_P.send(200, "text/html", _webSettingPage);
    });
    Configure_Server_P.on("/setting", []() {
		//
		int count = Configure_Server_P.args();
		if (count != 20) {
      String _webSettingPage = FPSTR(PAGE_404Page);
      DYWEB_DEBUG_PRINTLN("Sending 404");
      Configure_Server_P.send(200, "text/html", _webSettingPage);
      return;
		}

		memset(ws.SSID,0,33);
        strcpy(ws.SSID,Configure_Server_P.arg("ssid").c_str());
        memset(ws.SSID_PASSWORD,0,33);
		strcpy(ws.SSID_PASSWORD,Configure_Server_P.arg("pass").c_str());

		ws.DHCPAUTO = (byte)Configure_Server_P.arg("dhcpAuto").toInt();
		ws.IP[0] = (byte)Configure_Server_P.arg("ip1").toInt();
		ws.IP[1] = (byte)Configure_Server_P.arg("ip2").toInt();
		ws.IP[2] = (byte)Configure_Server_P.arg("ip3").toInt();
		ws.IP[3] = (byte)Configure_Server_P.arg("ip4").toInt();
		ws.GW[0] = (byte)Configure_Server_P.arg("gw1").toInt();
		ws.GW[1] = (byte)Configure_Server_P.arg("gw2").toInt();
		ws.GW[2] = (byte)Configure_Server_P.arg("gw3").toInt();
		ws.GW[3] = (byte)Configure_Server_P.arg("gw4").toInt();
		ws.SNET[0] = (byte)Configure_Server_P.arg("sn1").toInt();
		ws.SNET[1] = (byte)Configure_Server_P.arg("sn2").toInt();
		ws.SNET[2] = (byte)Configure_Server_P.arg("sn3").toInt();
		ws.SNET[3] = (byte)Configure_Server_P.arg("sn4").toInt();
		ws.DNS[0] = (byte)Configure_Server_P.arg("dns1").toInt();
		ws.DNS[1] = (byte)Configure_Server_P.arg("dns2").toInt();
		ws.DNS[2] = (byte)Configure_Server_P.arg("dns3").toInt();
		ws.DNS[3] = (byte)Configure_Server_P.arg("dns4").toInt();
		configureStruct(ws);
		configureCommit();
    _webTaskState = STATE_DISCONNECT;
    _webNextTaskState = STATE_REDISCONNECT;
		sendHtmlPageWithRedirectByTimer("/","done");
    });
    Configure_Server_P.on("/reconnect", []() {
		//
		int count = Configure_Server_P.args();
		if (count == 1){
			String reconnectStr = Configure_Server_P.arg("reconnect");
			if (reconnectStr.indexOf("1") > -1) {
        _webWIFIReconnectCount = 0;
        _webTaskState = STATE_DISCONNECT;
        _webNextTaskState = STATE_REDISCONNECT;
        sendHtmlPageWithRedirectByTimer("/","reconnect");
        return;
			}
		}else {
      String _webSettingPage = FPSTR(PAGE_404Page);
      DYWEB_DEBUG_PRINTLN("Sending 404");
      Configure_Server_P.send(200, "text/html", _webSettingPage);
    }
    });
}

void sendHtmlPageWithRedirectByTimer(String gotoUrl,String Message)
{
	String _redirectContent  = FPSTR(PAGE_RedirectPage);
  _redirectContent.replace("{R-DIRECT}",gotoUrl);
  _redirectContent.replace("{R-MSG}",Message);
	Configure_Server_P.send(200, "text/html", _redirectContent);
}

int webConfigureSetWifi(String essid, String epassword) {
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
  DYWEB_DEBUG_PRINTLN("DYWEB:Connect to AP");
  while ( c < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
        IPAddress sip = WiFi.localIP();
        String sipStr = String(sip[0]) + '.' + String(sip[1]) + '.' + String(sip[2]) + '.' + String(sip[3]);
        DYWEB_DEBUG_PRINTLN("");
        DYWEB_DEBUG_PRINTLN(sipStr);
		return 1;
	}
    delay(500);
    DYWEB_DEBUG_PRINT(WiFi.status());
    c++;
  }
  DYWEB_DEBUG_PRINTLN("");
  DYWEB_DEBUG_PRINTLN("DYWEB:Connection is timed out");
  return 0;
}

bool setDHCP(byte isAuto) {
	if (isAuto == 1) {
		WiFi.config(IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0));
		DYWEB_DEBUG_PRINTLN("DYWEB:DHCP IP");
	}else {
		WiFi.config(IPAddress(ws.IP[0],ws.IP[1],ws.IP[2],ws.IP[3]),IPAddress(ws.GW[0],ws.GW[1],ws.GW[2],ws.GW[3]),IPAddress(ws.SNET[0],ws.SNET[1],ws.SNET[2],ws.SNET[3]),IPAddress(ws.DNS[0],ws.DNS[1],ws.DNS[2],ws.DNS[3]),IPAddress(ws.DNS[0],ws.DNS[1],ws.DNS[2],ws.DNS[3]));
		DYWEB_DEBUG_PRINTLN("DYWEB:MANUAL IP");
	}
}

bool autoConnectToAP() {
	if (_webWIFIReconnectCount > 3) {
    DYWEB_DEBUG_PRINTLN("DYWEB:stop connect (3)");
		return false;
	}
  int state = setWifi(String(ws.SSID), String(ws.SSID_PASSWORD));
  //0=fail,1=success,2=none
	if (state == 1) {
    //configurePrints(ws);
		setDHCP(ws.DHCPAUTO);
		if (!MDNS.begin(ACCESS_POINT_NAME)) {
			DYWEB_DEBUG_PRINTLN("DYWEB:mDNS fail");
		}else {
			DYWEB_DEBUG_PRINTLN("DYWEB:mDNS started");
		}
    _WIFIReconnectCount = 0;
		return true;
	} else if(state == 0){
		_WIFIReconnectCount++;
		if (_webWIFIReconnectCount > 3) {
			WiFi.disconnect();
		}else {
      _nextTaskState = STATE_REDISCONNECT;
    }
		return false;
	}
  return true;
}
