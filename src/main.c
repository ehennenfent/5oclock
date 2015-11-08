#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_name_layer;
static TextLayer *s_shift_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
const char *zones[24] = {"American Samoa", "Honolulu", "Ketchikan", "Las Vegas", "Denver", "Chicago", "Miami", "Puerto Rico", "Rio de Janiero", "Fernando de Noronha", "Cape Verde", "London", "Paris", "Cairo", "Moscos", "Dubai", "Pakistan", "Bangledash", "Bangkok", "Manila", "Tokyo", "Sydney", "Solomon Islands", "New Zealend"};
// const char *shifts[24] = {"-11","-10","-9","-8","-7","-6","-5","-4","-3","-2","-1","+0","+1","+2","3","+4","+5","+6","+7","+8","+9","+10","+11","+12"};

static int calculate_zone(int hour){
  int shift = 11 + (17 - hour);
  if (shift < 0){
    shift = shift + 24;
  }
  if (shift > 23){
    shift = shift - 24;
  }
  return shift;
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = gmtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%I:%M" : "%H:%M", tick_time);
  
  int london_hour = tick_time->tm_hour;
  int zone = calculate_zone(london_hour);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Time in London: %d", london_hour);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Time zone: %d", zone);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_name_layer, zones[zone]);
//   text_layer_set_text(s_shift_layer, shifts[zone]);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Set up Time Layer
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "5:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Set up Name Layer
  s_name_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 128), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_name_layer, GColorClear);
  text_layer_set_text_color(s_name_layer, GColorWhite);
  text_layer_set_text(s_name_layer, "American Samoa");
  text_layer_set_font(s_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_name_layer, GTextAlignmentCenter);
  
  // Set up Shift Layer
//   s_name_layer = text_layer_create(
//       GRect(0, PBL_IF_ROUND_ELSE(58, 138), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
//   text_layer_set_background_color(s_shift_layer, GColorClear);
//   text_layer_set_text_color(s_shift_layer, GColorWhite);
//   text_layer_set_text(s_shift_layer, "UTC+0");
//   text_layer_set_font(s_shift_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
//   text_layer_set_text_alignment(s_shift_layer, GTextAlignmentCenter);
  
    // Create GBitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SAMOA_BMP);

  // Create BitmapLayer to display the GBitmap
  s_background_layer = bitmap_layer_create(bounds);

  // Set the bitmap onto the layer and add to the window
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_name_layer));
//   layer_add_child(window_layer, text_layer_get_layer(s_shift_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_name_layer);
//   text_layer_destroy(s_shift_layer);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Make sure the time is displayed from the start
  update_time();

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}