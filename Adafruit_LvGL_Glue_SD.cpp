#include "Adafruit_LvGL_Glue_SD.h"

static void waitForDisplay(Adafruit_SPITFT *display) {
  // Before accessing SD, wait on any in-progress
  // DMA screen transfer to finish (shared bus).
  display->dmaWait();
  display->endWrite();
}

// Callback functions to support reading images from SD cards
static void *sd_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode) {
  Adafruit_LvGL_Glue_SD *glue = (Adafruit_LvGL_Glue_SD *)drv->user_data;
  waitForDisplay(glue->display);

  // Support only reading
  if (mode != LV_FS_MODE_RD) {
    return NULL;
  }

  SdFat *sd = glue->sd;
  file_t file = sd->open(path);

  if (!file) {
    LV_LOG_ERROR("Failed to open file %s", path);
    return NULL;
  }

  if (!file.seek(0)) {
    return NULL;
  }

  file_t *fp = (file_t *)lv_mem_alloc(sizeof(file_t));

  if (fp == NULL) {
    return NULL;
  }

  *fp = file;
  return fp;
}

static lv_fs_res_t sd_read(struct _lv_fs_drv_t *drv, void *file_p, void *buf,
                           uint32_t btr, uint32_t *br) {
  Adafruit_LvGL_Glue_SD *glue = (Adafruit_LvGL_Glue_SD *)drv->user_data;
  waitForDisplay(glue->display);

  file_t *fp = (file_t *)file_p;
  *br = fp->read(buf, btr);

  return (*br != -1) ? LV_FS_RES_OK : LV_FS_RES_FS_ERR;
}

static lv_fs_res_t sd_close(lv_fs_drv_t *drv, void *file_p) {
  Adafruit_LvGL_Glue_SD *glue = (Adafruit_LvGL_Glue_SD *)drv->user_data;
  waitForDisplay(glue->display);

  file_t *fp = (file_t *)file_p;
  lv_fs_res_t result = fp->close() ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
  lv_mem_free(fp);

  return result;
}

static lv_fs_res_t sd_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos,
                           lv_fs_whence_t whence) {
  Adafruit_LvGL_Glue_SD *glue = (Adafruit_LvGL_Glue_SD *)drv->user_data;
  waitForDisplay(glue->display);

  file_t *fp = (file_t *)file_p;
  return fp->seek(pos) ? LV_FS_RES_OK : LV_FS_RES_UNKNOWN;
}

static lv_fs_res_t sd_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p) {
  Adafruit_LvGL_Glue_SD *glue = (Adafruit_LvGL_Glue_SD *)drv->user_data;
  waitForDisplay(glue->display);

  file_t *fp = (file_t *)file_p;
  *pos_p = fp->position();

  return LV_FS_RES_OK;
}

/**
 * @brief Configure the glue layer and the underlying LvGL code to use the given
 * TFT display driver, touchscreen controller and SD card instances
 *
 * @param tft Pointer to an **already initialized** display object instance
 * @param touch Pointer to an **already initialized** `Adafruit_STMPE610`
 * touchscreen controller object instance
 * @param sdFat Pointer to an **already initialized** `SdFat` object instance
 * @param debug Debug flag to enable debug messages. Only used if LV_USE_LOG is
 * configured in LittleLVGL's lv_conf.h
 * @return LvGLStatus The status of the initialization:
 * * LVGL_OK : Success
 * * LVGL_ERR_TIMER : Failure to set up timers
 * * LVGL_ERR_ALLOC : Failure to allocate memory
 */
LvGLStatus Adafruit_LvGL_Glue_SD::begin(Adafruit_SPITFT *tft,
                                        Adafruit_STMPE610 *touch, SdFat *sdFat,
                                        bool debug) {
  sd = sdFat;
  LvGLStatus status = Adafruit_LvGL_Glue::begin(tft, touch, debug);
  initFileSystem();
  return status;
}

/**
 * @brief Configure the glue layer and the underlying LvGL code to use the given
 * TFT display driver, touchscreen controller and SD card instances
 *
 * @param tft Pointer to an **already initialized** display object instance
 * @param touch Pointer to an **already initialized** `TouchScreen` touchscreen
 * controller object instance
 * @param sdFat Pointer to an **already initialized** `SdFat` object instance
 * @param debug Debug flag to enable debug messages. Only used if LV_USE_LOG is
 * configured in LittleLVGL's lv_conf.h
 * @return LvGLStatus The status of the initialization:
 * * LVGL_OK : Success
 * * LVGL_ERR_TIMER : Failure to set up timers
 * * LVGL_ERR_ALLOC : Failure to allocate memory
 */
LvGLStatus Adafruit_LvGL_Glue_SD::begin(Adafruit_SPITFT *tft,
                                        TouchScreen *touch, SdFat *sdFat,
                                        bool debug) {
  sd = sdFat;
  LvGLStatus status = Adafruit_LvGL_Glue::begin(tft, touch, debug);
  initFileSystem();
  return status;
}

/**
 * @brief Configure the glue layer and the underlying LvGL code to use the given
 * TFT display driver and SD card instances
 *
 * @param tft Pointer to an **already initialized** display object instance
 * @param sdFat Pointer to an **already initialized** `SdFat` object instance
 * @param debug Debug flag to enable debug messages. Only used if LV_USE_LOG is
 * configured in LittleLVGL's lv_conf.h
 * @return LvGLStatus The status of the initialization:
 * * LVGL_OK : Success
 * * LVGL_ERR_TIMER : Failure to set up timers
 * * LVGL_ERR_ALLOC : Failure to allocate memory
 */
LvGLStatus Adafruit_LvGL_Glue_SD::begin(Adafruit_SPITFT *tft, SdFat *sdFat,
                                        bool debug) {
  sd = sdFat;
  LvGLStatus status = Adafruit_LvGL_Glue::begin(tft, debug);
  initFileSystem();
  return status;
}

void Adafruit_LvGL_Glue_SD::initFileSystem() {
  lv_fs_drv_init(&lv_fs_drv);
  lv_fs_drv.letter = 'S';
  lv_fs_drv.open_cb = sd_open;
  lv_fs_drv.close_cb = sd_close;
  lv_fs_drv.read_cb = sd_read;
  lv_fs_drv.seek_cb = sd_seek;
  lv_fs_drv.tell_cb = sd_tell;
  lv_fs_drv.user_data = this;
  lv_fs_drv_register(&lv_fs_drv);
}
