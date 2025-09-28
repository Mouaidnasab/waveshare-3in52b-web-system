// WifiManager.ino

#ifdef ESP8266
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

extern char staSsid[32];
extern char staPassword[64];
extern bool apEnabled;

// ─── Generate unique AP name based on MAC address ──────────────────────────────
String generateApName() {
  String mac = WiFi.macAddress();
  mac.replace(":", "");
  mac.toLowerCase();
  return "e-paper-" + mac.substring(mac.length() - 6);
}

// ─── Enable and start Access Point mode ────────────────────────────────────────
void startPortalMode() {
  apEnabled = true;

  WiFi.mode(WIFI_AP_STA);

  String mac = WiFi.macAddress();
  if (mac == "00:00:00:00:00:00") {
    mac = WiFi.softAPmacAddress();
  }

  mac.replace(":", "");
  mac.toLowerCase();
  String apName = "e-paper-" + mac.substring(mac.length() - 6);

  // ─── AP Password ───
  WiFi.softAP(apName.c_str(), "12345678");
  Serial.printf("AP SSID: %s  IP=%s\n",
                apName.c_str(), WiFi.softAPIP().toString().c_str());
}

// ─── Connect to stored Wi-Fi network in Station mode ───────────────────────────
void startStationMode() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(staSsid, staPassword);
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED)
    Serial.printf("\nSTA IP=%s\n", WiFi.localIP().toString().c_str());
  else {
    Serial.println("\nFailed to connect, switching to AP mode");
    startPortalMode();
  }
}

// ─── Toggle Access Point ───────────────────────────────────────────────────────
void toggleAP() {
  if (apEnabled) {
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    apEnabled = false;
    if (strlen(staSsid) && strlen(staPassword))
      WiFi.begin(staSsid, staPassword);
    Serial.println("Hotspot disabled");
  } else {
    WiFi.mode(WIFI_AP_STA);
    String apName = generateApName();
    WiFi.softAP(apName.c_str(), "12345678");
    apEnabled = true;
    Serial.printf("Hotspot enabled → SSID %s  IP=%s\n",
                  apName.c_str(), WiFi.softAPIP().toString().c_str());
  }
}
