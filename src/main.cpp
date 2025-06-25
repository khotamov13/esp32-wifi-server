#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include "SPIFFS.h"
#include <esp_system.h>  // Xotira tekshiruvi uchun

// Funksiya deklaratsiyasi (forward declaration)
void blinkError();

const char* ssid = "InfixSmart8";          // Wi-Fi nomi
const char* password = "12345678";          // Wi-Fi paroli
const IPAddress apIP(192, 168, 4, 1);       // AP IP manzili
const byte DNS_PORT = 53;                   // DNS porti

// LED PIN lari (ESP32-S3 DevKitC-1 uchun moslashtiring)
const int BLUE_LED = 2;    // Ko‘k LED (ishlayotgan)
const int GREEN_LED = 3;   // Yashil LED (ishlamayotgan)
const int RED_LED = 4;     // Qizil LED (xato)

DNSServer dnsServer;
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(100);               // Serial tayyorlanishini kutish

  // LED larni sozlash
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // Xotira tekshiruvi
  if (ESP.getFreeHeap() < 10000) { // Agar xotira 10 KB dan kam bo‘lsa
    Serial.println("XATO: Xotira to‘lib qoldi!");
    blinkError();
    while (1) delay(100);
  }

  // SPIFFSni boshlash
  if (!SPIFFS.begin(true)) {
    Serial.println("XATO: SPIFFS boshlanmadi!");
    digitalWrite(GREEN_LED, HIGH);          // Yashil LED yoqilsin
    while (1) delay(100);                   // Xatolikda to‘xtash
  }
  Serial.println("SPIFFS muvaffaqiyatli boshlandi");

  // Wi-Fi AP sozlash
  if (!WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0))) {
    Serial.println("XATO: AP konfiguratsiyasi muvaffaqiyatsiz!");
    blinkError();                          // Qizil va Yashil LED ni tez-tez yoqish
    while (1) delay(100);
  }
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("XATO: AP boshlanmadi!");
    blinkError();                          // Qizil va Yashil LED ni tez-tez yoqish
    while (1) delay(100);
  }
  Serial.println("Wi-Fi AP ochildi: " + WiFi.softAPIP().toString());

  // DNS serverni boshlash
  if (!dnsServer.start(DNS_PORT, "*", apIP)) {
    Serial.println("XATO: DNS server boshlanmadi!");
    blinkError();                          // Qizil va Yashil LED ni tez-tez yoqish
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
  digitalWrite(BLUE_LED, HIGH);             // Ko‘k LED yoqilsin (hamma narsa yaxshi)
}

void loop() {
  dnsServer.processNextRequest();
}

// Xato holatida LED ni tez-tez yoqish funksiyasi
void blinkError() {
  while (1) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    delay(200);
  }
}
