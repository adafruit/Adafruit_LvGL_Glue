#ifndef _ADAFRUIT_LVGL_GLUE_SD_H
#define _ADAFRUIT_LVGL_GLUE_SD_H

#include "Adafruit_LvGL_Glue.h"
#include <SdFat.h>

typedef File file_t;

/**
 * @brief Class to act as a "glue" layer between the LvGL graphics library and
 * most of Adafruit's TFT displays, with added support for reading from SD card.
 *
 */
class Adafruit_LvGL_Glue_SD : public Adafruit_LvGL_Glue {
public:
  LvGLStatus begin(Adafruit_SPITFT *tft, Adafruit_STMPE610 *touch, SdFat *sdFat,
                   bool debug = false);

  LvGLStatus begin(Adafruit_SPITFT *tft, TouchScreen *touch, SdFat *sdFat,
                   bool debug = false);

  LvGLStatus begin(Adafruit_SPITFT *tft, SdFat *sdFat, bool debug = false);

  // The following need to be public for internal callbacks
  SdFat *sd; ///< Pointer to SD card reader

private:
  void initFileSystem();
  lv_fs_drv_t lv_fs_drv;
};

#endif //_ADAFRUIT_LVGL_GLUE_SD_H
