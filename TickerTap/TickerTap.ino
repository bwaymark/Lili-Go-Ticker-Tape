// ============================================================
//  TickerTap
//  A scrolling message ticker for the LilyGO T-Display S3.
//
//  Messages live in a text file hosted online (a GitHub Gist by
//  default). The display fetches the file on boot and then every
//  few minutes, so you can change what it says without
//  re-flashing the device and without knowing its IP address.
//
//  If WiFi or the fetch fails, it falls back to the built-in
//  messages in the `messages` list below.
//
//  See config.h for WiFi, message URL and scroll speed.
//  See README.md for full setup instructions.
// ============================================================

#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <vector>

#include "config.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

// Built-in messages, shown until (or unless) the online file loads.
// Replace these with your own if you like.
std::vector<String> messages = {
  "TickerTap is running. Set MESSAGES_URL in config.h to load your own messages.",
  "Edit your message file online and this display will pick up the changes.",
  "No re-flashing needed."
};

const bool useRemote = strlen(MESSAGES_URL) > 0;

int msgIndex = 0;
int textW;
int x;
unsigned long lastFetch = 0;
bool fetchedOnce = false;

// The built-in font is ASCII only, so swap smart punctuation for
// plain equivalents. Text typed on a Mac or in Word often has these.
String asciiOnly(String s) {
  s.replace("\xE2\x80\x99", "'");    // right single quote
  s.replace("\xE2\x80\x98", "'");    // left single quote
  s.replace("\xE2\x80\x9C", "\"");   // left double quote
  s.replace("\xE2\x80\x9D", "\"");   // right double quote
  s.replace("\xE2\x80\x93", "-");    // en dash
  s.replace("\xE2\x80\x94", "-");    // em dash
  s.replace("\xE2\x80\xA6", "...");  // ellipsis
  return s;
}

bool connectWiFi(int maxAttempts) {
  if (WiFi.status() == WL_CONNECTED) return true;
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  for (int i = 0; i < maxAttempts && WiFi.status() != WL_CONNECTED; i++) delay(500);
  return WiFi.status() == WL_CONNECTED;
}

bool fetchMessages() {
  if (!useRemote) return false;

  if (!connectWiFi(10)) {
    USBSerial.println("Fetch: WiFi not connected");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();   // public text file, so no certificate pinning
  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setTimeout(8000);
  if (!http.begin(client, MESSAGES_URL)) {
    USBSerial.println("Fetch: http.begin failed");
    return false;
  }

  int code = http.GET();
  USBSerial.println("Fetch HTTP: " + String(code));
  if (code != 200) { http.end(); return false; }
  String body = asciiOnly(http.getString());
  http.end();

  // One message per line. Blank lines and lines starting with # are ignored.
  std::vector<String> fresh;
  int start = 0;
  while (start < (int)body.length()) {
    int end = body.indexOf('\n', start);
    if (end < 0) end = body.length();
    String line = body.substring(start, end);
    line.trim();
    if (line.length() > 0 && line[0] != '#') fresh.push_back(line);
    start = end + 1;
  }

  if (fresh.empty()) {
    USBSerial.println("Fetch: no usable lines");
    return false;
  }

  messages = fresh;
  if (msgIndex >= (int)messages.size()) msgIndex = 0;
  USBSerial.println("Fetch: loaded " + String(messages.size()) + " messages");
  return true;
}

void setup() {
  USBSerial.begin(115200);

  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);       // display power, needed when on battery

  tft.init();
  tft.setRotation(1);           // landscape, 320x170
  tft.fillScreen(TFT_BLACK);

  pinMode(38, OUTPUT);
  digitalWrite(38, HIGH);       // backlight

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println(useRemote ? "Loading messages..." : "Starting...");

  spr.createSprite(tft.width(), 60);
  spr.setFreeFont(&FreeSansBold24pt7b);
  spr.setTextColor(TEXT_COLOUR);
  spr.setTextDatum(ML_DATUM);   // vertically centred on y

  if (useRemote) {
    connectWiFi(30);            // up to ~15 seconds on boot
    fetchedOnce = fetchMessages();
    lastFetch = millis();
  }

  tft.fillScreen(TFT_BLACK);
  textW = spr.textWidth(messages[msgIndex]);
  x = tft.width();              // start just off the right edge
}

void loop() {
  spr.fillSprite(TFT_BLACK);
  spr.drawString(messages[msgIndex], x, spr.height() / 2);
  spr.pushSprite(0, (tft.height() - spr.height()) / 2);

  x -= STEP_PX;

  if (x < -textW) {             // message fully off the left edge
    // Only fetch between messages so the scroll never stalls mid-sentence.
    // Retries quickly until the first success, then settles to REFRESH_MS.
    if (useRemote) {
      unsigned long interval = fetchedOnce ? REFRESH_MS : RETRY_MS;
      if (millis() - lastFetch >= interval) {
        if (fetchMessages()) fetchedOnce = true;
        lastFetch = millis();
      }
    }
    msgIndex = (msgIndex + 1) % messages.size();
    textW = spr.textWidth(messages[msgIndex]);
    x = tft.width();
  }

  delay(FRAME_MS);
}
