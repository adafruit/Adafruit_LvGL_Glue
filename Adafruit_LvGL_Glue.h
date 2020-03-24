#ifndef _ADAFRUIT_LVGL_GLUE_H_
#define _ADAFRUIT_LVGL_GLUE_H_

#include <lvgl.h>                 // LittlevGL core lib
#include <Adafruit_SPITFT.h>      // GFX lib for SPI and parallel displays
#include <Adafruit_STMPE610.h>    // Touchscreen lib
#if defined(ARDUINO_ARCH_SAMD)
  #include <Adafruit_ZeroTimer.h> // SAMD-specific timer lib
#elif defined(ESP32)
  #include <Ticker.h>             // ESP32-specific timer lib
#endif

typedef enum {
    LVGL_OK,
    LVGL_ERR_ALLOC,
    LVGL_ERR_TIMER,
} LvGLStatus;

class Adafruit_LvGL_Glue {
  public:
    Adafruit_LvGL_Glue(void);
    ~Adafruit_LvGL_Glue(void);
    LvGLStatus begin(Adafruit_SPITFT *tft, Adafruit_STMPE610 *touch,
                     bool debug=false);
    Adafruit_SPITFT    *display;     // Gotta be public for LvGL callbacks,
    Adafruit_STMPE610  *touchscreen; // please don't touch!
  private:
    lv_disp_drv_t       lv_disp_drv;
    lv_disp_buf_t       lv_disp_buf;
    lv_color_t         *lv_pixel_buf;
    lv_indev_drv_t      lv_indev_drv;
    lv_indev_t         *lv_input_dev_ptr;
#if defined(ARDUINO_ARCH_SAMD)
    Adafruit_ZeroTimer *zerotimer;
#elif defined(ESP32)
#elif defined(NRF52_SERIES)
#endif
};

#endif // _ADAFRUIT_LVGL_GLUE_H_
