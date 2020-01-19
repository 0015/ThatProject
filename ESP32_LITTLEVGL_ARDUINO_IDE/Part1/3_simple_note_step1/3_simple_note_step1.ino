#include <lvgl.h>
#include <Ticker.h>
#include <TFT_eSPI.h>

#define LVGL_TICK_PERIOD 60

Ticker tick; /* timer for interrupt handler */
TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

lv_obj_t * slider_label;
int screenWidth = 480;
int screenHeight = 320;
  
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

  lv_main();
}

void loop() {

  lv_task_handler(); /* let the GUI do its work */
  delay(5);
}

static lv_obj_t *kb;

static void lv_main(){
  lv_obj_t * scr = lv_cont_create(NULL, NULL);
  lv_disp_load_scr(scr);

  lv_obj_t * tv = lv_tabview_create(scr, NULL);
  lv_obj_set_size(tv, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
  lv_obj_t * tab1 = lv_tabview_add_tab(tv, "LIST TAB");
  lv_obj_t * tab2 = lv_tabview_add_tab(tv, "WRITE TAB");

  create_tab1(tab1);
  create_tab2(tab2);
}

static void create_tab1(lv_obj_t * parent){
  //List Tab
  lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY);
}

static void create_tab2(lv_obj_t * parent){
  //Write Tab
  lv_obj_t * ta = lv_ta_create(parent, NULL);
  lv_obj_set_size(ta, lv_obj_get_width(parent) - 10, lv_obj_get_height(parent) / 2);
  lv_ta_set_cursor_type(ta, LV_CURSOR_BLOCK);
  lv_ta_set_text(ta, "hello!");

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
  }else if(event == LV_EVENT_CANCEL){
    printf("LV_EVENT_CANCEL\n");
  }
}
