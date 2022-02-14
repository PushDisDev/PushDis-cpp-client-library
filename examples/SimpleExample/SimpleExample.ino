/*
 * This example shows, how to use the PushDisConnector API, which connects an wifi capable device to a smartphone over the internet. 
 * You need to install the PushDis app on your smartphone. Within the app you can create a PushDis connector, which is identified 
 * by a unique code (a String of 8 alphanumerical characters). 
 * 
 */

#include <ESP8266WiFi.h>
#include <PushDis.h>


// PushDis connector code
const String connectorCode = "XXXXXXXX";

// The device name
const String deviceName = "Very simple example";

WiFiClientSecure wifiClient;

void setup() {
  Serial.begin(115200);
  delay(1000);

  doWifiSetup();

  // Create a connector based on an HTTPS-capable wifiClient
  PushDisConnector pdc(wifiClient, connectorCode, deviceName);

  // connects to the PushDis App. 
  pdc.connect();

  // You need to open the connector in the PushDis App. It will show an incoming connection from a device called 
  // like the given "myName" parameter and asks for approval. Approve the connection in the App to finish the initial setup.

  // definition of the parameters, which should be displayed in the smartphone
  DisplayParameter p1 {"Param 1", "Value 1"};
  DisplayParameter p2 {"Param 2", "Value 2"};
  DisplayParameter p3 {"Param 3", "Value 3"};

  // send the parameters together with a display title to show them on the smartphone display.
  pdc.pushParams("Simple example", {p1, p2, p3});

  // send alert to the smartphone, it consists of a title and a short body message
  // alerts appear on the smartphone, no matter if the PushDis App is open or closed
  pdc.pushAlert("Alert example", "This is an example alert!");
  
}

void loop() {

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