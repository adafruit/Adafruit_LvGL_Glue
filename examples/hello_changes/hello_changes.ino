/*
THIS EXAMPLE IS REALLY JUST DOCUMENTATION TO GUIDE YOU THROUGH CHANGES IN
Adafruit_LvGL_Glue 2.0.

If you’re coming to Adafruit_LvGL_Glue for the first time as a current user
of LittlevGL on other platforms, you're fine, there are no surprises here,
have a look at the other examples to see how to set up an Adafruit display
for your LittlevGL application.

BUT...if you've used a prior version of Adafruit_LvGL_Glue, and had written
Arduino sketches around it, you're unfortunately in for some extra work.
The "glue" hasn't changed at all, but LittlevGL has seen repeated overhauls,
and projects using earlier versions of Adafruit_LvGL_Glue will no longer
compile in the new system without substantial changes.

If desperate, you can downgrade to lv_arduino 2.1.5 and Adafruit_LvGL_Glue
1.0.2, but this is NOT recommended when developing for the long haul --
lv_arduino is altogether deprecated now and won't be staging a comeback.

For starters, LittlevGL has moved to an entirely different Arduino library.
"lv_arduino" should be UNINSTALLED, and in its place, "lvgl" should be
INSTALLED. The latter is at version 7.11.0 as this is being written...if
there's a newer release, and if you find our glue examples failing to
compile, it's recommended to install that version until this library can be
updated. The LittlevGL developers' preference favors structural and
consistency upgrades over backwards compatibility -- and that's fine, their
project, their rules -- just explaining why this overhaul is necessary.

To repeat: in the Arduino Library Manager, uninstall lv_arduino, install
lvgl.

Also in the Arduino Library Manager, you'll see a related library called
"lv_examples" from the same developer. You can install that if you want, but
understand that this is not actually a library, nor will any of the examples
there compile in the Arduino IDE! But if you pick through these files
manually, there's C code you can dissect for insights in creating interfaces
with LittlevGL, and might create mash-ups with Adafruit_LvGL_Glue examples.

Before using the lvgl and Adafruit_LvGL_Glue libraries, you MUST create a
library configuration file, and this kind of breaks some Arduino rules...

Copy the file:
Documents/Arduino/libraries/lvgl/lv_conf_template.h
to:
Documents/Arduino/libraries/lv_conf.h

Yes, this header file lives OUTSIDE the lvgl library directory! That's just
how it is, I don't make the rules.

Near the top of this file, change:
 #if 0 //Set it to "1" to enable content
to:
 #if 1 //Set it to "1" to enable content

A little ways down, look for:
 #  define LV_MEM_SIZE    (32U * 1024U)
and change to:
 #ifdef ARDUINO_SAMD_ZERO
 #  define LV_MEM_SIZE    (4U * 1024U)
 #else
 #  define LV_MEM_SIZE    (32U * 1024U)
 #endif

Much further down, search for:
 #define LV_USE_USER_DATA        0
and change this to:
 #define LV_USE_USER_DATA        1
and that should get you started.

BELOW IS A HYPOTHETICAL AND MINIMAL BUT ESSENTIALLY VALID ADAFRUIT_LVGL_GLUE
ARDUINO SKETCH. Please see the other examples for more realistic use. Actual
projects will have different display interfacing, backlight control, a set
of UI widgets and so forth.
*/

#include <lvgl.h>               // LittlevGL header
#include <Adafruit_ST7789.h>    // Display-specific header
#include <Adafruit_LvGL_Glue.h> // Glue library header

#define TFT_CS  1 // Display chip-select pin
#define TFT_DC  2 // Display data/command pin
#define TFT_RST 3 // Display reset pin

Adafruit_ST7789    tft(TFT_CS, TFT_DC, TFT_RST); // TFT on default SPI port
Adafruit_LvGL_Glue glue;

void setup(void) {
  Serial.begin(115200);

  tft.init(240, 240); // Initialize display

  // Initialize glue, passing in address of display
  LvGLStatus status = glue.begin(&tft);
  if(status != LVGL_OK) {
    Serial.printf("Glue error %d\r\n", (int)status);
    for(;;);
  }

  // Create simple label centered on screen
  lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, "Hello Arduino!");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
}

void loop(void) {
  lv_task_handler(); // Call LittleVGL task handler periodically
  delay(5);
}
