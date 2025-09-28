// main.ino

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #error "This code only supports ESP8266 or ESP32."
#endif
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "EPD_3in52b.h"
#include <ElegantOTA.h>

#define WIFI_CONFIG_PATH "/wifi.json"
char staSsid[32]     = "";
char staPassword[64] = "";
bool apEnabled       = true;

constexpr size_t BUF_SZ = 240 * 360 / 8;
static uint8_t uploaded_bw  [BUF_SZ];
static uint8_t uploaded_red [BUF_SZ];

static bool    bwUploaded  = false;
static bool    redUploaded = false;

volatile bool displayPending = false;
volatile bool clearPending   = false;

AsyncWebServer server(80);

void   startPortalMode();
void   startStationMode();
void   toggleAP();
void   initServerRoutes();
void   displayImageTask(void*);

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW); 
  #endif

  // ─── Initialize Screen ─────────────────────────────────────────────────────
  DEV_Module_Init();
  EPD_3IN52B_Init();    
  EPD_3IN52B_sleep();
  DEV_Delay_ms(500);

  // ─── Mount or format filesystem ────────────────────────────────────────────
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed, formatting…");
    LittleFS.format();
    if (!LittleFS.begin()) {
      Serial.println("LittleFS still failed → halt");
      return;
    }
  }

  // ─── Load saved Wi-Fi credentials ─────────────────────────────────────────
  if (LittleFS.exists(WIFI_CONFIG_PATH)) {
    File f = LittleFS.open(WIFI_CONFIG_PATH, "r");
    DynamicJsonDocument doc(256);
    if (!deserializeJson(doc, f)) {
      strlcpy(staSsid,     doc["ssid"]     | "", sizeof(staSsid));
      strlcpy(staPassword, doc["password"] | "", sizeof(staPassword));
    }
    f.close();
  }

  // ─── Choose Station or Portal mode based on stored credentials ─────────────
  strlen(staSsid) && strlen(staPassword) ? startStationMode() : startPortalMode();

  // ─── Configure HTTP routes and OTA endpoint ────────────────────────────────
  initServerRoutes();
  ElegantOTA.begin(&server); 
  Serial.printf("Free heap before alloc: %u\n", ESP.getFreeHeap());

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  ElegantOTA.loop();       
  if (displayPending) {
    displayPending = false;
    displayImageTask(nullptr);
  }
  if (clearPending) {
    clearPending = false;
     

    EPD_3IN52B_Init();
    EPD_3IN52B_Clear();
    EPD_3IN52B_sleep();
    DEV_Delay_ms(200);  
  }
}