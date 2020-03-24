// Minimal HELLO WORLD example for Adafruit_LvGL_Glue library

#include <lvgl.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <Adafruit_LvGL_Glue.h>

#define TFT_CS    9
#define TFT_DC    10
Adafruit_ILI9341  tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define STMPE_CS  6
Adafruit_STMPE610 ts  = Adafruit_STMPE610(STMPE_CS);

Adafruit_LvGL_Glue glue;

void setup(void) {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(1); // Landscape orientation

  if(!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    for(;;);
  }

  // Display and touchscreen must be initialized BEFORE glue setup...
  LvGLStatus status = glue.begin(&tft, &ts);
  if(status != LVGL_OK) {
    Serial.printf("Glue error %d\r\n", (int)status);
    for(;;);
  }

  // Create simple label on active screen
  lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, "Hello Arduino!");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

void loop(void) {
  lv_task_handler(); // Call LittleVGL task handler periodically
  delay(5);
}
