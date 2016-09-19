
#include <ESP8266WebServer.h>
#include "Configure_index.h"
#include "Configure_header.h"
#include <ESP8266mDNS.h>

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
}
#endif


String st;
String content;
ESP8266WebServer Pserver(80);
#define Configure_Server_P Pserver
//external Configure_Server_P
WIFI_SETTINGS ws;
int WIFI_STATE_MACHINE = 0;
unsigned long WIFI_REFRESH_SCANAPS_STAMP = 0;
#define WIFI_REFRESH_SCANAPS_INTERVAL 20000

void webConfigureWifi() {
configureBegin();
ws = getConfigureStruct();
//
WiFi.mode(WIFI_AP_STA);
WiFi.disconnect();
WiFi.softAP(ACCESS_POINT_NAME, NULL);
WIFI_STATE_MACHINE = 1;

scanAPs();
setupWeb();
WIFI_REFRESH_SCANAPS_STAMP = millis();
}

void webConfigureWifiHandle() {
	if (WIFI_STATE_MACHINE == 1) {
		//
		configurePrints(ws);
		if (setWifi(String(ws.SSID), String(ws.SSID_PASSWORD))) {
			setDHCP(ws.DHCPAUTO);
			WIFI_STATE_MACHINE = 0;
			if (!MDNS.begin(ACCESS_POINT_NAME)) {
				Serial.println("Error setting up MDNS responder!");
			}else {
				Serial.println("mDNS responder started");
			}
		}
		WIFI_STATE_MACHINE = 0;
	}

	if ((millis() - WIFI_REFRESH_SCANAPS_STAMP) > WIFI_REFRESH_SCANAPS_INTERVAL) {
		WIFI_REFRESH_SCANAPS_STAMP = millis();
		scanAPs();
	}

	Configure_Server_P.handleClient();
}

void scanAPs(void) {
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  }
  Serial.println("");
  st = "";

  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      st +="<option value=\"" + WiFi.SSID(i) + "\">" + ((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*") + WiFi.SSID(i) + "(RSSI:" + WiFi.RSSI(i) + ")</option>";
    }

  delay(100);
}


void setupWeb() {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  Configure_Server_P.begin();
  Serial.println("Server started");
}

void createWebServer()
{

    Configure_Server_P.on("/", []() {
        IPAddress aip = WiFi.softAPIP();
        String aipStr = String(aip[0]) + '.' + String(aip[1]) + '.' + String(aip[2]) + '.' + String(aip[3]);
        IPAddress sip = WiFi.localIP();
        String sipStr = String(sip[0]) + '.' + String(sip[1]) + '.' + String(sip[2]) + '.' + String(sip[3]);
        IPAddress sgw = WiFi.gatewayIP();
        String sgwStr = String(sgw[0]) + '.' + String(sgw[1]) + '.' + String(sgw[2]) + '.' + String(sgw[3]);
        IPAddress sdns1 = WiFi.dnsIP(0);
        String sdns1Str = String(sdns1[0]) + '.' + String(sdns1[1]) + '.' + String(sdns1[2]) + '.' + String(sdns1[3]);
		//apply template
        content = PAGE_IndexPage;
        content.replace("{A-IP}",aipStr);
        //My Setting
        content.replace("{S-SSID}",ws.SSID);
        content.replace("{S-PWD}",ws.SSID_PASSWORD);

        if (ws.DHCPAUTO == 1) {
			content.replace("{S-DHCP}",String("Auto"));
		}else {
			content.replace("{S-DHCP}",String("Static"));
		}
        content.replace("{S-IP}",sipStr);
        content.replace("{S-GW}",sgwStr);
        content.replace("{S-DNS}",sdns1Str);
        if (WiFi.status() == WL_CONNECTED) {
			content.replace("{S-STATUS}","CONNECTED");
		}else {
			content.replace("{S-STATUS}","DISCONNECTED");
		}
		//Change Setting
        content.replace("{C-SSIDOPT}",st);


        Serial.println(Configure_Server_P.uri());
        Configure_Server_P.send(200, "text/html", content);
    });
    Configure_Server_P.on("/setting", []() {
		//
		int count = Configure_Server_P.args();
		if (count != 20) {
          content = "{\"Error\":\"404 not found\"}";
          statusCode = 404;
          Serial.println("Sending 404");
          Configure_Server_P.send(statusCode, "application/json", content);
		}

        strcpy(ws.SSID,Configure_Server_P.arg("ssid").c_str());
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
		WIFI_STATE_MACHINE = 1;
		sendHtmlPageWithRedirectByTimer("/","done");
    });
    Configure_Server_P.on("/reconnect", []() {
		//
		int count = Configure_Server_P.args();
		if (count != 1){
			String reconnectStr = Configure_Server_P.arg("reconnect");
			if (reconnectStr.indexOf("1") == -1) {
				content = "{\"Error\":\"404 not found\"}";
				statusCode = 404;
				Serial.println("Sending 404");
				Configure_Server_P.send(statusCode, "application/json", content);
			}
		}
		WIFI_STATE_MACHINE = 1;
		sendHtmlPageWithRedirectByTimer("/","reconnect");

    });
}

void sendHtmlPageWithRedirectByTimer(String gotoUrl,String Message)
{
	content  = "<script type=\"text/javascript\">";
	content += "var count = 15;";
	content += "var redirect = \"" + gotoUrl +"\";";
	content += "function countDown(){";
	content += "var timer = document.getElementById(\"timer\");";
	content += "if(count > 0){";
	content += "count--;";
	content += "timer.innerHTML = \"This page will redirect in \"+count+\" seconds.\";";
	content += "setTimeout(\"countDown()\", 1000);";
	content += "}else{";
	content += "window.location.href = redirect;";
	content += "}";
	content += "}";
	content += "</script>";
	content += Message;
	content += "<br>";
	content += "<span id=\"timer\">";
	content += "<script type=\"text/javascript\">countDown();</script>";
	content += "</span>";
	Configure_Server_P.send(200, "text/html", content);
}

bool setWifi(String essid, String epassword) {
  int c = 0;
  if ((essid.length()<=0) || (epassword.length()<=0)) {
	  return false;
  }

  if (WiFi.status() == WL_CONNECTED) {
	  if (WiFi.SSID().indexOf(essid) >=0) {
		  return true;
	  }
  }

  WiFi.begin(essid.c_str(), epassword.c_str());
  Serial.println("Waiting for Wifi to connect");
  while ( c < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
        IPAddress sip = WiFi.localIP();
        String sipStr = String(sip[0]) + '.' + String(sip[1]) + '.' + String(sip[2]) + '.' + String(sip[3]);
        Serial.println("");
        Serial.println(sipStr);
		return true;
	}
    delay(500);
    Serial.print(WiFi.status());
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}

bool setDHCP(byte isAuto) {
	if (isAuto == 1) {
		WiFi.config(IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0));
	}else {
		WiFi.config(IPAddress(ws.IP[0],ws.IP[1],ws.IP[2],ws.IP[3]),IPAddress(ws.GW[0],ws.GW[1],ws.GW[2],ws.GW[3]),IPAddress(ws.SNET[0],ws.SNET[1],ws.SNET[2],ws.SNET[3]),IPAddress(ws.DNS[0],ws.DNS[1],ws.DNS[2],ws.DNS[3]),IPAddress(ws.DNS[0],ws.DNS[1],ws.DNS[2],ws.DNS[3]));
	}
	Serial.println("SET DHCP");
}
