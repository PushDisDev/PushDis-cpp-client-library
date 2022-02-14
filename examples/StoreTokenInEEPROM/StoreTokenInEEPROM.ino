/*
 * This example shows, how a PushDis connector survives a reboot by saving an API token in an EEPROM. 
 * 
 */

#include <ESP8266WiFi.h>
#include <PushDis.h>
#include <EEPROM.h>

// PushDis connector code
const String connectorCode = "XXXXXXXX";

// The device name
const String deviceName = "Very simple example";

WiFiClientSecure wifiClient;
PushDisConnector* pdc;

boolean blink = true;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(1000);

  doWifiSetup();

  if (existsTokenInEEPROM()) { 

    // if a token is available in EEPROM load it into a new PushDisConnector object
    Token* token = readTokenFromEEPROM();
    pdc = PushDisConnector::reconnect(wifiClient, connectorCode, *token);
 
    
  } else {
    
    // if there is no token in EEPROM, create a new PushDisConnector and do the initial connection
    pdc = new PushDisConnector(wifiClient, connectorCode, deviceName); 
    pdc->connect();

    // after the initial connect, the token is available and can be stored in the EEPROM
    writeTokenToEEPROM(pdc->getToken());
  
  }
  
}

void loop() {

  // blink simulation on the display of the smartphone
  DisplayParameter p;
  p.name = "Blink value";
  
  if (blink) {
    p.value = "***"; 
  } else {
    p.value = "";
  }
  
  pdc->pushParams("Blink", {p});
  
  blink = !blink;
  delay(5000);

}

void doWifiSetup() {

  // WiFi credentials
  char* ssid     = "wifi_ssid";
  char* password = "wifi_password";
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");  

  // suppress certificate fingerprint check, choose your own wifi security preferences
  wifiClient.setInsecure();
}

void writeTokenToEEPROM(Token* token)
{
  // store the token as String
  String s = token->toString();
  byte len = s.length();
  EEPROM.write(0, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(1 + i, s[i]);
  }
}

Token* readTokenFromEEPROM()
{
  
  // read token String from EEPROM and create a new Token object
  int newStrLen = EEPROM.read(0);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(1 + i);
  }
  data[newStrLen] = '\0'; 
  return new Token(String(data));
}

boolean existsTokenInEEPROM() {
  return (EEPROM.read(0) > 0);
}

void removeTokenFromEEPROM() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}
