#include <SPI.h>
// WiFi
#include <WiFiNINA.h>
#include <aWOT.h>
// JSON
#include <ArduinoJson.h>
// secrets
#include "arduinoSecrets.h"


// Create an instance of the server
WiFiServer server(80);
int status = WL_IDLE_STATUS;

// Create aWOT instance
Application app;

// Variables to be exposed to the API
int relayPins[4] = { 2, 3, 4, 5 };
int relayCount = 4;

// Converts a relayId to the actual pin
int getRelayPin(Request req) {
  char relay[4];
  req.route("relayId", relay, 4);
  String relayId(relay); // Convert to string first then to integer
  return relayPins[relayId.toInt()];
}

// Turns a relay on, returns the status after
void relayOn(Request & req, Response & res) {
  int relayPin = getRelayPin(req);
  digitalWrite(relayPin, HIGH);
  relayStatus(req, res);
}

// Turns a relay off, returns the status after
void relayOff(Request & req, Response & res) {
  int relayPin = getRelayPin(req);
  digitalWrite(relayPin, LOW);
  relayStatus(req, res);
}

// Returns the status
void relayStatus(Request & req, Response & res) {
  res.set("Content-Type", "application/json");
  res.set("Access-Control-Allow-Origin", "*");
  int relayPin = getRelayPin(req);
  StaticJsonDocument<200> doc;
  doc["relay-pin"] = relayPin;
  doc["relay-status"] = digitalRead(relayPin);
  serializeJson(doc, Serial);
  Serial.println("");
  serializeJson(doc, res);
}

void addTimer(Request &req, Response &res) {

}

void editTimer(Request &req, Response &res) {

}


void removeTimer(Request &req, Response &res) {

}

void listTimers(Request &req, Response &res) {

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // routes
  app.get("/on/:relayId", &relayOn);
  app.get("/off/:relayId", &relayOff);
  app.get("/status/:relayId", &relayStatus);
  app.get("/timers", &listTimers);
  app.post("/timers", &addTimer);
  app.put("/timers/:timerId", &editTimer);
  app.post("/timers/:timerId/delete", &removeTimer);

  // setup pinmodes
  for (int i = 0; i < relayCount; i++) {
    pinMode( relayPins[i], OUTPUT );
  }

  // Attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, password);

    // Wait 10 seconds for connection
    delay(10000);
  }

  // Start the server
  server.begin();
}

void loop() {
    // listen for incoming clients
  WiFiClient client = server.available();

  if (client.connected()) {
    app.process(&client);
    client.stop();
  }
}