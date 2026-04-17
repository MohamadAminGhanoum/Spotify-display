#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <SPI.h>

// ================= TFT PINS (C3 SAFE) =================
#define TFT_CS   7
#define TFT_RST  10
#define TFT_DC   2
#define TFT_SCLK 4
#define TFT_MOSI 6

// ================= INPUTS =================
#define POT_PIN 1
#define BTN_NEXT 8
#define BTN_PREV 9
#define BTN_PLAY 3

// ================= WIFI =================
char* SSID = "INSERT_SSID";
char* PASSWORD = "INSERT_PASSWORD";

// ================= SPOTIFY =================
const char* CLIENT_ID = "CLIENT_ID";
const char* CLIENT_SECRET = "CLIENT_SECRET";

Spotify sp(CLIENT_ID, CLIENT_SECRET);

// ================= DISPLAY =================
Adafruit_ST7735 tft = Adafruit_ST7735(
  TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST
);

// ================= STATE =================
String lastArtist = "";
String lastTrack = "";

void setup() {
  Serial.begin(115200);

  // ---------- DISPLAY ----------
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(10, 10);
  tft.setTextSize(1);
  tft.write("Booting...");

  // ---------- INPUTS ----------
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);
  pinMode(BTN_PLAY, INPUT_PULLUP);

  // ---------- WIFI ----------
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 10);
  tft.write("WiFi OK");

  // ---------- SPOTIFY ----------
  sp.begin();

  while (!sp.is_auth()) {
    sp.handle_client();
  }

  Serial.println("Spotify Connected!");
}

void loop() {

  // ================= SPOTIFY DATA =================
  String artist = sp.current_artist_names();
  String track  = sp.current_track_name();

  // ================= POTENTIOMETER (VOLUME INPUT) =================
  int potValue = analogRead(POT_PIN);
  int volume = map(potValue, 0, 4095, 0, 100);

  // ================= BUTTONS =================
  if (digitalRead(BTN_PLAY) == LOW) {
    sp.start_resume_playback();
    delay(300);
  }

  if (digitalRead(BTN_NEXT) == LOW) {
    sp.skip();
    delay(300);
  }

  if (digitalRead(BTN_PREV) == LOW) {
    sp.previous();
    delay(300);
  }

  // ================= DISPLAY UPDATE =================
  if (artist != lastArtist && artist != "Something went wrong") {
    lastArtist = artist;

    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 10);
    tft.setTextSize(1);
    tft.write("Artist:");
    tft.setCursor(10, 25);
    tft.write(lastArtist.c_str());
  }

  if (track != lastTrack && track != "Something went wrong") {
    lastTrack = track;

    tft.setCursor(10, 50);
    tft.write("Track:");
    tft.setCursor(10, 65);
    tft.write(lastTrack.c_str());
  }

  // ================= VOLUME DISPLAY (OPTIONAL) =================
  tft.setCursor(10, 100);
  tft.write("Vol: ");
  tft.write(String(volume).c_str());

  delay(1000);
}
