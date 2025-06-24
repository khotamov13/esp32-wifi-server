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
  
  // SPIFFSni boshlash
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS boshlanmadi!");
    return;
  }

  // Wi-Fi AP sozlash
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP ochildi");

  // DNS: barcha so‘rovni ESP32'ga yo‘naltirish
  dnsServer.start(DNS_PORT, "*", apIP);

  // Static fayllarni berish
  server.serveStatic("/", SPIFFS, "/");

  // Fallback: notFound bo‘lsa ham index.html qaytarilsin
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
