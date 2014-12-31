/*

   "Classic" Digital Watch Pebble App

 */

// Standard includes
#include "pebble.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"


// App-specific data
Window *window; // All apps must have at least one window
TextLayer *time_layer; // The clock
TextLayer *deci;
TextLayer *line;
TextLayer *realtime;
int second_shown;
int real_show;
int how_many;
static GFont custom_font;
enum {
  SECONDS = 0x0,
  R_TIME = 0x1
};
static AppSync sync;
static uint8_t sync_buffer[124];

// Called once per second
static void handle_second_tick(struct tm* tick_time, TimeUnits units_changed) {

  static char time_text[] = "hh"; // Needs to be static because it's used by the system later.
  static char time_min[] = "mm";
  static char time_sec[] = "ss";
  static char time_pam[] = "pm";
  static char d_time[] = "hh:mm:ss";
  static char real_time[] = "hh:mm:ss pm";
  strftime(time_text, sizeof(time_text), "%T", tick_time);
  strftime(time_min, sizeof(time_min), "%MM", tick_time);
  strftime(time_sec, sizeof(time_sec), "%SS", tick_time);
  strftime(time_pam, sizeof(time_pam), "%p", tick_time);

  int day_second = ((atoi(time_text) * 60 + atoi(time_min)) * 60 + atoi(time_sec));
  int d_hour = day_second / 8640;
  day_second = day_second % 8640;
  int d_minute = (int) floor(day_second / 86.4);
  day_second = (int) floor(day_second- d_minute*86.4);
  int d_second = (int) floor(day_second/0.864);
  //char timeywhimy = itoas(d_hour);
  if(real_show != 0) {
    layer_set_hidden((Layer *)realtime, false);
    if(real_show ==1) {
      if(atoi(time_text) > 12){
        snprintf(time_text, sizeof(time_text), "%d", atoi(time_text)-12 );
      }
    }
    if(atoi(time_min)< 10){
      snprintf(time_min, sizeof(time_min), "0%d", atoi(time_min));
    }
    if(atoi(time_sec)<10) {
      snprintf(time_sec,sizeof(time_sec), "0%d", atoi(time_sec));
    }
    if(real_show ==1 && second_shown == 1) {
      snprintf(real_time, sizeof(real_time), "%s:%s:%s %c", time_text, time_min,time_sec,time_pam[0]);
    } else if(real_show == 2 && second_shown == 1) {
      snprintf(real_time, sizeof(real_time), "%s:%s:%s", time_text, time_min,time_sec);
    } else if(real_show == 1 && second_shown == 0) {
      snprintf(real_time, sizeof(real_time), "%s:%s %c", time_text, time_min,time_pam[0]);
    } else {
      snprintf(real_time, sizeof(real_time), "%s:%s", time_text, time_min);
    }
    text_layer_set_text(realtime, real_time);
  } else {
    layer_set_hidden((Layer *)realtime, true);
  }
  switch (second_shown){
    case 1:
      if(d_minute <10 && d_second >9){
      snprintf(d_time, sizeof(d_time), "%d.0%d.%d",d_hour,d_minute,d_second);
      
      }
      else if(d_minute < 10 && d_second < 10){
        snprintf(d_time, sizeof(d_time), "%d.0%d.0%d",d_hour,d_minute,d_second);
      }
      else if(d_minute > 10 && d_second < 10) {
       snprintf(d_time, sizeof(d_time), "%d.%d.0%d",d_hour,d_minute,d_second);
      }
      else {
       snprintf(d_time, sizeof(d_time), "%d.%d.%d",d_hour,d_minute,d_second);
      }
      text_layer_set_text(time_layer, d_time);
      //text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
      text_layer_set_font(time_layer, custom_font);
      break;
    default:
      if(d_minute <10){
      snprintf(d_time, sizeof(d_time), "%d.0%d",d_hour,d_minute);
      
      }
      else {
       snprintf(d_time, sizeof(d_time), "%d.%d",d_hour,d_minute);
      }
      text_layer_set_text(time_layer, d_time);
      //text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
      text_layer_set_font(time_layer, custom_font);
  }
}

void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  
}

void sync_tuple_changed_callback(const uint32_t key,
        const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  how_many += 1;
  static char good[] = "dd";
  switch (key) {
    case SECONDS:
      second_shown = new_tuple->value->uint8;
      break;
    case R_TIME:
      real_show = new_tuple->value->uint8;
      break;

  }
}



// Handle the start-up of the app
static void do_init(void) {
  const int inbound_size = 128;
  const int outbound_size = 128;
  app_message_open(inbound_size, outbound_size);
  // Create our app's base window
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);
  custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OSP_DIN_60));

  // Init the text layer used to show the time
  time_layer = text_layer_create(GRect(0, 58, 144 /* width */, 65 /* height */));
  text_layer_set_text(time_layer, "9.99.99");
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  deci = text_layer_create(GRect(0,3,144,25));
  line = text_layer_create(GRect(0,27,144,2));
  realtime = text_layer_create(GRect(0, 142, 144, 25));
  text_layer_set_text_alignment(realtime, GTextAlignmentCenter);
  text_layer_set_background_color(realtime, GColorClear);
  text_layer_set_text_color(realtime, GColorWhite);
  text_layer_set_font(realtime, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(realtime,"cTime");
  text_layer_set_background_color(line, GColorWhite);
  text_layer_set_text_alignment(deci, GTextAlignmentCenter);
  text_layer_set_text_color(deci, GColorWhite);
  text_layer_set_background_color(deci, GColorClear);
  text_layer_set_font(deci, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(deci, "Decimal.time");
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  Tuplet initial_values[] = {
     TupletInteger(SECONDS, 1),
     TupletInteger(R_TIME, 0)
  };
  how_many = 0;
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);
  //app_message_register_inbox_received(in_received_handler);
  //text_layer_set_font(time_layer, custom_font);
  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_second_tick(current_time, SECOND_UNIT);
  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(deci));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(line));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(realtime));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}



static void do_deinit(void) {
  app_sync_deinit(&sync);
  text_layer_destroy(time_layer);
  text_layer_destroy(deci);
  window_destroy(window);
}

// The main event/run loop for our app
int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
