// ServerRoutes.ino

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "EPD_3in52b.h"

extern AsyncWebServer server;
extern uint8_t uploaded_bw[];  
extern uint8_t uploaded_red[];
extern const size_t BUF_SZ;  


extern bool bwUploaded, redUploaded, apEnabled;
extern char staSsid[32], staPassword[64];
extern volatile bool displayPending;
extern volatile bool clearPending;


String generateApName();
extern const char portalPageHtml[];
extern const char screenSettingsHtml[];
extern const char uploadPageHtml[];


#define WIFI_CONFIG_PATH "/wifi.json"

void displayImageTask(void*);

void initServerRoutes() {
  
  // ─── Home page route ─────────────────────────────────────────────────────
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *r){
    r->send_P(200, "text/html", portalPageHtml);

  });

  // ─── Configuration page route ────────────────────────────────────────────
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *r){
    r->send_P(200, "text/html", screenSettingsHtml);
  });
  server.on("/config", HTTP_POST, [](AsyncWebServerRequest *r){
    if (r->hasParam("ssid", true) && r->hasParam("password", true)) {
      String s = r->getParam("ssid", true)->value();
      String p = r->getParam("password", true)->value();
      DynamicJsonDocument doc(256);
      doc["ssid"]     = s;
      doc["password"] = p;
      File f = LittleFS.open(WIFI_CONFIG_PATH, "w");
      serializeJson(doc, f);
      f.close();
      r->send(200, "text/html", "<h1>Saved! Rebooting…</h1>");
      delay(1000);
      ESP.restart();
    } else {
      r->send(400, "text/plain", "Bad Request");
    }
  });

  // ─── Toggle AP on/off route ───────────────────────────────────────────────
  server.on("/toggleAP", HTTP_GET, [](AsyncWebServerRequest *r){
    toggleAP();
    String state = apEnabled ? "ON" : "OFF";
    r->send(200, "text/plain", state);
  });

  // ─── Image upload routes ─────────────────────────────────────────────────
  server.on("/upload", HTTP_GET, [](AsyncWebServerRequest *r){
    r->send_P(200, "text/html", uploadPageHtml);
  });
  server.on("/upload_bw", HTTP_POST,
    [](AsyncWebServerRequest *r){},
    nullptr,
    [](AsyncWebServerRequest *r, uint8_t *data, size_t len, size_t idx, size_t total){
      if (idx == 0) memset(uploaded_bw, 0, BUF_SZ);
      memcpy(uploaded_bw + idx, data, len);
      if (idx + len == total) {
        File f = LittleFS.open("/bw.bin", "w");
        f.write(uploaded_bw, BUF_SZ);
        f.close();
        bwUploaded = true;
        r->send(200, "text/plain", "OK");
      }
    }
  );
  server.on("/upload_red", HTTP_POST,
    [](AsyncWebServerRequest *r){},
    nullptr,
    [](AsyncWebServerRequest *r, uint8_t *data, size_t len, size_t idx, size_t total){
      if (idx == 0) memset(uploaded_red, 0, BUF_SZ);
      memcpy(uploaded_red + idx, data, len);
      if (idx + len == total) {
        File f = LittleFS.open("/red.bin", "w");
        f.write(uploaded_red, BUF_SZ);
        f.close();
        redUploaded = true;
        r->send(200, "text/plain", "OK");
      }
    }
  );

  // ─── Image download routes ────────────────────────────────────────────────
  server.on("/download_bw", HTTP_GET, [](AsyncWebServerRequest *r){
    r->send(LittleFS, "/bw.bin", "application/octet-stream", true);
  });
  server.on("/download_red", HTTP_GET, [](AsyncWebServerRequest *r){
    r->send(LittleFS, "/red.bin", "application/octet-stream", true);
  });

  // ─── Generate BMP preview from raw buffers ────────────────────────────────
  server.on("/preview.bmp", HTTP_GET, [](AsyncWebServerRequest *r){
    if (!LittleFS.exists("/bw.bin") || !LittleFS.exists("/red.bin")) {
      r->send(404, "text/plain", "No preview");
      return;
    }
    const uint16_t W = 240, H = 360;
    const size_t planeSize = W * H / 8;
    File fb = LittleFS.open("/bw.bin", "r");
    fb.read(uploaded_bw, planeSize); fb.close();
    File fr = LittleFS.open("/red.bin", "r");
    fr.read(uploaded_red, planeSize); fr.close();

    uint32_t rowSize     = (W * 3 + 3) & ~3;
    uint32_t pixelDataSz = rowSize * H;
    uint32_t fileSize    = 54 + pixelDataSz;
    File out = LittleFS.open("/preview.bmp", "w");
    uint8_t hdr[54] = {0};
    hdr[0]='B'; hdr[1]='M';
    hdr[2]=fileSize; hdr[3]=fileSize>>8; hdr[4]=fileSize>>16; hdr[5]=fileSize>>24;
    hdr[10]=54; hdr[14]=40;
    hdr[18]=W; hdr[19]=W>>8; hdr[20]=W>>16; hdr[21]=W>>24;
    hdr[22]=H; hdr[23]=H>>8; hdr[24]=H>>16; hdr[25]=H>>24;
    hdr[26]=1; hdr[28]=24;
    out.write(hdr, 54);

    uint8_t row[rowSize];
    for (int y = H-1; y>=0; y--) {
      memset(row, 255, rowSize);
      for (uint16_t x=0; x<W; x++) {
        size_t i = ((y*W + x)>>3);
        uint8_t bit = 7 - (x & 7);
        bool bw = (uploaded_bw[i]  >> bit) & 1;
        bool rd = (uploaded_red[i] >> bit) & 1;
        size_t p = x*3;
        if (!bw) { row[p]=row[p+1]=row[p+2]=0; }
        if (!rd) { row[p]=row[p+1]=0; row[p+2]=255; }
      }
      out.write(row, rowSize);
    }
    out.close();
    r->send(LittleFS, "/preview.bmp", "image/bmp");
  });

  server.on("/display", HTTP_POST, [](AsyncWebServerRequest *r){
  if (bwUploaded && redUploaded) {

    displayPending = true;   
    
    r->send(200, "text/plain", "Displaying…");
    bwUploaded = redUploaded = false;
  } else {
    r->send(400, "text/plain", "Missing data");
  }
});

  server.on("/display_current", HTTP_POST, [](AsyncWebServerRequest *r){
    if (!LittleFS.exists("/bw.bin") || !LittleFS.exists("/red.bin")) {
      r->send(400, "text/plain", "No image data");
      return;
    }

    File fb = LittleFS.open("/bw.bin", "r");
    fb.read(uploaded_bw, BUF_SZ);
    fb.close();

    File fr = LittleFS.open("/red.bin", "r");
    fr.read(uploaded_red, BUF_SZ);
    fr.close();

    

    displayPending = true;

    r->send(200, "text/plain", "Display queued");
  });   

  // ─── Clear Image ─────────────────────────────────────────────────────────
  server.on("/clear_image", HTTP_POST, [](AsyncWebServerRequest *r){
    memset(uploaded_bw,  0xFF, BUF_SZ);   
    memset(uploaded_red, 0xFF, BUF_SZ);  
    bwUploaded = redUploaded = false;
    LittleFS.remove("/bw.bin");
    LittleFS.remove("/red.bin");
    LittleFS.remove("/preview.bmp");

    clearPending = true;

    r->send(200, "text/plain", "Clearing queued");
  });



  // ─── Factory Reset ─────────────────────────────────────────────────────────
  server.on("/factory_reset", HTTP_POST, [](AsyncWebServerRequest *r){
    LittleFS.remove(WIFI_CONFIG_PATH);
    staSsid[0] = staPassword[0] = '\0';
    LittleFS.remove("/bw.bin");
    LittleFS.remove("/red.bin");
    LittleFS.remove("/preview.bmp");
    memset(uploaded_bw,  0, BUF_SZ);
    memset(uploaded_red, 0, BUF_SZ);
    bwUploaded = redUploaded = false;
    clearPending = true;
    r->send(200, "text/plain", "Factory reset completed");
  });

  // ─── Status ───────────────────────────────────────────────────────────────
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *r){
    DynamicJsonDocument doc(256);
    doc["apEnabled"] = apEnabled;
    doc["staSsid"]   = String(staSsid);
    doc["staIp"]     = WiFi.localIP().toString();
    doc["apSsid"]    = generateApName();
    doc["apPass"]    = String("12345678");
    doc["apIp"]      = WiFi.softAPIP().toString();
    String out;
    serializeJson(doc, out);
    r->send(200, "application/json", out);
  });
}
