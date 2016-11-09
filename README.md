# DYWebConfig For Arduino ESP8266

The sample code is very simple to config wifi connection for Arduino ESP8266.

# Feature
* save all settings to eeprom
* detect disconnect and connect and no need reboot ESP
* DHCP/Manual IP

# How to use
This will open AP and Station mode, you can turn on Wifi to connect `ACCESS_POINT_NAME` and browser `192.168.4.1` by WebConfig's port.

Ex:
ACCESS_POINT_NAME :`ESP-WEB-01`

open browser and input http://192.168.4.1.


# Setup
## Copy files
copy `Configure_EEPROM.ino` , `Configure_Server.ino` , `Configure_header.h` , `Configure_index.h` , `Configure_redirect.h` to your project folder.

## Integrate code

### Declare

```
	#define ACCESS_POINT_NAME "ESP-WEB-01"

```

To change `ACCESS_POINT_NAME` for yours default web port is  80.

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
		webConfigureWifiHandle();
	}
```

insert `'webConfigureWifiHandle();` to your Loop() function.

