// ============================================================
//  TickerTap — config.h
//  Fill in the WiFi details and message URL before uploading.
//  See README.md for how to find each value.
// ============================================================

#pragma once

// ============================================================
//  WIFI
//  SSID is the name of your WiFi network, exactly as it appears
//  on your phone or laptop (it is case sensitive).
//  The T-Display S3 supports 2.4GHz networks only.
// ============================================================
#define WIFI_SSID      "YOUR_WIFI_SSID"
#define WIFI_PASSWORD  "YOUR_WIFI_PASSWORD"

// ============================================================
//  MESSAGES URL
//  The raw URL of your hosted message file. For a GitHub Gist
//  it looks like this (note: no long revision hash after /raw/):
//
//    https://gist.githubusercontent.com/USERNAME/GIST_ID/raw/messages.txt
//
//  Leave it as "" to use only the built-in messages in the sketch.
// ============================================================
#define MESSAGES_URL  "https://gist.githubusercontent.com/YOUR_USERNAME/YOUR_GIST_ID/raw/messages.txt"

// ============================================================
//  REFRESH TIMING
//  REFRESH_MS: how often to re-fetch the message file once it
//    has loaded successfully (default 10 minutes).
//  RETRY_MS: how often to retry if the first fetch failed
//    (default 30 seconds).
//  Fetches only happen between messages, never mid-scroll.
// ============================================================
#define REFRESH_MS  (10UL * 60UL * 1000UL)
#define RETRY_MS    (30UL * 1000UL)

// ============================================================
//  SCROLL SPEED
//  STEP_PX:  pixels moved per frame (higher = faster)
//  FRAME_MS: delay per frame in milliseconds (lower = faster)
// ============================================================
#define STEP_PX   2
#define FRAME_MS  10

// ============================================================
//  TEXT COLOUR
//  Any TFT_eSPI colour name (TFT_YELLOW, TFT_WHITE, TFT_CYAN,
//  TFT_GREEN, TFT_ORANGE, TFT_MAGENTA...) or a 16-bit value.
// ============================================================
#define TEXT_COLOUR  TFT_YELLOW
