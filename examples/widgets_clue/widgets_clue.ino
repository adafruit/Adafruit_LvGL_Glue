// A more functional example for LittlevGL on Adafruit CLUE. Lacking
// touchscreen input, interaction options are limited (but not impossible).
// In this case we'll pretend like it's a status display for something --
// the left and right buttons switch between three different tabs, each
// displaying different information. The code's a bit more complex than
// the hello_clue example, so best get that working before trying this.

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

lv_obj_t          *tabview,        // LittlevGL tabview object
                  *gauge,          // Gauge object (on first of three tabs)
                  *chart,          // Chart object (second tab)
                  *canvas;         // Canvas object (third tab)
uint8_t            active_tab = 0, // Index of currently-active tab (0-2)
                   prev_tab   = 0; // Index of previously-active tab
lv_chart_series_t *series;         // 'Series' data for the bar chart

// Canvas object is an image in memory. Although LittlevGL supports several
// internal image formats, only the TRUE_COLOR variety allows drawing
// operations. Fortunately nRF52840 has gobs of RAM for this. The canvas
// buffer is global because it's referenced both in the setup and main loop.
#define CANVAS_WIDTH  200 // Dimensions in pixels
#define CANVAS_HEIGHT 150
lv_color_t canvas_buffer[
  LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];
lv_style_t draw_style; // Drawing style (for canvas) is similarly global
  
void lvgl_setup(void) {
  // Create a tabview object, by default this covers the full display.
  tabview = lv_tabview_create(lv_disp_get_scr_act(NULL), NULL);
  // The CLUE display has a lot of pixels and can't refresh very fast.
  // To show off the tabview animation, let's slow it down to 1 second.
  lv_tabview_set_anim_time(tabview, 1000);

  // Because they're referenced any time an object is drawn, styles need
  // to be permanent in scope; either declared globally (outside all
  // functions), or static. The styles used on tabs are never modified after
  // they're used here, so let's use static on those...
  static lv_style_t tab_on_style, tab_off_style, tab_background_style,
                    indicator_style;

  // This is the background style "behind" the tabs. Later we'll set up
  // a foreground style that's transparent, so this is what shows through
  // for "off" (inactive) tabs:
  lv_style_copy(&tab_background_style, &lv_style_plain); // Init defaults
  tab_background_style.body.main_color = lv_color_hex(0x408040); // Green
  tab_background_style.body.grad_color = lv_color_hex(0x304030); // gradient
  tab_background_style.body.padding.top = 0; // Minimize size on screen
  tab_background_style.body.padding.bottom = 0;
  lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_BG,
    &tab_background_style);

  // Style for "off" (inactive) tabs. It's set transparent so the background
  // shows through; only the white text is seen.
  lv_style_copy(&tab_off_style, &lv_style_plain); // Init defaults
  tab_off_style.body.opa = LV_OPA_TRANSP;
  tab_off_style.text.color = LV_COLOR_WHITE;
  lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_REL, &tab_off_style);
  lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_TGL_REL, &tab_off_style);

  // This should be the style for an "on" (active) tab. For whatever reason
  // I don't yet understand, this isn't rendering. Fortunately the indicator
  // (below) gives some visual feedback on which tab is active.
  lv_style_copy(&tab_on_style, &lv_style_plain); // Init defaults
  tab_on_style.body.opa = LV_OPA_COVER;
  tab_on_style.body.main_color = tab_on_style.body.grad_color = LV_COLOR_WHITE;
  tab_on_style.text.color = LV_COLOR_GRAY;
  lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_PR, &tab_on_style);
  lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_BTN_TGL_PR, &tab_on_style);

  // Style for the small indicator bar that appears below the active tab.
  lv_style_copy(&indicator_style, &lv_style_plain); // Init defaults
  indicator_style.body.main_color = indicator_style.body.grad_color =
    LV_COLOR_RED;
  lv_tabview_set_style(tabview, LV_TABVIEW_STYLE_INDIC, &indicator_style);

  // Back to creating widgets...

  // Add three tabs to the tabview
  lv_obj_t *tab1 = lv_tabview_add_tab(tabview, "Gauge");
  lv_obj_t *tab2 = lv_tabview_add_tab(tabview, "Chart");
  lv_obj_t *tab3 = lv_tabview_add_tab(tabview, "Canvas");

  // And then add stuff in each tab...

  // The first tab holds a gauge. To keep the demo simple, let's just use
  // the default style and range (0-100). See LittlevGL docs for options.
  gauge = lv_gauge_create(tab1, NULL);
  lv_obj_set_size(gauge, 200, 180);
  lv_obj_align(gauge, NULL, LV_ALIGN_CENTER, 0, 0);

  // Second tab, make a chart...
  chart = lv_chart_create(tab2, NULL);
  lv_obj_set_size(chart, 200, 180);
  lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_COLUMN);
  // For simplicity, we'll stick with the chart's default 10 data points:
  series = lv_chart_add_series(chart, LV_COLOR_RED);
  lv_chart_init_points(chart, series, 0);
  // Make each column shift left as new values enter on right:
  lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);

  // Third tab is a canvas, which we'll fill with random colored lines.
  // LittlevGL draw functions only work on TRUE_COLOR canvas.
  canvas = lv_canvas_create(tab3, NULL);
  lv_canvas_set_buffer(canvas, canvas_buffer,
    CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
  lv_obj_align(canvas, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_canvas_fill_bg(canvas, LV_COLOR_WHITE);
  
  // Set up canvas line-drawing style based on the "plain" defaults.
  // Later we'll fiddle with the color settings when drawing each line.
  lv_style_copy(&draw_style, &lv_style_plain);
}

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

  lvgl_setup(); // Call UI-building function above

  // Enable "A" and "B" buttons as inputs
  pinMode(PIN_BUTTON1, INPUT_PULLUP);
  pinMode(PIN_BUTTON2, INPUT_PULLUP);
}

uint32_t prev_time = -1;

void loop(void) {

  // Read left/right button inputs. Debouncing could be done better,
  // but this'll do for a quick simple demo...
  if(digitalRead(PIN_BUTTON1) == LOW) {
    if(active_tab > 0) {
      active_tab--;
    }
    while(digitalRead(PIN_BUTTON1) == LOW); // Wait for button release
  } else if(digitalRead(PIN_BUTTON2) == LOW) {
    if(active_tab < 2) {
      active_tab++;
    }
    while(digitalRead(PIN_BUTTON2) == LOW); // Wait for button release
  }

  // Change active tab if button pressings happened
  if(active_tab != prev_tab) {
    lv_tabview_set_tab_act(tabview, active_tab, true);
    prev_tab = active_tab;
  }

  // Make the gauge sweep a full sine wave over time
  lv_gauge_set_value(gauge, 0, (int)(50.5 + sin(millis() / 1000.0) * 50.0));

  // About 2X a second, things happen on the other two tabs...
  uint32_t new_time = millis() / 500; // Current half-second
  if(new_time != prev_time) {         // freshly elapsed
    prev_time = new_time;

    // Add a new random item to the bar chart (old value shift left)
    lv_chart_set_next(chart, series, random(100));
    lv_chart_refresh(chart);

    // Add a random line to the canvas
    lv_point_t points[2];
    points[0].x = random(CANVAS_WIDTH);
    points[0].y = random(CANVAS_HEIGHT);
    points[1].x = random(CANVAS_WIDTH);
    points[1].y = random(CANVAS_HEIGHT);
    draw_style.line.color.ch.red   = random();
    draw_style.line.color.ch.green = random();
    draw_style.line.color.ch.blue  = random();
    lv_canvas_draw_line(canvas, points, 2, &draw_style);
    // This forces the canvas to update (otherwise changes aren't
    // seen unless leaving and returning to the canvas tab):
    lv_canvas_set_buffer(canvas, canvas_buffer,
      CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
  }

  lv_task_handler(); // Call LittleVGL task handler periodically
  delay(5);
}
