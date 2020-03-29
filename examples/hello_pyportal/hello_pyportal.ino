// Minimal "Hello" example for LittlevGL on Adafruit PyPortal. Requires
// LittlevGL, Adafruit_LvGL_Glue, Adafruit Touchscreen, Adafruit_GFX and
// Adafruit_ILI9341 (PyPortal, PyPortal Pynt) or Adafruit_HX8357 (PyPortal
// Titano) libraries. This example doesn't use any touchscreen input, but
// it's declared anyway so this sketch can be copied-and-pasted to serve
// as a starting point for other projects.

#include <lvgl.h>
#include <TouchScreen.h>
#include <Adafruit_LvGL_Glue.h>

#define ROTATION       3 // PyPortal landscape orientation
#define TFT_D0        34 // PyPortal TFT pins
#define TFT_WR        26
#define TFT_DC        10
#define TFT_CS        11
#define TFT_RST       24
#define TFT_RD         9
#define TFT_BACKLIGHT 25
#define YP            A4 // PyPortal touchscreen pins
#define XP            A5
#define YM            A6
#define XM            A7

#if defined(ADAFRUIT_PYPORTAL_M4_TITANO)
  #include <Adafruit_HX8357.h>
  Adafruit_HX8357  tft(tft8bitbus, TFT_D0, TFT_WR, TFT_DC, TFT_CS, TFT_RST,
    TFT_RD);
#else
  #include <Adafruit_ILI9341.h>
  Adafruit_ILI9341 tft(tft8bitbus, TFT_D0, TFT_WR, TFT_DC, TFT_CS, TFT_RST,
    TFT_RD);
#endif
TouchScreen        ts(XP, YP, XM, YM, 300);
Adafruit_LvGL_Glue glue;

void setup(void) {
  Serial.begin(115200);

  // Initialize display BEFORE glue setup
  tft.begin();
  tft.setRotation(ROTATION);
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);

  // PyPortal touchscreen needs no init

  // Initialize glue, passing in address of display & touchscreen
  LvGLStatus status = glue.begin(&tft, &ts);
  if(status != LVGL_OK) {
    Serial.printf("Glue error %d\r\n", (int)status);
    for(;;);
  }

  // LittlevGL UI setup proceeds from here ---------------------------------

  // Create simple label centered on screen
  lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, "Hello Arduino!");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

void loop(void) {
  lv_task_handler(); // Call LittleVGL task handler periodically
  delay(5);
}
