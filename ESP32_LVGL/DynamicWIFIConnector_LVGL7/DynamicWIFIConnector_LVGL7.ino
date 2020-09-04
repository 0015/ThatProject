#include "WiFi.h"
#include <ESP32Ping.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

int screenWidth = 320;
int screenHeight = 480;
static lv_color_t buf[LV_HOR_RES_MAX * LV_VER_RES_MAX / 10];
static lv_disp_buf_t disp_buf;

TaskHandle_t ntScanTaskHandler;
TaskHandle_t ntConnectTaskHandler;

String ssidName, password;
const char* remote_host = "www.google.com";
unsigned long timeout = 10000; // 10sec
int pingCount = 5; 

/* LVGL objects */
static lv_obj_t * ddlist;
static lv_obj_t * bg_top;
static lv_obj_t * bg_middle;
static lv_obj_t * bg_bottom;
static lv_obj_t * ta_password;
static lv_obj_t * kb;
static lv_obj_t * mbox_connect;
static lv_obj_t * gauge;
static lv_obj_t * label_status;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint16_t c;

  tft.startWrite(); /* Start new TFT transaction */
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite(); /* terminate TFT transaction */
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    uint16_t touchX, touchY;

    bool touched = tft.getTouch(&touchX, &touchY, 600);

    if(!touched)
    {
      return false;
    }

    if(touchX>screenWidth || touchY > screenHeight)
    {
      Serial.println("Y or y outside of expected parameters..");
      Serial.print("y:");
      Serial.print(touchX);
      Serial.print(" x:");
      Serial.print(touchY);
    }
    else
    {

      data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
      data->point.x = touchX;
      data->point.y = touchY;
    }

    return false; /*Return `false` because we are not buffering and no more data to read*/
}

void setup() {
  Serial.begin(115200);
  
  xTaskCreate(guiTask,
                  "gui",
                  4096*2,
                  NULL,
                  2,
                  NULL);

  networkScanner();                 
}


void networkScanner(){
  vTaskDelay(500);
  xTaskCreate(scanWIFITask,
                "ScanWIFITask",
                4096,
                NULL,
                1,
                &ntScanTaskHandler);
}

void loop() {}

void guiTask(void *pvParameters) {
    
    lv_init();

    tft.begin();
    tft.setRotation(2);

    uint16_t calData[5] = { 295, 3493, 320, 3602, 2 };
    tft.setTouch(calData);


    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX / 10);
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
    lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/
  
    lv_main();

  
    while (1) {
         lv_task_handler();
    }
}

static void lv_main(){
    
    //LV_THEME_MATERIAL_FLAG_LIGHT
    //LV_THEME_MATERIAL_FLAG_DARK
    
    lv_theme_t * th = lv_theme_material_init(LV_THEME_DEFAULT_COLOR_PRIMARY, LV_THEME_DEFAULT_COLOR_SECONDARY, LV_THEME_MATERIAL_FLAG_LIGHT, LV_THEME_DEFAULT_FONT_SMALL , LV_THEME_DEFAULT_FONT_NORMAL, LV_THEME_DEFAULT_FONT_SUBTITLE, LV_THEME_DEFAULT_FONT_TITLE);     
    lv_theme_set_act(th);

    lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    bg_top = lv_obj_create(scr, NULL);
    lv_obj_clean_style_list(bg_top, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_COLOR_NAVY);
    lv_obj_set_size(bg_top, LV_HOR_RES, 50);
    
    bg_middle = lv_obj_create(scr, NULL);
    lv_obj_clean_style_list(bg_middle, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_middle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_middle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_COLOR_WHITE);
    lv_obj_set_pos(bg_middle, 0, 50);
    lv_obj_set_size(bg_middle, LV_HOR_RES, 400);

    bg_bottom = lv_obj_create(scr, NULL);
    lv_obj_clean_style_list(bg_bottom, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_bottom, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_bottom, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_COLOR_ORANGE);
    lv_obj_set_pos(bg_bottom, 0, 450);
    lv_obj_set_size(bg_bottom, LV_HOR_RES, 30);

    label_status = lv_label_create(bg_bottom, NULL);
    lv_label_set_long_mode(label_status, LV_LABEL_LONG_SROLL_CIRC);
    lv_obj_set_width(label_status, LV_HOR_RES - 20);
    lv_label_set_text(label_status, "");
    lv_obj_align(label_status, NULL, LV_ALIGN_CENTER, 0, 0);
    
    makeDropDownList();
    makeGaugeIndicator();
    makeKeyboard();
    makePWMsgBox();
 }

static void updateBottomStatus(lv_color_t color, String text){
  lv_obj_set_style_local_bg_color(bg_bottom, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,color);
  lv_label_set_text(label_status, text.c_str());
}

static void makeDropDownList(void){

    ddlist = lv_dropdown_create(bg_top, NULL);
    lv_dropdown_set_show_selected(ddlist, false);
    lv_dropdown_set_text(ddlist, "WIFI");
    lv_dropdown_set_options(ddlist, "...Searching...");
    lv_obj_align(ddlist, NULL, LV_ALIGN_IN_TOP_LEFT, 4, 8);
    lv_obj_set_event_cb(ddlist, dd_event_handler);
}

static void dd_event_handler(lv_obj_t * obj, lv_event_t event){
  
  if(event == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        ssidName = String(buf);
        
        for (int i = 0; i < ssidName.length()-1; i++) {
          if (ssidName.substring(i, i+2) == " (") {
              ssidName = ssidName.substring(0, i);
            break;
          }
        }
        
        popupPWMsgBox();
    }
}

static void makeGaugeIndicator(void){
    gauge = lv_gauge_create(bg_middle, NULL);

    lv_obj_set_size(gauge, 200, 200);
    lv_obj_align(gauge, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_gauge_set_value(gauge, 0, 100);

    lv_obj_t * label = lv_label_create(gauge, NULL);
    lv_obj_align(label, gauge, LV_ALIGN_CENTER, 0, 70);
    lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_theme_get_font_title());

    setGaugeValue(100, 0);
}

static void setGaugeValue(float prev, float curt){
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, gauge);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)gauge_anim);
    lv_anim_set_values(&a, prev, curt);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_repeat_count(&a, 0);
    lv_anim_start(&a);
}

static void gauge_anim(lv_obj_t * gauge, lv_anim_value_t value){
    lv_gauge_set_value(gauge, 0, value);

    static char buf[64];
    lv_snprintf(buf, sizeof(buf), "%d ms", value);
    lv_obj_t * label = lv_obj_get_child(gauge, NULL);
    lv_label_set_text(label, buf);
    lv_obj_align(label, gauge, LV_ALIGN_CENTER, 0, 70);
}

static void makeKeyboard(){
  kb = lv_keyboard_create(lv_scr_act(), NULL);
  lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);
  lv_keyboard_set_cursor_manage(kb, true);
  
  lv_keyboard_set_textarea(kb, ta_password);
  lv_obj_set_event_cb(kb, keyboard_event_cb);
  lv_obj_move_background(kb);
}

static void keyboard_event_cb(lv_obj_t * kb, lv_event_t event){
  lv_keyboard_def_event_cb(kb, event);

  if(event == LV_EVENT_APPLY){
    lv_obj_move_background(kb);
    
  }else if(event == LV_EVENT_CANCEL){
    lv_obj_move_background(kb);
  }
}

static void makePWMsgBox(){
  mbox_connect = lv_msgbox_create(lv_scr_act(), NULL);
  static const char * btns[] ={"Connect", "Cancel", ""};
  
  ta_password = lv_textarea_create(mbox_connect, NULL);
  lv_obj_set_size(ta_password, 200, 40);
  lv_textarea_set_text(ta_password, "");


  lv_msgbox_add_btns(mbox_connect, btns);
  lv_obj_set_width(mbox_connect, 200);
  lv_obj_set_event_cb(mbox_connect, mbox_event_handler);
  lv_obj_align(mbox_connect, NULL, LV_ALIGN_CENTER, 0, -90);
  lv_obj_move_background(mbox_connect);
}

static void mbox_event_handler(lv_obj_t * obj, lv_event_t event){
    if(event == LV_EVENT_VALUE_CHANGED) {
      lv_obj_move_background(kb);
      lv_obj_move_background(mbox_connect);
      
          if(strcmp(lv_msgbox_get_active_btn_text(obj), "Connect")==0){
            password = lv_textarea_get_text(ta_password);
            password.trim();
            connectWIFI();
          }
    
    }
}

static void popupPWMsgBox(){
  if(ssidName == NULL || ssidName.length() == 0){
    return;
  }

    lv_textarea_set_text(ta_password, ""); 
    lv_msgbox_set_text(mbox_connect, ssidName.c_str());
    lv_obj_move_foreground(mbox_connect);
    
    lv_obj_move_foreground(kb);
    lv_keyboard_set_textarea(kb, ta_password);
}

/*
 * NETWORK TASKS
 */

void scanWIFITask(void *pvParameters) {  

  vTaskDelay(1000); 
  while (1) {
    updateBottomStatus(LV_COLOR_ORANGE, "::: Searching Available WIFI :::");        
    int n = WiFi.scanNetworks();
    if (n <= 0) {
      updateBottomStatus(LV_COLOR_RED, "Sorry no networks found!");        
    }else{
      lv_dropdown_clear_options(ddlist);  
      vTaskDelay(10);
      for (int i = 0; i < n; ++i) {
                           
        String item = WiFi.SSID(i) + " (" + WiFi.RSSI(i) +") " + ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
        lv_dropdown_add_option(ddlist,item.c_str(),LV_DROPDOWN_POS_LAST);
        vTaskDelay(10);
      }

     updateBottomStatus(LV_COLOR_GREEN, String(n) + " networks found!");                     
    }

    vTaskDelay(30000); 
   } 
}


void connectWIFI(){
  if(ssidName == NULL || ssidName.length() <1 || password == NULL || password.length() <1){
    return;
  }
  
  vTaskDelete(ntScanTaskHandler);
  vTaskDelay(500);
  xTaskCreate(beginWIFITask,
                "BeginWIFITask",
                2048,
                NULL,
                0,
                &ntConnectTaskHandler);                
}


void beginWIFITask(void *pvParameters) {

  updateBottomStatus(LV_COLOR_TEAL,"Connecting WIFI: " + ssidName);

  unsigned long startingTime = millis();

  WiFi.begin(ssidName.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < timeout)
  {
    vTaskDelay(250);
  }

   if(WiFi.status() != WL_CONNECTED) {
    updateBottomStatus(LV_COLOR_RED, "Please check your wifi password and try again.");
    vTaskDelay(2500);
    networkScanner();
    vTaskDelete(NULL);
  }
  
  updateBottomStatus(LV_COLOR_GREEN, "WIFI is Connected! Local IP: " +  WiFi.localIP().toString());
  
  if(Ping.ping(remote_host)) {
 
      updateBottomStatus(LV_COLOR_GRAY, ">>> Check the network latency. Ping to " + String(remote_host));
      
      float prev = 0;
      float totalPing = 0;
      
      for(int i=0; i<pingCount; i++){
        Ping.ping(remote_host, 1);  
        float avgTime = Ping.averageTime();
      
        if(avgTime <=0){
          i--;
          continue;
        }
        
        totalPing += avgTime;
        setGaugeValue(prev, avgTime);
        prev = avgTime;
        vTaskDelay(1000);
      }
      
      float totalAvg = totalPing / pingCount; 
      setGaugeValue(prev, totalAvg);

      updateBottomStatus(LV_COLOR_BLACK, "The average delay time is " + String(totalAvg) + " ms.");
      vTaskDelay(4000);

      setGaugeValue(totalAvg, 0);
    
    }else{
      updateBottomStatus(LV_COLOR_RED, "This WIFI you choose is not connected. Please select one of them again.");
    }

    networkScanner();
    vTaskDelete(NULL);
}
