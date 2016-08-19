String st;
String content;

void webConfigureWifi() {
  EEPROM.begin(512);
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  WiFi.softAP(ACCESS_POINT_NAME, NULL);
  String esid = configureReadSSIDName();
  byte factory = configureReadStateForFactory();
  String epass = configureReadSSIDPassword();
  if ( factory == 1 ) {
      if (waitWifi(esid, epass)) {
        setupWeb(0);
        return;
      }
  }
  //fail / into factory
  setupAP();
  setupWeb(1);
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
  st = "<ol>";
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      st += "<li>";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
  st += "</ol>";
  delay(100);
}


void setupWeb(int webtype) {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer(webtype);
  // Start the server
  Configure_Server_P.begin();
  Serial.println("Server started");
}

void createWebServer(int webtype)
{
  if ( webtype == 1 ) {
    Configure_Server_P.on("/", []() {
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        content = "<!DOCTYPE HTML>\r\n<html>Configure Wifi";
        content += ipStr;
        content += "<p>";
        content += st;
        content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
        content += "</html>";
        Serial.println(Configure_Server_P.uri());
        Configure_Server_P.send(200, "text/html", content);
    });
    Configure_Server_P.on("/setting", []() {
        String qsid = Configure_Server_P.arg("ssid");
        String qpass = Configure_Server_P.arg("pass");
        if (qsid.length() > 0 && qpass.length() > 0) {
          Serial.println("clearing eeprom");
          for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
          Serial.println(qsid);
          Serial.println("");
          Serial.println(qpass);
          Serial.println("");
          //
          configureWriteSSIDName(qsid);
          configureWriteSSIDPassword(qpass);
          configureWriteStateForFactory(1);
          EEPROM.commit();
			sendHtmlPageWithRedirectByTimer("/","saved to eeprom and reset to boot into new wifi by self");
            ESP.restart();
        } else {
          content = "{\"Error\":\"404 not found\"}";
          statusCode = 404;
          Serial.println("Sending 404");
          Configure_Server_P.send(statusCode, "application/json", content);
        }
    });
  } else if (webtype == 0) {
    Configure_Server_P.on("/", []() {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      //Configure_Server_P.send(200, "application/json", "{" + "\"SSID\":\"" + WiFi.SSID() + "\"" + "\"IP\":\"" + ipStr + "\"" + "}");
        content = "<!DOCTYPE HTML>\r\n<html>Connect to Wifi";
        content += "<p>SSID:" + WiFi.SSID() + "</p>";
        content += "<p>IP:" + ipStr + "</p>";;
        content += "<p><a href='cleareeprom'>clear setting</a>";
        content += "</html>";
        Configure_Server_P.send(200, "text/html", content);
    });
    Configure_Server_P.on("/cleareeprom", []() {
			Serial.println("clearing eeprom");
			sendHtmlPageWithRedirectByTimer("/","Clearing the EEPROM and reset to boot");
      for (int i = 0; i < 96 + 5; ++i) {
		  EEPROM.write(i, 0);
	   }
		EEPROM.commit();
		ESP.restart();
    });
  }
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
  WiFi.begin(essid.c_str(), epassword.c_str());
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) { return true; }
    delay(500);
    Serial.print(WiFi.status());
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}
