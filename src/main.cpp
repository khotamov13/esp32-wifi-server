#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "SPIFFS.h"

const char* ssid = "Infix smart 8";
const char* password = "12345678";
const IPAddress apIP(192,168,4,1);
const byte DNS_PORT = 53;
DNSServer dnsServer;
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  SPIFFS.begin(true);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  WiFi.softAP(ssid, password);
  dnsServer.start(DNS_PORT, "*", apIP);
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
