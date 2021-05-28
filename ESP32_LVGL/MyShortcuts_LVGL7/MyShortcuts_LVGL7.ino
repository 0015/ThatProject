/////////////////////////////////////////////////////////////////
/*
  ESP32 | LVGL7 = My Shortcuts for Windows 10 = (ft. BLE HID Keyboard)
  Video Tutorial: https://youtu.be/EWKAyQBdcYM
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <FT6236.h>
#include "BLECont.h"

FT6236 ts = FT6236();
TFT_eSPI tft = TFT_eSPI();

BLECont *bleCont;
QueueHandle_t msgQueue;
bool isConnected;

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
static lv_obj_t* headerBG;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

bool my_touchpad_read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data)
{
    if (ts.touched())
    {
        data->state = LV_INDEV_STATE_PR;
        TS_Point p = ts.getPoint();
        data->point.x = tft.width() - p.y;
        data->point.y = p.x;
    }else{
      data->state = LV_INDEV_STATE_REL;      
    }
    return false; 
}

void setup()
{
    Serial.begin(115200);

    lv_init();
    
    tft.begin(); 
    tft.setRotation(3);

    if (!ts.begin(40, 18, 5))
    {
        Serial.println("Unable to start the capacitive touchscreen.");
    }  

    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = tft.width();
    disp_drv.ver_res = tft.height();
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    static lv_style_t style_screen;
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&style_screen, LV_STATE_DEFAULT, &lv_font_montserrat_40); 
    lv_style_set_text_color(&style_screen, LV_STATE_DEFAULT, LV_COLOR_WHITE);   
    lv_obj_add_style(lv_scr_act(), LV_OBJ_PART_MAIN, &style_screen);  

    isConnected = false;
    makeHeader();
    makeGrid();

    beginMsgTask();
    beginBleController();
}

void loop(){
  lv_task_handler();
  bleCont->loop();
  delay(5);
}

void updateConnection(bool isOn){
  if(isConnected != isOn){
    isConnected = isOn;
    setConnectIcon(isOn);
  }
}

void beginBleController(){
  bleCont = new BLECont();
  bleCont->bindCallback(updateConnection);
  bleCont->begin();
}

void beginMsgTask(){
  msgQueue = xQueueCreate(10, sizeof(int)); 
  xTaskCreate(
              msgTask,          
              "msgTask",       
              1024,            
              NULL,            
              1,               
              NULL); 
}

void makeGrid(){
  LV_IMG_DECLARE(blank_icon);
  LV_IMG_DECLARE(task_icon);
  LV_IMG_DECLARE(close_icon);
  LV_IMG_DECLARE(min_icon);
  LV_IMG_DECLARE(max_icon);
  LV_IMG_DECLARE(taskswitch_icon);
  LV_IMG_DECLARE(desktop_icon);
  LV_IMG_DECLARE(file_icon);
  LV_IMG_DECLARE(lock_icon);

  lv_img_dsc_t * const my_icons[] = {
    (lv_img_dsc_t *)&task_icon,
    (lv_img_dsc_t *)&close_icon,
    (lv_img_dsc_t *)&min_icon,
    (lv_img_dsc_t *)&max_icon,
    (lv_img_dsc_t *)&taskswitch_icon,
    (lv_img_dsc_t *)&desktop_icon,
    (lv_img_dsc_t *)&file_icon,
    (lv_img_dsc_t *)&lock_icon
  };

  lv_obj_t * gridLayout =  lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_click(gridLayout, false);
  lv_cont_set_layout(gridLayout, LV_LAYOUT_GRID);
  lv_obj_set_y(gridLayout, 70);
  lv_obj_set_size(gridLayout, LV_HOR_RES, LV_VER_RES - 70);
  lv_obj_set_style_local_pad_all(gridLayout, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 20);
  lv_obj_set_style_local_pad_inner(gridLayout, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 20);

  lv_obj_t * rect;
  for (int i = 0; i < (sizeof(my_icons) / sizeof(my_icons[0])); i++) {
    rect = lv_obj_create(gridLayout, NULL);
    lv_obj_set_click(rect, false);
		lv_obj_set_size(rect, 90, 90);

    lv_obj_t * imgbtn1 = lv_imgbtn_create(rect, NULL);
    lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_PRESSED, &blank_icon);
    lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_RELEASED, my_icons[i]);
    
    lv_imgbtn_set_checkable(imgbtn1, true);
    lv_obj_align(imgbtn1, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_user_data(imgbtn1, (lv_obj_user_data_t) i);
    lv_obj_set_event_cb(imgbtn1, key_event_cb);
  }
}

void makeHeader(void){
  headerBG = lv_objmask_create(lv_scr_act(), NULL);
  lv_obj_set_size(headerBG, LV_HOR_RES, 70);

  lv_obj_t * headerLB = lv_label_create(headerBG, NULL);
  lv_label_set_long_mode(headerLB, LV_LABEL_LONG_BREAK); 
  lv_label_set_recolor(headerLB, true);                    
  lv_label_set_align(headerLB, LV_LABEL_ALIGN_LEFT);       
  lv_label_set_text(headerLB, LV_SYMBOL_BLUETOOTH " My Shortcuts ::");
  lv_obj_set_width(headerLB, LV_HOR_RES);
  lv_obj_align(headerLB, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 14);

  setConnectIcon(false);
}

void setConnectIcon(bool isOn){
  LV_IMG_DECLARE(unconnected_icon);
  LV_IMG_DECLARE(connected_icon);
  lv_obj_t * connectionImg = lv_img_create(headerBG, NULL);
  lv_img_set_src(connectionImg, isOn ? &connected_icon : &unconnected_icon);
  lv_obj_align(connectionImg, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
}

static void key_event_cb(lv_obj_t* obj, lv_event_t e){
    lv_obj_user_data_t itemData = lv_obj_get_user_data(obj);
    int itemNum = (int)itemData;
    if(e == LV_EVENT_CLICKED){
      xQueueSend(msgQueue, &itemNum, portMAX_DELAY);
    }
}

void msgTask(void* pvParam){
    int itemNum;
    while(1){
      if(xQueueReceive(msgQueue, &itemNum, portMAX_DELAY)== pdTRUE){
        bleCont->triggerTask(itemNum);
      }
      vTaskDelay(10);
    }
}
