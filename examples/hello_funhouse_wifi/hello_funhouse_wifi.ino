/*
 * Minimal "Hello" example for LVGL + WiFi (or Bluetooth) on Adafruit FunHouse
 * ESP32-S2.
 *
 * This example demonstrates usage of an ESP32 running LVGL and WiFi (or
 * Bluetooth) simultaneously.
 *
 * Full details around LVGL_Glue's enhancements for LVGL/WiFi:
 * https://github.com/adafruit/Adafruit_LvGL_Glue/pull/22
 *
 * Requires LittlevGL, Adafruit_LvGL_Glue, Adafruit_GFX and Adafruit_ST7735
 * libraries.
 *
 * Prior Adafruit_LvGL_Glue users: see hello_changes example for updates!
 */

#include <Adafruit_LvGL_Glue.h> // Always include this BEFORE lvgl.h!
#include <Adafruit_ST7789.h>
#include <WiFiClientSecure.h>
#include <lvgl.h>

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RESET);
Adafruit_LvGL_Glue glue;
WiFiClientSecure client;

const char *ssid = "your-ssid"; // your network SSID (name of wifi network)
const char *password = "your-password"; // your network password

// This example sketch's LittlevGL UI-building calls are all in this
// function rather than in setup(), so simple programs can just
// copy-and-paste this sketch as a starting point, then embellish here:
void lvgl_setup(void) {
  // Locks LVGL resource to prevent memory corrupton on ESP32
  // When using WiFi and LVGL_Glue, this function MUST be called PRIOR to LVGL
  // function (`lv_`) calls
  glue.lvgl_acquire();

  // Create simple label centered on screen
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello Arduino!");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  // Unlocks LVGL resource to prevent memory corrupton on ESP32
  // NOTE: This function MUST be called AFTER lvgl_acquire
  glue.lvgl_release();
}

void setup(void) {
  Serial.begin(115200);

  // Initialize display BEFORE glue setup
  tft.init(240, 240);
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);

  // Initialize glue, passing in address of display
  LvGLStatus status = glue.begin(&tft);
  if (status != LVGL_OK) {
    Serial.printf("Glue error %d\r\n", (int)status);
    for (;;)
      ;
  }

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  lvgl_setup(); // Call UI-building function above
}

void loop(void) {
  // On ESP32, LittleVGL's task handler (`lv_task_handler`) is a task in
  // FreeRTOS and is pinned to the core upon successful initialization.
  // This means that you do not need to call `lv_task_handler()` within the
  // loop() on ESP32
  delay(5);
}
