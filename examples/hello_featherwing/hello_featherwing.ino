// Minimal "Hello" example for LittlevGL on Adafruit TFT FeatherWings.
// Requires LittlevGL, Adafruit_LvGL_Glue, Adafruit_STMPE610, Adafruit_GFX
// and Adafruit_ILI9341 (2.4" TFT) or Adafruit_HX8357 (3.5") libraries.
// This example doesn't use any touchscreen input, but it's declared anyway
// so this sketch can be copied-and-pasted to serve as a starting point for
// other projects. If display is scrambled, check that correct FeatherWing
// type is selected below (set BIG_FEATHERWING to 0 or 1 as needed).
// If the display is a V2 with the TSC2007 set FEATHER_V2 to 1.
// If you are using an ESP32 with PSRAM then enable BOARD_HAS_PSRAM in lv_conf.h.
// Prior Adafruit_LvGL_Glue users: see hello_changes example for updates!

#define BIG_FEATHERWING 0 // Set this to 1 for 3.5" (480x320) FeatherWing!
#ifndef FEATHER_V2
#define FEATHER_V2 0 // Set this to 1 for V2 TSC2007 touchscreen displays
#endif

#include <Adafruit_LvGL_Glue.h> // Always include this BEFORE lvgl.h!
#include <lvgl.h>
#if FEATHER_V2
#include <Adafruit_TSC2007.h>
#include <Wire.h>
#else
#include <Adafruit_STMPE610.h>
#endif

#ifdef ESP32
   #define TFT_CS   15
   #define TFT_DC   33
   #define STMPE_CS 32
#else
   #define TFT_CS    9
   #define TFT_DC   10
   #define STMPE_CS  6
#endif
#define TFT_ROTATION 1 // Landscape orientation on FeatherWing
#define TFT_RST     -1

#if BIG_FEATHERWING
  #include <Adafruit_HX8357.h>
  Adafruit_HX8357  tft(TFT_CS, TFT_DC, TFT_RST);
#else
  #include <Adafruit_ILI9341.h>
  Adafruit_ILI9341 tft(TFT_CS, TFT_DC);
#endif

#if FEATHER_V2
#define TSC_IRQ STMPE_CS
Adafruit_TSC2007 ts=Adafruit_TSC2007();
#else
Adafruit_STMPE610  ts(STMPE_CS);
#endif

Adafruit_LvGL_Glue glue;

// This example sketch's LittlevGL UI-building calls are all in this
// function rather than in setup(), so simple programs can just
// copy-and-paste this sketch as a starting point, then embellish here:
void lvgl_setup(void) {
  // Locks LVGL resource to prevent memory corrupton on ESP32
  // When using WiFi and LVGL_Glue, this function MUST be called PRIOR to LVGL function (`lv_`) calls
#ifdef ESP32
  glue.lvgl_acquire();
#endif //ESP32

  // Create simple label centered on screen
  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello Arduino!");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  // Unlocks LVGL resource to prevent memory corrupton on ESP32
  // NOTE: This function MUST be called AFTER lvgl_acquire
#ifdef ESP32
  glue.lvgl_release();
#endif //ESP32
}

void setup(void) {
  Serial.begin(115200);

  // Initialize display and touchscreen BEFORE glue setup
  tft.begin();
  tft.setRotation(TFT_ROTATION);

#if FEATHER_V2
  if (ts.begin(0x48, &Wire)) {
    pinMode(TSC_IRQ, INPUT);
  } else {
    Serial.println("Couldn't start TSC2007 touchscreen controller");
    for(;;);
  }
  // Initialize glue, passing in address of display & touchscreen
  LvGLStatus status = glue.begin(&tft, &ts, TSC_IRQ);
#else
  if(!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    for(;;);
  }

  // Initialize glue, passing in address of display & touchscreen
  LvGLStatus status = glue.begin(&tft, &ts);
#endif

  if(status != LVGL_OK) {
    Serial.printf("Glue error %d\r\n", (int)status);
    for(;;);
  }

  lvgl_setup(); // Call UI-building function above
}

void loop(void) {
  // On ESP32, LittleVGL's task handler (`lv_task_handler`) is a task in
  // FreeRTOS and is pinned to the core upon successful initialization.
  // This means that you do not need to call `lv_task_handler()` within the loop() on ESP32
#ifndef ESP32
  lv_task_handler();
#endif //ESP32
  delay(5);
}
