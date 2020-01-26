#include "SPIFFS.h"
#include <lvgl.h>
#include <Ticker.h>
#include <TFT_eSPI.h>

#define LVGL_TICK_PERIOD 60

Ticker tick; /* timer for interrupt handler */
TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

int screenWidth = 480;
int screenHeight = 320;
String currentFileName;
  
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
  
      /*Save the state and save the pressed coordinate*/
      //if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&last_x, &last_y);
     
      /*Set the coordinates (if released use the last pressed coordinates)*/
      data->point.x = touchX;
      data->point.y = touchY;
  
      Serial.print("Data x");
      Serial.println(touchX);
      
      Serial.print("Data y");
      Serial.println(touchY);

    }

    return false; /*Return `false` because we are not buffering and no more data to read*/
}

/* Interrupt driven periodic handler */
static void lv_tick_handler(void)
{

  lv_tick_inc(LVGL_TICK_PERIOD);
}


void setup() {

  Serial.begin(115200); /* prepare for possible serial debug */

  lv_init();

  tft.begin(); /* TFT init */
  tft.setRotation(3);

  //uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
  uint16_t calData[5] = {299, 3588, 348, 3474, 1};
  tft.setTouch(calData);

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
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


  /*Initialize the graphics library's tick*/
  tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);

  if(initSPIFFS()){
    lv_main();
  } else {
    lv_error_page();
  }
}

void loop() {

  lv_task_handler(); /* let the GUI do its work */
  delay(5);
}

bool initSPIFFS(){
  if(!SPIFFS.begin(true)){
    return false;
  }
  return true;
}

String openFile(String filename){
  File file = SPIFFS.open(filename)  ;
  if(!file){
    file.close();
    return "";
  }
  String textData;
  while(file.available()){
    textData += char(file.read());
  }
  Serial.println(textData);
  file.close();
  return textData;
}

bool writeFile(String filename, String contents){
  File file = SPIFFS.open(filename, FILE_WRITE);
  
  if(!file){
    file.close();
    return false;
  }
  
  if(file.print(contents)){
    file.close();
    return true;
  }

  file.close();
  return false;
}

void deleteFile(String filename){
  SPIFFS.remove(filename);
}


static lv_obj_t *kb, *ta, *mbox, *mbox_filename, *ta_filename;
static const char * modal_popup_btns[] = {"Save", "Save as", "Delete", "Cancel", ""};
static const char * filename_popup_btns[]  ={"Apply", "Close", ""};

static void lv_main(){
  lv_theme_t * th = lv_theme_night_init(210, NULL);
  lv_theme_set_current(th);
  
  lv_obj_t * scr = lv_cont_create(NULL, NULL);
  lv_disp_load_scr(scr);

  lv_obj_t * tv = lv_tabview_create(scr, NULL);
  lv_obj_set_size(tv, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
  lv_obj_t * tab1 = lv_tabview_add_tab(tv, "LIST TAB");
  lv_obj_t * tab2 = lv_tabview_add_tab(tv, "WRITE TAB");

  create_tab1(tab1);
  create_tab2(tab2);
}

static void lv_error_page(){
  lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, "Please check your SPIFFS");
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0,0);
}

static void create_tab1(lv_obj_t * parent){
  //List Tab
  lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY);

  lv_obj_t * list = lv_list_create(parent, NULL);
  lv_obj_set_size(list, lv_obj_get_width(parent)-20, lv_obj_get_height(parent)-20);
  lv_obj_align(list, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);

  lv_obj_t * btn = lv_list_add_btn(list, LV_SYMBOL_EDIT, "New");
  lv_obj_set_event_cb(btn, file_list_event);

  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file){
    lv_obj_t * btn = lv_list_add_btn(list, LV_SYMBOL_FILE, file.name());
    lv_obj_set_event_cb(btn, file_list_event);
    file = root.openNextFile();
  }
}

static void create_tab2(lv_obj_t * parent){
  //Write Tab
  ta = lv_ta_create(parent, NULL);
  lv_obj_set_size(ta, lv_obj_get_width(parent) - 10, lv_obj_get_height(parent) / 2);
  lv_ta_set_cursor_type(ta, LV_CURSOR_BLOCK);
  lv_ta_set_text(ta, "");

 /*Create a keyboard and apply the styles*/
  kb = lv_kb_create(parent, NULL);
  lv_obj_set_size(kb, lv_obj_get_width(parent) -10, lv_obj_get_height(parent) / 2 - 20);
  lv_obj_set_event_cb(kb, keyboard_event_cb);
  /*Assign the text area to the keyboard*/
  lv_kb_set_ta(kb, ta);
  
}

static void keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event){
  lv_kb_def_event_cb(kb, event);
  if(event == LV_EVENT_APPLY){
    printf("LV_EVENT_APPLY\n");
    mbox_menu_modal();
  }else if(event == LV_EVENT_CANCEL){
    printf("LV_EVENT_CANCEL\n");
  }
}

static void mbox_menu_modal(){
  static lv_style_t modal_style;
  /* Create a full-screen background */
  lv_style_copy(&modal_style, &lv_style_plain_color);

  /* Set the background's style */
  modal_style.body.main_color = modal_style.body.grad_color = LV_COLOR_BLACK;
  modal_style.body.opa = LV_OPA_50;

  /* Create a base object for the modal background */
  lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
  lv_obj_set_style(obj, &modal_style);
  lv_obj_set_pos(obj, 0, 0);
  lv_obj_set_size(obj, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_opa_scale_enable(obj, true); /* Enable opacity scaling for the animation */

  /* Create the message box as a child of the modal background */
  mbox = lv_mbox_create(obj, NULL);
  lv_mbox_add_btns(mbox, modal_popup_btns);
  lv_mbox_set_text(mbox, "Menu options!");
  lv_obj_set_size(mbox, LV_HOR_RES - 100, LV_VER_RES - 50);
  lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_event_cb(mbox, mbox_event_cb);

  /* Fade the message box in with an animation */
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_time(&a, 500, 0);
  lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
  lv_anim_set_exec_cb(&a, obj, (lv_anim_exec_xcb_t)lv_obj_set_opa_scale);
  lv_anim_create(&a);

}

static void mbox_event_cb(lv_obj_t *obj, lv_event_t evt)
{
  if(evt == LV_EVENT_DELETE && obj == mbox) {
    /* Delete the parent modal background */
    lv_obj_del_async(lv_obj_get_parent(mbox));
    mbox = NULL; /* happens before object is actually deleted! */
  } else if(evt == LV_EVENT_VALUE_CHANGED) {
    /* A button was clicked */
    lv_mbox_start_auto_close(mbox, 0);
    String contents = lv_ta_get_text(ta);
    
    if(strcmp(lv_mbox_get_active_btn_text(obj), "Save")==0){
      if(contents.length() > 0){
        if(strcmp(currentFileName.c_str(),"New") == 0){
          Serial.println("New Save");
          open_mbox_filename();
        }else{
          if(writeFile(currentFileName, contents)){
            Serial.println("Write Success");
          }else{
            //Error
          }
        }
      }else{
        //Nothing to write
      }
      
    }else if(strcmp(lv_mbox_get_active_btn_text(obj), "Save As")==0){
      if(contents.length() > 0){
        open_mbox_filename();
      }
      
    }else if(strcmp(lv_mbox_get_active_btn_text(obj), "Delete")==0){
      if(strcmp(currentFileName.c_str(), "New")==0){
        //Just go back to home screen
      }else{
        deleteFile(currentFileName);
      }
    } 
  }
}

static void open_mbox_filename(){
    mbox_filename = lv_mbox_create(lv_scr_act(), NULL);
    lv_mbox_set_text(mbox_filename, "File Name");
    
    ta_filename = lv_ta_create(mbox_filename, NULL);
    lv_obj_set_size(ta_filename, 200, 40);
    lv_ta_set_cursor_type(ta_filename, LV_CURSOR_LINE);
    lv_ta_set_text(ta_filename, "");
    
    lv_mbox_add_btns(mbox_filename, filename_popup_btns);
    lv_obj_set_width(mbox_filename, 200);
    lv_obj_set_event_cb(mbox_filename, filename_event_handler);
    lv_obj_align(mbox_filename, NULL, LV_ALIGN_CENTER, 0, 0); /*Align to the corner*/
    lv_kb_set_ta(kb, ta_filename);
}

static void filename_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        lv_mbox_start_auto_close(mbox_filename, 0);
        printf("Button: %s\n", lv_mbox_get_active_btn_text(obj));

        if(strcmp(lv_mbox_get_active_btn_text(obj), "Apply")==0){
          String contents = lv_ta_get_text(ta);
          String fileName = lv_ta_get_text(ta_filename);
          if(fileName.length() > 0){
            if(writeFile("/"+fileName, contents)){
              //Write Success
            }else{
              //Cannot write to file
            }
          }else{
            //Check the file name
          }
        }
    }
}


static void file_list_event(lv_obj_t * btn, lv_event_t e){
  if(e == LV_EVENT_CLICKED){
    currentFileName = lv_list_get_btn_text(btn);
    Serial.println(currentFileName);
    if(strcmp(currentFileName.c_str(),"New") == 0){
      lv_ta_set_text(ta, "");
    }else{
      lv_ta_set_text(ta, openFile(currentFileName).c_str());
    }
  }
}
