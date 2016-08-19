

byte configureReadStateForFactory()
{
  Serial.println("Reading EEPROM Factory");
  byte factory = byte(EEPROM.read(0));
  Serial.print("Factory: ");
  Serial.println(factory);
  return factory;
}

void configureWriteStateForFactory(byte state)
{
	Serial.println("Writing EEPROM SSID:");
	EEPROM.write(0,state);
	Serial.print("Wrote: ");
	Serial.println(state);
}


void configureWriteSSIDName(String ssid)
{
	Serial.println("Writing EEPROM SSID:");
	int index = 5;
	for (int i = 0; i < ssid.length(); ++i)
	{
		EEPROM.write(i + index, ssid[i]);
		Serial.print("Wrote: ");
		Serial.println(ssid[i]);
	}
}

String configureReadSSIDName()
{
  String esid;
  int index = 5;
  Serial.println("Reading EEPROM SSID");
  for (int i = 0 + index; i < 32 + index; ++i)
    {
      esid += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  Serial.println(esid);
  return esid;
}

String configureReadSSIDPassword()
{
  int index = 5;
  Serial.println("Reading EEPROM SSID Password");
  String epass = "";
  for (int i = 32 + index; i < 96 + index; ++i)
    {
      epass += char(EEPROM.read(i));
    }
  Serial.print("SSID Password: ");
  Serial.println(epass);
  return epass;
}

void configureWriteSSIDPassword(String password)
{
	  Serial.println("Writing EEPROM SSID Password");
	  int index = 5;
	  for (int i = 0; i < password.length(); ++i)
		{
		  EEPROM.write(32+ i + index, password[i]);
		  Serial.print("Wrote: ");
		  Serial.println(password[i]);
		}
}
