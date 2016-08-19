# DYWebConfig For Arduino ESP8266

The sample code is very simple to config wifi connection for Arduino ESP8266.

# How to use
This will open AP and Station mode, you can turn on Wifi to connect `ACCESS_POINT_NAME` and browser `192.168.4.1` by WebConfig's port.

Ex:
ACCESS_POINT_NAME :`ESP-WEB-01`
web port : `80`.

open browser and input http://192.168.4.1.


# Setup
## Copy files
copy `Configure_EEPROM.ino` and `Configure_Server.ino` to your project folder.

## Integrate code

### Declare

```
	ESP8266WebServer Pserver(80);
	#define Configure_Server_P Pserver
	#define ACCESS_POINT_NAME "ESP-WEB-01"

```

To change `ACCESS_POINT_NAME` for yours and setup web port. this 80 is port that declare of `Pserver(80)`.

### Setup()

```
	void setup() {
		:
		:
		:
		webConfigureWifi();
	}
```

### Loop()

```
	void loop() {
		Configure_Server_P.handleClient();
	}
```

insert `'Configure_Server_P.handleClient()` to your Loop() function.

