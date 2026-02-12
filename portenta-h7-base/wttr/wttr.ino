#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include <ArduinoECCX08.h>
#include <ArduinoBearSSL.h>
#include <ArduinoHttpClient.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "arduino_secrets.h"
#include "ArduinoBearSSLConfig.h"
#include "trust_anchors.h"


// Network SSID (name).
char ssid[] = SECRET_SSID;
// Network WPA key.
char pass[] = SECRET_PASS;

// WiFi client.
WiFiClient client;
BearSSLClient sslClient(client, TAs, (size_t)TAs_NUM);

// Open weather map api key.
String apiKey = SECRET_APIKEY;

// Open wather map server name.
//char server[] = "api.met.no";
char server[] = "www.arduino.cc";
const uint16_t server_port = 443;

unsigned long getTime() {
  return WiFi.getTime();
}

void connectWiFi() {
    // initial WiFi status is IDLE:
    int status = WL_IDLE_STATUS;

    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true);
    }

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED) {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000);
    }

    Serial.println("Connected to wifi");
}

void getWeather() {
  Serial.println("\nStarting connection to server...");

  // if you get a connection, report back via serial:
  if (client.connectSSL(server, 443)) {
      Serial.println("connected to server");
      // make a HTTP request:
      client.print("GET /weatherapi/locationforecast/2.0/compact.json?");
      client.print("lat=48.7764953");
      client.print("lon=19.0985419");
      client.println("Host: api.met.no");
      client.println("Connection: close");
      client.println();
  } else {
      Serial.println("unable to connect");
  }

  delay(1000);
  String line = "";

  while (client.connected()) {
      line = client.readStringUntil('\n');

      //Serial.println(line);
      Serial.println("parsingValues");

      //create a json buffer where to store the json data
      DynamicJsonDocument doc(5000);

      DeserializationError error = deserializeJson(doc, line);
      if (error)
          Serial.println("parseObject() failed");
          return;
  }
}

void printWiFiStatus() {
    // print the SSID of the network attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print WiFi shield's IP address/mask:
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("IP Subnet: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway IP : ");
    Serial.println((IPAddress)WiFi.gatewayIP());

    // print the received signal strength:
    Serial.print("signal strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    String fv = WiFi.firmwareVersion();
    Serial.println(fv);
}

void scanNetworks() {
    // scan for nearby networks:
    byte numSsid = WiFi.scanNetworks();

    // print the list of networks seen:
    Serial.print("SSID List: ");
    Serial.println(numSsid);
    // print the network number and name for each network found:
    for (int thisNet = 0; thisNet<numSsid; thisNet++) {
        Serial.print(thisNet);
        Serial.print(") Network: ");
        Serial.println(WiFi.SSID(thisNet));
    }
}

void setup() {
    // initialize serial and wait for port to open:
    Serial.begin(9600);

    // scan for existing networks:
    scanNetworks();

    // Connect WiFi status:
    connectWiFi();

    // should be connected now, so print out the status:
    printWiFiStatus();
}

void loop() {
    // read weather via API:
    //getWeather();
    ArduinoBearSSL.getTime();

    Serial.println("\nStarting connection to server...");
    // if you get a connection, report back via serial:
    if (sslClient.connect(server, server_port)) {
        Serial.println("connected to server");
        // Make a HTTP request:
        sslClient.println("GET /search?q=arduino HTTP/1.1");
        sslClient.println("Host: www.google.com");
        sslClient.println("Connection: close");
        sslClient.println();
    } else {
        Serial.println("Failed to connect server");      
    }

    delay(100000);
}
