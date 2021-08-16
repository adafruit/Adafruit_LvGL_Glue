// Minimal example for LittlevGL demonstrating how to load images from the
// SD card reader on Adafruit TFT FeatherWings.
// Requires LittlevGL, Adafruit_LvGL_Glue, Adafruit_STMPE610, Adafruit_GFX,
// SdFat - Adafruit Fork, and Adafruit_ILI9341 (2.4" TFT) or
// Adafruit_HX8357 (3.5") libraries.
// This example doesn't use any touchscreen input, but it's declared anyway
// so this sketch can be copied-and-pasted to serve as a starting point for
// other projects. If display is scrambled, check that correct FeatherWing
// type is selected below (set BIG_FEATHERWING to 0 or 1 as needed).

// Prior Adafruit_LvGL_Glue users: see hello_changes example for updates!

// This example requires the 'cloudy.bin' image file to be placed in the
// root directory of the SD card. Available in the examples folder
// (hello_featherwing_sd/images/cloudy.bin). See official LittlevGL
// documentation for help on converting images to .bin format.

// Not recommended for SAMD21 (M0) boards -- best for SAMD51, ESP32, etc.

#define BIG_FEATHERWING 0 // Set this to 1 for 3.5" (480x320) FeatherWing!

// Always include this BEFORE lvgl.h
// NOTE: this is Adafruit_LvGL_Glue_SD.h and not Adafruit_LvGL_Glue.h
#include <Adafruit_LvGL_Glue_SD.h>
#include <lvgl.h>
#include <Adafruit_STMPE610.h>
#include <SdFat.h>

#ifdef ESP32
#define TFT_CS   15
#define TFT_DC   33
#define STMPE_CS 32
#define SD_CS    14
#else
#define TFT_CS    9
#define TFT_DC   10
#define STMPE_CS  6
#define SD_CS     5
#endif
#define TFT_ROTATION 1 // Landscape orientation on FeatherWing
#define TFT_RST     -1

#if BIG_FEATHERWING
#include <Adafruit_HX8357.h>
Adafruit_HX8357 tft(TFT_CS, TFT_DC, TFT_RST);
#else
#include <Adafruit_ILI9341.h>
Adafruit_ILI9341 tft(TFT_CS, TFT_DC);
#endif

Adafruit_STMPE610 ts(STMPE_CS);

// Use Adafruit_LvGL_Glue_SD instead of Adafruit_LvGL_Glue
Adafruit_LvGL_Glue_SD glue;
SdFat sd;

// This example sketch's LittlevGL UI-building calls are all in this
// function rather than in setup(), so simple programs can just
// copy-and-paste this sketch as a starting point, then embellish here:
void lvgl_setup(void) {
  // Create an image from a file on the SD card
  // The cloudy.bin file (available in this example folder) should have
  // been  manually copied to the SD card first.
  // LVGL Glue initializes the LVGL file system with drive letter 'S'
  lv_obj_t *img = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(img, "S:cloudy.bin");

  lv_obj_t *label = lv_label_create(img, NULL);
  lv_label_set_text(label, "Hello Arduino!");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

void setup(void) {
  Serial.begin(115200);

  // Initialize display and touchscreen BEFORE glue setup
  tft.begin();
  tft.setRotation(TFT_ROTATION);
  if(!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    for(;;);
  }

  // SD controller needs to be initialized before starting Glue
  if (!sd.begin(SD_CS, SD_SCK_MHZ(25))) { // ESP32 requires 25 MHz limit
    Serial.println("Couldn't start SD card controller");
    for(;;);
  }

  // Initialize glue, passing in address of display, touchscreen & SD controller
  LvGLStatus status = glue.begin(&tft, &ts, &sd);
  if(status != LVGL_OK) {
    Serial.printf("Glue error %d\r\n", (int)status);
    for(;;);
  }

  lvgl_setup(); // Call UI-building function above
}

void loop(void) {
  lv_task_handler(); // Call LittleVGL task handler periodically
  delay(5);
}
