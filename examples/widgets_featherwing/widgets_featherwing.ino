// A Pmore interesting example for LittlevGL on Adafruit TFT FeatherWings,
// showing use of the touchscreen. Code's a little more complex than the
// hello_featherwing example, so best get that working before trying this.
// By default, as written, on a 320x240 TFT FeatherWing the example is a
// pretend calculator keypad, while 480x320 TFT has a whole keyboard
// (though you'll probably need a stylus). These just seemed the right
// level of detail for their respective screens, but feel free to override
// and try either for yourself. If display is scrambled, check that
// correct FeatherWing type is selected below (set BIG_FEATHERWING to 0
// or 1 as needed).

#define BIG_FEATHERWING 0 // Set this to 1 for 3.5" (480x320) FeatherWing!

#include <lvgl.h>
#include <Adafruit_LvGL_Glue.h>
#include <Adafruit_STMPE610.h>

#define DEMO_CALC 0
#define DEMO_TEXT 1

#ifdef ESP32
   #define TFT_CS   15
   #define TFT_DC   33
   #define STMPE_CS 32
#else
   #define TFT_CS    9
   #define TFT_DC   10
   #define STMPE_CS  6
#endif
#define TFT_RST     -1

#if BIG_FEATHERWING
  #include <Adafruit_HX8357.h>
  Adafruit_HX8357  tft(TFT_CS, TFT_DC, TFT_RST);
  #define DEMO DEMO_TEXT // On big TFT, do text/keyboard example
#else
  #include <Adafruit_ILI9341.h>
  Adafruit_ILI9341 tft(TFT_CS, TFT_DC);
  #define DEMO DEMO_CALC // Smaller TFT, do keypad example
#endif

Adafruit_STMPE610  ts(STMPE_CS);
Adafruit_LvGL_Glue glue;

#undef DEMO
#define DEMO DEMO_TEXT

#if DEMO == DEMO_CALC

// "Pretend" calculator example. Please, PLEASE...do NOT implement the whole
// calculator and submit as a pull request, because it will NOT be merged!
// This sketch is meant only to be illustrative and not functional, just
// showing LittlevGL + Adafruit display/touch tied together with a modest
// amount of code. Even a simple but working calc would require WAY more
// code, distracting from that core idea (and is a waste of hardware).
// Daiso has better calculators for $1.50.

#define TFT_ROTATION 0 // Portrait orientation

lv_obj_t   *digits_label = NULL; // LittlevGL label object showing digits
String      digits     = "0";    // Current digits string value
bool        hasDecimal = false;  // Only allow one decimal point
const char *buttons[]  = {       // Button matrix labels
  "7", "8", "9", "/", "\n",
  "4", "5", "6", "x", "\n",
  "1", "2", "3", "-", "\n",
  "0", ".", "=", "+", "" };

// This function processes events from the button matrix
void button_event_handler(lv_obj_t *obj, lv_event_t event) {
  if(event == LV_EVENT_VALUE_CHANGED) {
    const char *txt = lv_btnm_get_active_btn_text(obj);
    if(txt) { // NULL if pressed in frame area outside buttons
      if(txt[0] == '.') {
        // Decimal button pressed. Add decimal point to "digits" string
        // if it's not too long and there's no decimal present yet...
        if((digits.length() < 15) && !hasDecimal) {
          digits    += '.';
          hasDecimal = true;
        }
      } else if((txt[0] >= '0') && (txt[0] <= '9')) {
        // Number button (0-9) pressed. If there's nothing currently
        // being displayed, take the digit literally. Otherwise, append
        // the new digit if the "digits" string is not too long.
        if(digits.equals("0")) {
          digits = txt[0];
        } else if(digits.length() < 15) {
          digits += txt[0];
        }
      } else {
        // Any other button, just reset the calculator display.
        // It's all just pretend.
        digits     = "0";
        hasDecimal = false;
      }
      if(digits_label != NULL) {
        lv_label_set_text(digits_label, digits.c_str());
      }
    }
  }
}

void lvgl_setup(void) {
  // Because they're referenced any time an object is drawn, styles need
  // to be permanent in scope; either declared globally (outside all
  // functions), or static. The styles used on the container and label are
  // never modified after they're used here, so let's use static on those...
  static lv_style_t container_style, label_style;

  // Initialize styles to the "plain" defaults
  lv_style_copy(&container_style, &lv_style_plain);
  lv_style_copy(&label_style, &lv_style_plain);

  // The calculator digits are held inside a LvGL container object
  // as this gives us a little more control over positioning.
  lv_obj_t *container = lv_cont_create(lv_scr_act(), NULL);
  lv_cont_set_fit(container, LV_FIT_NONE); // Don't auto fit
  lv_obj_set_size(container, tft.width(), 50); // Full width x 50 px
  container_style.body.main_color = lv_color_hex(0xC0C0C0); // Gray
  container_style.body.grad_color = lv_color_hex(0x909090); // gradient
  lv_cont_set_style(container, LV_CONT_STYLE_MAIN, &container_style);

  // Calculator digits are just a text label inside the container,
  // refreshed whenever the global "digits" string changes.
  digits_label = lv_label_create(container, NULL);
  lv_label_set_text(digits_label, digits.c_str());
  lv_obj_align(digits_label, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 10);
  lv_label_set_long_mode(digits_label, LV_LABEL_LONG_CROP);
  lv_obj_set_size(digits_label, tft.width() - 40, 30);
  lv_label_set_align(digits_label, LV_LABEL_ALIGN_RIGHT);

  // Fill the remaining space with the button matrix.
  lv_obj_t *button_matrix = lv_btnm_create(lv_scr_act(), NULL);
  lv_btnm_set_map(button_matrix, buttons);
  lv_obj_align(button_matrix, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 50);
  lv_obj_set_size(button_matrix, tft.width(), tft.height() - 50);
  lv_obj_set_event_cb(button_matrix, button_event_handler);
}

#else // Keyboard demo

// Keyboard example, lets you enter and edit text in a field. Even on a
// 480x320 TFT it requires a stylus to be even half useful (fingertip
// is possible if very patient), but having the option of a keyboard at
// all on this device is pretty nifty!

#define TFT_ROTATION 1 // Landscape orientation

lv_obj_t  *textarea,
          *keyboard = NULL; // Created/deleted as needed

// Because they're referenced any time an object is drawn, styles need
// to be permanent in scope.
lv_style_t textarea_style, keyboard_style, pressed_style, released_style;

#if LV_USE_ANIMATION
// Called after keyboard slides closed - deletes keyboard object
void delete_keyboard(lv_anim_t * a) {
  lv_obj_del((lv_obj_t *)a->var);
  keyboard = NULL;
}
#endif

// Called when the close or ok button is pressed on the keyboard
void keyboard_event_handler(lv_obj_t *obj, lv_event_t event) {
  lv_kb_def_event_cb(keyboard, event);
  
  if(event == LV_EVENT_APPLY || event == LV_EVENT_CANCEL) {
#if LV_USE_ANIMATION
    lv_anim_t a;
    a.var = keyboard;
    a.start = lv_obj_get_y(keyboard);
    a.end = LV_VER_RES;
    a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
    a.path_cb = lv_anim_path_ease_in_out;
    a.ready_cb = delete_keyboard;
    a.act_time = 0;
    a.time = 300;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;
    lv_anim_create(&a);
#else
    lv_obj_del(keyboard);
    keyboard = NULL;
#endif
  }
}

// Other clicks in the text area
void text_area_event_handler(lv_obj_t *obj, lv_event_t event) {
  lv_obj_t *parent = lv_obj_get_parent(obj);

  if(event == LV_EVENT_CLICKED) {
    if(keyboard == NULL) {
      // If not present, create keyboard object at bottom of screen
      keyboard = lv_kb_create(lv_scr_act(), NULL);
      lv_obj_set_size(keyboard, tft.width(), tft.height() * 7 / 16);
      lv_obj_align(keyboard, textarea, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
      lv_kb_set_ta(keyboard, textarea);
      lv_kb_set_cursor_manage(keyboard, true);
      lv_kb_set_style(keyboard, LV_KB_STYLE_BG, &keyboard_style);
      lv_kb_set_style(keyboard, LV_KB_STYLE_BTN_PR, &pressed_style);
      lv_kb_set_style(keyboard, LV_KB_STYLE_BTN_REL, &released_style);
      lv_obj_set_event_cb(keyboard, keyboard_event_handler);

#if LV_USE_ANIMATION
      // If animation is enabled, make keyboard slide into place
      lv_anim_t a;
      a.var = keyboard;
      a.start = LV_VER_RES;
      a.end = lv_obj_get_y(keyboard);
      a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
      a.path_cb = lv_anim_path_ease_in_out;
      a.ready_cb = NULL;
      a.act_time = 0;
      a.time = 300;
      a.playback = 0;
      a.playback_pause = 0;
      a.repeat = 0;
      a.repeat_pause = 0;
      lv_anim_create(&a);
#endif
    }
  }
}

void lvgl_setup(void) {
  // Copy the "plain" style to our textarea style variable. No gradient,
  // no rounded corners, etc. Can tweak things here if you want, that's
  // why I'm not just assigning the plain style directly to the textarea.
  lv_style_copy(&textarea_style, &lv_style_plain);

  textarea = lv_ta_create(lv_scr_act(), NULL);
  lv_obj_set_size(textarea, LV_HOR_RES, LV_VER_RES); // Whole screen
  lv_obj_align(textarea, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_ta_set_cursor_type(textarea, LV_CURSOR_BLOCK);
  lv_ta_set_style(textarea, LV_TA_STYLE_BG, &textarea_style);
  lv_ta_set_text(textarea, "This text is editable.");
  lv_obj_set_event_cb(textarea, text_area_event_handler);
  lv_ta_set_cursor_pos(textarea, LV_TA_CURSOR_LAST);

  lv_style_copy(&keyboard_style, &lv_style_plain);
  keyboard_style.body.padding.left   = 2;
  keyboard_style.body.padding.right  = 2;
  keyboard_style.body.padding.top    = 2;
  keyboard_style.body.padding.bottom = 2;
  keyboard_style.body.padding.inner  = 0;

  // Initialize key styles to the "button" defaults, override the
  // radius and border settings so the keys pack better.
  // The keyboard widget isn't actually created until needed.
  // But the styles, being global and unchanging, only need to
  // be set up once.
  lv_style_copy(&pressed_style , &lv_style_btn_pr);
  lv_style_copy(&released_style, &lv_style_btn_rel);
  pressed_style.body.radius        = 0;
  pressed_style.body.border.width  = 1;
  released_style.body.radius       = 0;
  released_style.body.border.width = 1;
}

#endif // end calculator / keyboard examples

void setup(void) {
  Serial.begin(115200);

  // Initialize display and touchscreen BEFORE glue setup
  tft.begin();
  tft.setRotation(TFT_ROTATION);
  if(!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    for(;;);
  }

  // Initialize glue, passing in address of display & touchscreen
  LvGLStatus status = glue.begin(&tft, &ts);
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

// NOTE TO FUTURE SELF: this sketch is largely similar to the PyPortal
// widgets example. If updating/bugfixing one sketch, make sure the other
// is kept in sync.
