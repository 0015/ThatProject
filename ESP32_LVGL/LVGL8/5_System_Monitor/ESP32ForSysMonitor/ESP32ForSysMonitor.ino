/////////////////////////////////////////////////////////////////
/*
  Make your own System Monitor with ESP32 + LVGL 8
  For More Information: https://youtu.be/gliwNg25fLE
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

////////////////////
// Arduino IDE
// ESP v2.0.3
// LVGL v8.2.0
////////////////////

#include <ArduinoJson.h> //https://arduinojson.org/v6/doc/installation/
#include <ESP32Time.h>   //https://github.com/fbiego/ESP32Time
#include <lvgl.h>        //https://github.com/lvgl/lvgl
 
#define LGFX_USE_V1      //https://github.com/lovyan03/LovyanGFX
#include <LovyanGFX.hpp>
#include <FT6236.h>      //https://github.com/DustinWatts/FT6236
#define SDA_FT6236 21
#define SCL_FT6236 22
FT6236 ts = FT6236();

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;

public:
  LGFX(void) {
    {                                        
      auto cfg = _bus_instance.config();     
      cfg.spi_host = VSPI_HOST;              
      cfg.spi_mode = 0;                      
      cfg.freq_write = 40000000;             
      cfg.freq_read = 16000000;              
      cfg.spi_3wire = false;                 
      cfg.use_lock = true;                   
      cfg.dma_channel = 1;                   
      cfg.pin_sclk = 18;                     
      cfg.pin_mosi = 19;                     
      cfg.pin_miso = 23;                     
      cfg.pin_dc = 27;                       
      _bus_instance.config(cfg);             
      _panel_instance.setBus(&_bus_instance);
    }

    {                                       
      auto cfg = _panel_instance.config();  
      cfg.pin_cs = 5;                      
      cfg.pin_rst = -1;                    
      cfg.pin_busy = -1;                   
      cfg.memory_width = 320;              
      cfg.memory_height = 480;             
      cfg.panel_width = 320;               
      cfg.panel_height = 480;              
      cfg.offset_x = 0;                    
      cfg.offset_y = 0;                    
      cfg.offset_rotation = 0;             
      cfg.dummy_read_pixel = 8;            
      cfg.dummy_read_bits = 1;             
      cfg.readable = true;                 
      cfg.invert = false;                  
      cfg.rgb_order = false;               
      cfg.dlen_16bit = false;              
      cfg.bus_shared = true;               

      _panel_instance.config(cfg);
    }

    {                                      
      auto cfg = _light_instance.config(); 

      cfg.pin_bl = 12;      
      cfg.invert = false;   
      cfg.freq = 44100;     
      cfg.pwm_channel = 7;  

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance); 
    }

    setPanel(&_panel_instance); 
  }
};

LGFX tft;

ESP32Time rtc;

StaticJsonDocument<512> doc;

struct queue_t {
  String data_string;
};
static QueueHandle_t data_queue;

/*Change to your screen resolution*/
static const uint32_t screenWidth = 480;
static const uint32_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];
int prev_cpu_usage = 0;
int prev_mem_usage = 0;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (ts.touched()) {
    data->state = LV_INDEV_STATE_PR;
    TS_Point p = ts.getPoint();
    data->point.x = tft.width() - p.y;
    data->point.y = p.x;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(3);
  tft.setBrightness(255);

  if (!ts.begin(40, SDA_FT6236, SCL_FT6236)) {
    Serial.println("Unable to start the capacitive touch Screen.");
  }

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  lv_main();

  data_queue = xQueueCreate(8, sizeof(queue_t));

  xTaskCreatePinnedToCore(lvglTimerHandler,
                          "lvglTimerHandler",
                          4096,
                          NULL,
                          2,
                          NULL,
                          1);

  xTaskCreatePinnedToCore(serialTask,
                          "serialTask",
                          2048,
                          NULL,
                          1,
                          NULL,
                          0);
}

void lvglTimerHandler(void *pvParameters) {
  queue_t q;

  while (1) {
    if (xQueueReceive(data_queue, &q, 0) == pdTRUE) {
      dataParser(q.data_string);
      loading_spinner_delete();
    }
    lv_timer_handler();
    vTaskDelay(5);
  }
}

void loop() {}

static lv_obj_t *loading_screen;
static lv_obj_t *meter;
static lv_obj_t *date_time_label;
static lv_obj_t *cpu_label;
static lv_obj_t *mem_label;
static lv_obj_t *battery_label;
static lv_meter_indicator_t *cpu_indic;
static lv_meter_indicator_t *mem_indic;
static lv_obj_t *process_list;
static lv_anim_t a;

static void set_value(void *indic, int32_t v) {
  lv_meter_set_indicator_end_value(meter, (lv_meter_indicator_t *)indic, v);
}

void lv_main(void) {
  meter = lv_meter_create(lv_scr_act());
  lv_obj_align(meter, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  lv_obj_set_size(meter, 280, 280);
  lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);

  lv_meter_scale_t *scale = lv_meter_add_scale(meter);
  lv_meter_set_scale_ticks(meter, scale, 11, 2, 10, lv_palette_main(LV_PALETTE_GREY));
  lv_meter_set_scale_major_ticks(meter, scale, 1, 2, 30, lv_color_hex3(0xeee), 15);
  lv_meter_set_scale_range(meter, scale, 0, 100, 270, 90);

  cpu_indic = lv_meter_add_arc(meter, scale, 10, lv_palette_main(LV_PALETTE_RED), 0);
  mem_indic = lv_meter_add_arc(meter, scale, 10, lv_palette_main(LV_PALETTE_BLUE), -20);

  lv_anim_init(&a);
  lv_anim_set_exec_cb(&a, set_value);

  static lv_style_t style_red;
  lv_style_init(&style_red);
  lv_style_set_radius(&style_red, 5);
  lv_style_set_bg_opa(&style_red, LV_OPA_COVER);
  lv_style_set_bg_color(&style_red, lv_palette_main(LV_PALETTE_RED));
  lv_style_set_outline_width(&style_red, 2);
  lv_style_set_outline_color(&style_red, lv_palette_main(LV_PALETTE_RED));
  lv_style_set_outline_pad(&style_red, 4);

  static lv_style_t style_blue;
  lv_style_init(&style_blue);
  lv_style_set_radius(&style_blue, 5);
  lv_style_set_bg_opa(&style_blue, LV_OPA_COVER);
  lv_style_set_bg_color(&style_blue, lv_palette_main(LV_PALETTE_BLUE));
  lv_style_set_outline_width(&style_blue, 2);
  lv_style_set_outline_color(&style_blue, lv_palette_main(LV_PALETTE_BLUE));
  lv_style_set_outline_pad(&style_blue, 4);

  lv_obj_t *red_obj = lv_obj_create(lv_scr_act());
  lv_obj_set_size(red_obj, 10, 10);
  lv_obj_add_style(red_obj, &style_red, 0);
  lv_obj_align(red_obj, LV_ALIGN_CENTER, -60, 80);

  cpu_label = lv_label_create(lv_scr_act());
  lv_obj_set_width(cpu_label, 200);
  lv_label_set_text(cpu_label, "CPU Usage: 0%");
  lv_obj_align_to(cpu_label, red_obj, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

  lv_obj_t *blue_obj = lv_obj_create(lv_scr_act());
  lv_obj_set_size(blue_obj, 10, 10);
  lv_obj_add_style(blue_obj, &style_blue, 0);
  lv_obj_align(blue_obj, LV_ALIGN_CENTER, -60, 110);

  mem_label = lv_label_create(lv_scr_act());
  lv_obj_set_width(mem_label, 200);
  lv_label_set_text(mem_label, "MEM Usage: 0%");
  lv_obj_align_to(mem_label, blue_obj, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

  date_time_label = lv_label_create(lv_scr_act());
  lv_obj_set_style_text_align(date_time_label, LV_TEXT_ALIGN_RIGHT, 0);
  lv_obj_align(date_time_label, LV_ALIGN_TOP_RIGHT, -10, 0);
  lv_label_set_text(date_time_label, "Mon 1:25 PM");

  battery_label = lv_label_create(lv_scr_act());
  lv_label_set_text(battery_label, LV_SYMBOL_CHARGE "  0%");
  lv_obj_set_style_text_align(battery_label, LV_TEXT_ALIGN_RIGHT, 0);
  lv_obj_align(battery_label, LV_ALIGN_TOP_RIGHT, -130, 0);

  process_list = lv_list_create(lv_scr_act());
  lv_obj_set_size(process_list, 240, 190);
  lv_obj_align(process_list, LV_ALIGN_TOP_RIGHT, 0, 30);
  lv_obj_set_style_pad_row(process_list, -10, 0);
  lv_obj_set_style_pad_hor(process_list, -6, 0);

  loading_spinner_make();
}

void loading_spinner_make() {

  loading_screen = lv_obj_create(lv_scr_act());
  lv_obj_set_size(loading_screen, 480, 320);
  lv_obj_center(loading_screen);

  lv_obj_t *loading_spinner = lv_spinner_create(loading_screen, 1000, 60);
  lv_obj_set_size(loading_spinner, 240, 240);
  lv_obj_center(loading_spinner);

  lv_obj_t *loading_label = lv_label_create(loading_spinner);
  lv_label_set_text(loading_label, "Waiting for Data...");
  lv_obj_center(loading_label);
}

void loading_spinner_delete() {
  if (loading_screen != NULL) {
    lv_obj_del(loading_screen);
    loading_screen = NULL;
  }
}

void serialTask(void *pvParameters) {
  queue_t q;
  while (1) {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      q.data_string = input;
      xQueueSend(data_queue, &q, portMAX_DELAY);
    }
    vTaskDelay(100);
  }
}

void dataParser(String jsonData) {
  Serial.println(jsonData);

  if (jsonData.length() < 10)
    return;

  DeserializationError error = deserializeJson(doc, jsonData);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  long local_time = doc["local_time"];
  long utc_offset = doc["utc_offset"];
  update_time(local_time, utc_offset);

  float cpu_percent_total = doc["cpu_percent_total"];
  float mem_percent = doc["mem_percent"];
  update_cpu_mem_usage(cpu_percent_total, mem_percent);

  float battery_percent = doc["battery_percent"];
  bool power_plugged = doc["power_plugged"];
  update_battery(battery_percent, power_plugged);

  update_process(doc["cpu_top5_process"]);
}

void update_time(long local_time, long utc_offset) {
  rtc.setTime(local_time);
  rtc.offset = utc_offset;
  lv_label_set_text(date_time_label, rtc.getTime("%a %I:%M %p").c_str());
}

void update_cpu_mem_usage(float cpu_percent_total, float mem_percent) {
  String cpu_text = String("CPU Usage: ") + cpu_percent_total + "%";
  lv_label_set_text(cpu_label, cpu_text.c_str());

  lv_anim_set_var(&a, cpu_indic);
  lv_anim_set_time(&a, 500);
  lv_anim_set_values(&a, prev_cpu_usage, int(cpu_percent_total));
  lv_anim_start(&a);
  prev_cpu_usage = int(cpu_percent_total);

  String mem_text = String("MEM Usage: ") + mem_percent + "%";
  lv_label_set_text(mem_label, mem_text.c_str());

  lv_anim_set_var(&a, mem_indic);
  lv_anim_set_time(&a, 500);
  lv_anim_set_values(&a, prev_mem_usage, int(mem_percent));
  lv_anim_start(&a);
  prev_mem_usage = int(mem_percent);
}

void update_battery(float battery_percent, bool power_plugged) {
  char buffer[16];
  if (power_plugged) {
    sprintf(buffer, "\xEF\x83\xA7  %d %%", int(battery_percent));
  } else {
    if (battery_percent >= 95) {
      sprintf(buffer, "\xEF\x89\x80  %d %%", int(battery_percent));
    } else if (battery_percent >= 75 && battery_percent < 95) {
      sprintf(buffer, "\xEF\x89\x81  %d %%", int(battery_percent));
    } else if (battery_percent >= 50 && battery_percent < 75) {
      sprintf(buffer, "\xEF\x89\x82  %d %%", int(battery_percent));
    } else if (battery_percent >= 20 && battery_percent < 50) {
      sprintf(buffer, "\xEF\x89\x83  %d %%", int(battery_percent));
    } else {
      sprintf(buffer, "\xEF\x89\x84  %d %%", int(battery_percent));
    }
  }

  lv_label_set_text(battery_label, buffer);
}

void update_process(JsonArray processes) {
  int arraySize = processes.size();
  if (arraySize > 0) {
    lv_obj_clean(process_list);
    for (int i = 0; i < arraySize; i++) {
      lv_obj_t *list_item_button = lv_list_add_btn(process_list, LV_SYMBOL_WARNING, processes[i]);
      lv_obj_t *child = lv_obj_get_child(list_item_button, 1);
      lv_label_set_long_mode(child, LV_LABEL_LONG_WRAP);
    }
  }
}