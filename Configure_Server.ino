
#include <ESP8266WebServer.h>
#include "Configure_index.h"
#include "Configure_header.h"

String st;
String content;
ESP8266WebServer Pserver(80);
#define Configure_Server_P Pserver
//external Configure_Server_P
WIFI_SETTINGS ws;

void webConfigureWifi() {
  configureBegin();
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  WiFi.softAP(ACCESS_POINT_NAME, NULL);
  String esid = configureReadSSIDName();
  byte factory = configureReadStateForFactory();
  String epass = configureReadSSIDPassword();
  if ( factory == 1 ) {
      if (waitWifi(esid, epass)) {
      }
  }
	setupAP();
	setupWeb();
	ws  = getConfigureStruct();
}

void webConfigureWifiHandle() {
	Configure_Server_P.handleClient();
}

void setupAP(void) {
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
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        IPAddress lip = WiFi.localIP();
        String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
        IPAddress gwip = WiFi.gatewayIP();
        String gwStr = String(gwip[0]) + '.' + String(gwip[1]) + '.' + String(gwip[2]) + '.' + String(gwip[3]);        
        
        content = PAGE_IndexPage;
        content.replace("{APIP}",ipStr);
        content.replace("{SIDOPT}",st);
        content.replace("{LIP}",lipStr);
        content.replace("{LGW}",gwStr);
        content.replace("{SI}",ws.SSID);
        content.replace("{SIP}",ws.SSID_PASSWORD);
        if (WiFi.status() == WL_CONNECTED) {
			content.replace("{STATE}","CONNECTED");
		}else {
			content.replace("{STATE}","DISCONNECTED");
		}

        
        Serial.println(Configure_Server_P.uri());
        Configure_Server_P.send(200, "text/html", content);
    });
    Configure_Server_P.on("/setting", []() {
        String qsid = Configure_Server_P.arg("ssid");
        String qpass = Configure_Server_P.arg("pass");
        if (qsid.length() > 0 && qpass.length() > 0) {
			Serial.println("clearing eeprom");
			configureClear();
			Serial.println(qsid);
			Serial.println("");
			Serial.println(qpass);
			Serial.println("");
          //
			configureWriteSSIDName(qsid);
			configureWriteSSIDPassword(qpass);
			configureWriteStateForFactory(1);
			configureCommit();

			sendHtmlPageWithRedirectByTimer("/","saved to eeprom and reset to boot into new wifi by self");
            ESP.restart();
        } else {
          content = "{\"Error\":\"404 not found\"}";
          statusCode = 404;
          Serial.println("Sending 404");
          Configure_Server_P.send(statusCode, "application/json", content);
        }
    });
    Configure_Server_P.on("/cleareeprom", []() {
			Serial.println("clearing eeprom");
			sendHtmlPageWithRedirectByTimer("/","Clearing the EEPROM and reset to boot");
		configureClear();
		configureCommit(ws);
		ESP.restart();
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

bool waitWifi(String essid, String epassword) {
  int c = 0;
  if ((essid.length()<=0) || (epassword.length()<=0)) {
	  return false;
  }
  WiFi.begin(essid.c_str(), epassword.c_str());
  Serial.println("Waiting for Wifi to connect");
  while ( c < 30 ) {
    if (WiFi.status() == WL_CONNECTED) { return true; }
    delay(500);
    Serial.print(WiFi.status());
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}
