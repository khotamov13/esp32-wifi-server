#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "SPIFFS.h"

const char* ssid = "InfixSmart8";          // Wi-Fi nomi
const char* password = "12345678";          // Wi-Fi paroli
const IPAddress apIP(192, 168, 4, 1);       // AP IP manzili
const byte DNS_PORT = 53;                   // DNS porti

DNSServer dnsServer;
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(100);               // Serial tayyorlanishini kutish

  // SPIFFSni boshlash
  if (!SPIFFS.begin(true)) {
    Serial.println("XATO: SPIFFS boshlanmadi!");
    while (1) delay(100);                   // Xatolikda to‘xtash
  }
  Serial.println("SPIFFS muvaffaqiyatli boshlandi");

  // Wi-Fi AP sozlash
  if (!WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0))) {
    Serial.println("XATO: AP konfiguratsiyasi muvaffaqiyatsiz!");
    while (1) delay(100);
  }
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("XATO: AP boshlanmadi!");
    while (1) delay(100);
  }
  Serial.println("Wi-Fi AP ochildi: " + WiFi.softAPIP().toString());

  // DNS serverni boshlash
  if (!dnsServer.start(DNS_PORT, "*", apIP)) {
    Serial.println("XATO: DNS server boshlanmadi!");
    while (1) delay(100);
  }
  Serial.println("DNS server ishga tushdi");

  // Static fayllarni berish
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  // Fallback: notFound bo‘lsa index.html qaytarilsin
  server.onNotFound([](AsyncWebServerRequest *request) {
    if (SPIFFS.exists("/index.html")) {
      request->send(SPIFFS, "/index.html", "text/html");
    } else {
      request->send(404, "text/plain", "XATO: index.html topilmadi!");
    }
  });

  server.begin();
  Serial.println("Veb-server ishga tushdi");
}

void loop() {
  dnsServer.processNextRequest();
}
