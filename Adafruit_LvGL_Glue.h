#ifndef _ADAFRUIT_LVGL_GLUE_H_
#define _ADAFRUIT_LVGL_GLUE_H_

//#define __USE_TOUCHSCREEN_H__  // Enable use of ADC touchscreen
#ifndef TSC2007_TS
#define TSC2007_TS 0 // Set this to 1 for V2 TSC2007 touchscreen displays
#endif

#include <Adafruit_SPITFT.h>   // GFX lib for SPI and parallel displays
#if TSC2007_TS
#include <Adafruit_TSC2007.h>  // Other SPI touchscreen lib
#else
#include <Adafruit_STMPE610.h> // SPI Touchscreen lib
#endif
#ifdef __USE_TOUCHSCREEN_H__
#include <TouchScreen.h>       // ADC touchscreen lib
#endif //__USE_TOUCHSCREEN_H__
#ifndef LV_CONF_INCLUDE_SIMPLE
#define LV_CONF_INCLUDE_SIMPLE
#endif //LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#include <lvgl.h>              // LittlevGL core lib
#if defined(ARDUINO_ARCH_SAMD)
#include <Adafruit_ZeroTimer.h> // SAMD-specific timer lib
#elif defined(ESP32)
#include <Ticker.h> // ESP32-specific timer lib
//#define BOARD_HAS_PSRAM // If your ESP32 board has PSRAM enable this
#endif

typedef enum {
  LVGL_OK,
  LVGL_ERR_ALLOC,
  LVGL_ERR_TIMER,
  LVGL_ERR_MUTEX,
  LVGL_ERR_TASK
} LvGLStatus;

/**
 * @brief Class to act as a "glue" layer between the LvGL graphics library and
 * most of Adafruit's TFT displays
 *
 */
class Adafruit_LvGL_Glue {
public:
  Adafruit_LvGL_Glue(void);
  ~Adafruit_LvGL_Glue(void);
  // Different begin() funcs for TSC2007, STMPE610, ADC or no touch
#if TSC2007_TS
  LvGLStatus begin(Adafruit_SPITFT *tft, Adafruit_TSC2007 *touch,
                   u_int16_t irq_pin, bool debug = false);
#else
  LvGLStatus begin(Adafruit_SPITFT *tft, Adafruit_STMPE610 *touch,
                   bool debug = false);
#endif
#ifdef __USE_TOUCHSCREEN_H__
  LvGLStatus begin(Adafruit_SPITFT *tft, TouchScreen *touch,
                   bool debug = false);
#endif //__USE_TOUCHSCREEN_H__
  LvGLStatus begin(Adafruit_SPITFT *tft, bool debug = false);
  // These items need to be public for some internal callbacks,
  // but should be avoided by user code please!
  Adafruit_SPITFT *display; ///< Pointer to the SPITFT display instance
  void *touchscreen;        ///< Pointer to the touchscreen object to use
  bool is_adc_touch; ///< determines if the touchscreen controlelr is ADC based
  bool first_frame;  ///< Tracks if a call to `lv_flush_callback` needs to wait
                     ///< for DMA transfer to complete
#if TSC2007_TS
  u_int16_t tsc_irq_pin; ///< IRQ pin to check for TSC2007
#endif

#ifdef ESP32
  void lvgl_acquire(); ///< Acquires the lock around the lvgl object
  void lvgl_release(); ///< Releases the lock around the lvgl object
#endif

private:
  LvGLStatus begin(Adafruit_SPITFT *tft, void *touch, bool debug);
  static lv_disp_drv_t lv_disp_drv;
  static lv_disp_draw_buf_t lv_disp_draw_buf;
  static lv_color_t *lv_pixel_buf;
  static lv_indev_drv_t lv_indev_drv;
  lv_indev_t *lv_input_dev_ptr;
#if defined(ARDUINO_ARCH_SAMD)
  Adafruit_ZeroTimer *zerotimer;
#elif defined(ESP32)
  Ticker tick;
#elif defined(NRF52_SERIES)
#endif
};

#endif // _ADAFRUIT_LVGL_GLUE_H_
