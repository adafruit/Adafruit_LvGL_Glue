// Minimal HELLO WORLD example for Adafruit_LvGL_Glue library

#include <lvgl.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_LvGL_Glue.h>

#if defined(ADAFRUIT_PYPORTAL)
  #include <TouchScreen.h>
  #define ROTATION       3
  #define TFT_D0        34 // Data bit 0 pin (MUST be on PORT byte boundary)
  #define TFT_WR        26 // Write-strobe pin (CCL-inverted timer output)
  #define TFT_DC        10 // Data/command pin
  #define TFT_CS        11 // Chip-select pin
  #define TFT_RST       24 // Reset pin
  #define TFT_RD         9 // Read-strobe pin
  #define TFT_BACKLIGHT 25
  #define YP            A4
  #define XM            A7
  #define YM            A6
  #define XP            A5
  Adafruit_ILI9341 tft = Adafruit_ILI9341(tft8bitbus, TFT_D0, TFT_WR, TFT_DC, TFT_CS, TFT_RST, TFT_RD);
  TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#else // FeatherWing
  #include <Adafruit_STMPE610.h>
  #define ROTATION       1
 #if defined(ESP32)
   #define STMPE_CS     32
   #define TFT_CS       15
   #define TFT_DC       33
 #else
  #define TFT_CS         9
  #define TFT_DC        10
  #define STMPE_CS       6
 #endif
  Adafruit_ILI9341  tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
  Adafruit_STMPE610 ts  = Adafruit_STMPE610(STMPE_CS);
#endif

Adafruit_LvGL_Glue glue;

void setup(void) {
  Serial.begin(115200);
while(!Serial);

  tft.begin();
  tft.setRotation(ROTATION); // Landscape orientation
#if defined(TFT_BACKLIGHT)
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH);
  // No begin function needed for TouchScreen lib
#else
  if(!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    for(;;);
  }
#endif

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
