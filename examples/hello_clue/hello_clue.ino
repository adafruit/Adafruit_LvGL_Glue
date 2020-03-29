// Minimal "Hello" example for LittlevGL on Adafruit CLUE. Requires
// LittlevGL, Adafruit_LvGL_Glue, Adafruit_GFX and Adafruit_ST7735
// libraries.

#include <lvgl.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_LvGL_Glue.h>

#define TFT_ROTATION   1 // Landscape orientation on CLUE
#define TFT_SPI     SPI1 // CLUE display peripheral & pins
#define TFT_CS        31
#define TFT_DC        32
#define TFT_RST       33
#define TFT_BACKLIGHT 34

Adafruit_ST7789    tft(&TFT_SPI, TFT_CS, TFT_DC, TFT_RST);
Adafruit_LvGL_Glue glue;

void setup(void) {
  Serial.begin(115200);

  // Initialize display BEFORE glue setup
  tft.init(240, 240);
  tft.setRotation(TFT_ROTATION);
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);

  // Initialize glue, passing in address of display
  LvGLStatus status = glue.begin(&tft);
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
