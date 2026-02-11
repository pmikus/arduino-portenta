/* 
 * Upload this sketch to a Host board to use it as an I2C bridge between 
 * Nicla Sense ME and the Arduino Cloud. 
 * Nicla Sense ME board needs to run the basic App.ino sketch and to be plugged
 * as a shield on top of the Host board.
 * 
 * Before uploading this sketch to the Host board:
 * - add your Host device to Arduino Cloud
 * - setup temperature and seconds Things in Arduino Cloud
 * - get the THING_ID and copy it in thingProperties.h
 * - create a Dashboard in Arduino Cloud, like a live chart of the temperature
*/

#include "thingProperties.h"

#include "Arduino.h"
#include "Arduino_BHY2Host.h"
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <ArduinoMDNS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>


// Set DEBUG to true in order to enable debug print.
#define DEBUG true
// Se BAUD rates.
#define BAUD_RATE_SERIAL_DEBUG  (115200 * 8)
#define BAUD_RATE_SERIAL_DATA   (115200 * 8)
#define NAME_TO_RESOLVE "portenta.local"


// Network SSID (name).
char ssid[] = SECRET_SSID;
// Network WPA key.
char pass[] = SECRET_PASS;

// WiFi client.
WiFiClient client;

// WiFi server.
WiFiServer server(80);

// mDNS
WiFiUDP udp;
MDNS mdns(udp);

// Nicla Sense ME sensors.
Sensor sensorTemp(SENSOR_ID_TEMP);
Sensor sensorHum(SENSOR_ID_HUM);
Sensor sensorBaro(SENSOR_ID_BARO);
Sensor sensorGas(SENSOR_ID_GAS);
SensorBSEC sensorBsec(SENSOR_ID_BSEC_LEGACY);

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
    Serial.println("Connected to WiFi");

    // Initialize the mDNS library. You can now reach or ping this
    // Arduino via the host name "arduino.local", provided that your operating
    // system is mDNS/Bonjour-enabled (such as macOS).
    // Always call this before any other method!
    mdns.begin(WiFi.localIP(), "arduino");

    // Now let's register the service we're offering (a web service) via Bonjour!
    // To do so, we call the addServiceRecord() method. The first argument is the
    // name of our service instance and its type, separated by a dot. In this
    // case, the service type is _http. There are many other service types, use
    // Google to look up some common ones, but you can also invent your own
    // service type, like _mycoolservice - As long as your clients know what to
    // look for, you're good to go.
    // The second argument is the port on which the service is running. This is
    // port 80 here, the standard HTTP port.
    // The last argument is the protocol type of the service, either TCP or UDP.
    // Of course, our service is a TCP service.
    // With the service registered, it will show up in a mDNS/Bonjour-enabled web
    // browser. As an example, if you are using Apple's Safari, you will now see
    // the service under Bookmarks -> Bonjour (Provided that you have enabled
    // Bonjour in the "Bookmarks" preferences in Safari).
    mdns.addServiceRecord("Arduino mDNS Webserver Example._http",
                          80,
                          MDNSServiceTCP);
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

void setup() {
    // initialize serial and wait for port to open:
    Serial.begin(9600);
  
    // defined in thingProperties.h:
    initProperties();

    // connect WiFi status:
    connectWiFi();

    // should be connected now, so print out the status:
    printWiFiStatus();

    // connect to Arduino IoT Cloud:
    ArduinoCloud.begin(ArduinoIoTPreferredConnection);
    ArduinoCloud.printDebugInfo();
    Serial.println("Connecting to the Arduino IoT Cloud");
    while (ArduinoCloud.connected() != 1) {
      ArduinoCloud.update();
      delay(500);
    }
    delay(1500);

    // initilize Nicla Sense ME:
    BHY2Host.begin(NICLA_VIA_ESLOV, NICLA_VIA_ESLOV);
    sensorTemp.begin();
    sensorHum.begin();
    sensorBaro.begin();
    sensorGas.begin();
    sensorBsec.begin();

    // start the web server on port 80:
    server.begin();
}

void loop() {
  // This actually runs the mDNS module. YOU HAVE TO CALL THIS PERIODICALLY,
  // OR NOTHING WILL WORK! Preferably, call it once per loop().
  mdns.run();

  static auto lastCheck = millis();
  BHY2Host.update();

  // Check sensor values repeatedly.  

  if (millis() - lastCheck >= 10000) {
    lastCheck = millis();
    temperature = 1.0095 * sensorTemp.value() - 4.8051;
    humidity = 1.4383 * sensorHum.value() - 2.5628;
    pressure = 0.718/100 * sensorBaro.value() + sensorBaro.value();
    gas = sensorGas.value();
    iaq = sensorBsec.iaq();
    ArduinoCloud.update();
    Serial.println(sensorBsec.toString());
    Serial.println("T" + String(temperature));
    Serial.println("H" + String(humidity));
    Serial.println("P" + String(pressure));
  }

  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected()) {            // loop while the client's connected

      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // Send the HTML Content
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>body { font-family: 'Segoe UI', sans-serif; text-align: center; background: #121212; color: white; padding-top: 50px; }");
            client.println(".card { background: #1e1e1e; padding: 30px; border-radius: 20px; display: inline-block; box-shadow: 0 10px 20px rgba(0,0,0,0.5); border: 1px solid #333; }");
            client.println("h1 { color: #00d4ff; margin-bottom: 5px; }");
            client.println(".temp { font-size: 4rem; font-weight: bold; color: #ff4757; }");
            client.println(".date { color: #888; letter-spacing: 1px; text-transform: uppercase; font-size: 0.9rem; }</style>");
            client.println("<script>setInterval(() => { location.reload(); }, 10000);</script></head>");
            client.println("<body><div class='card'><h1>Nicla Environment</h1>");
            
            // Display Data
            client.print("<p class='date'>");
            client.print(__DATE__); // Compile-time date as a placeholder
            client.println("</p>");
            client.print("<div class='temp'>");
            client.print(temperature, 1); 
            client.println("C</div>");
            client.println("</div></body></html>");\

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
