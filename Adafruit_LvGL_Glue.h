#ifndef _ADAFRUIT_LVGL_GLUE_H_
#define _ADAFRUIT_LVGL_GLUE_H_

#include <lvgl.h>                 // LittlevGL core lib
#include <Adafruit_SPITFT.h>      // GFX lib for SPI and parallel displays
#include <TouchScreen.h>          // ADC touchscreen lib
#include <Adafruit_STMPE610.h>    // SPI Touchscreen lib
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
    // Different begin() funcs for STMPE610, ADC or no touch
    LvGLStatus          begin(Adafruit_SPITFT *tft, Adafruit_STMPE610 *touch,
                              bool debug=false);
    LvGLStatus          begin(Adafruit_SPITFT *tft, TouchScreen *touch,
                              bool debug=false);
    LvGLStatus          begin(Adafruit_SPITFT *tft, bool debug=false);
    // These items need to be public for some internal callbacks,
    // but should be avoided by user code please!
    Adafruit_SPITFT    *display;
    void               *touchscreen;
    bool                is_adc_touch;
    bool                first_frame;
  private:
    LvGLStatus          begin(Adafruit_SPITFT *tft, void *touch, bool debug);
    lv_disp_drv_t       lv_disp_drv;
    lv_disp_buf_t       lv_disp_buf;
    lv_color_t         *lv_pixel_buf;
    lv_indev_drv_t      lv_indev_drv;
    lv_indev_t         *lv_input_dev_ptr;
#if defined(ARDUINO_ARCH_SAMD)
    Adafruit_ZeroTimer *zerotimer;
#elif defined(ESP32)
    Ticker              tick;
#elif defined(NRF52_SERIES)
#endif
};

#endif // _ADAFRUIT_LVGL_GLUE_H_
