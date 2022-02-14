/*
 * This example shows, how to use the PushDisConnector API, which connects an wifi capable device to a smartphone over the internet. 
 * You need to install the PushDis app on your smartphone. Within the app you can create a PushDis connector, which is identified 
 * by a unique code (a String of 8 alphanumerical characters). 
 * 
 * We show, how to transmit the data measured by a temperature sensor to the app on the smartphone. Additionally, it will create a 
 * alert on the smartphone, if the temperature increases 20 degrees.
 * 
 */

#include <ESP8266WiFi.h>
#include <PushDis.h>

// PushDis connector code
const String pushDisCode = "XXXXXXXX";

// The device name
const String myName = "Temperature Sensor Example";

// Create a connector based on an HTTPS-capable wifiClient
WiFiClientSecure wifiClient;
PushDisConnector pdc(wifiClient, pushDisCode, myName);

// additional parameters to control the alerting logic
const float temperatureLimit = 20.0;
int millisLastAlert = -900000;

void setup() {
  Serial.begin(115200);
  delay(1000);

  doWifiSetup();

  // connects to the PushDis App. 
  pdc.connect();

  // You need to open the connector in the PushDis App. It will show an incoming connection from a device called 
  // like the given "myName" parameter and asks for approval. Approve the connection in the App to finish the initial setup.
  
}

void loop() {

    // read value from a TMP36 sensor (3.3v) and calculate temperature
    int sensorVal = analogRead(A0);
    float voltage = (sensorVal / 1024.0) * 3.3;
    float temperature = (voltage - 0.5) * 100;    

    // definition of the parameters, which should be displayed in the smartphone
    DisplayParameter p1 {"Sensor value", String(sensorVal)};
    DisplayParameter p2 {"Voltage", String(voltage)};
    DisplayParameter p3 {"Degrees C", String(temperature)};

    // send the parameters together with a display title. They will appear on the smartphone.
    pdc.pushParams("Temperature sensor", {p1, p2, p3});

    // specify, when an alert should be fired
    if (temperature > temperatureLimit and millis() - millisLastAlert > 900000) {
      millisLastAlert = millis();

      // send alert to the smartphone, it consists of a title and a short body message
      pdc.pushAlert("Temperature Alert", "Temperature exceeds warning limit!");
    }

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
