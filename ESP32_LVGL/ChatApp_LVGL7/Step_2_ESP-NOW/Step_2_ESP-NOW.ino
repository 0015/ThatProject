#include "WiFi.h"
#include <esp_now.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();

int screenWidth = 320;
int screenHeight = 480;
static lv_color_t buf[LV_HOR_RES_MAX * LV_VER_RES_MAX / 10];
static lv_disp_buf_t disp_buf;

/* LVGL objects */
static lv_obj_t * bg_top;
static lv_obj_t * bg_middle;
static lv_obj_t * bg_bottom;
static lv_obj_t * bg_keyboard;
static lv_obj_t * kb;
static lv_obj_t * kb_ta;
static lv_obj_t * label_status;
static lv_obj_t * fake_ta;
static lv_obj_t * page;
static lv_obj_t * table;

int itemIdx;

#define CHANNEL 1
enum Role { Primary, Secondary };
Role role = Primary; // Select the role of this device
esp_now_peer_info_t peer;

unsigned long peer_ping_interval = 1 * 1000; // 1sec
unsigned long peer_ping_lasttime = 0; 
unsigned long peer_check_interval = 3 * 1000; // 3sec
unsigned long peer_last_packet_time = 0;

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

  vTaskDelay(500);
  xTaskCreate(espnowTask,
                "espnowTask",
                4096,
                NULL,
                1,
                NULL);
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
    lv_obj_t * scr = lv_obj_create(NULL, NULL);
    lv_scr_load(scr);

    bg_top = lv_obj_create(scr, NULL);
    lv_obj_clean_style_list(bg_top, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_COLOR_YELLOW);
    lv_obj_set_size(bg_top, LV_HOR_RES, 30);

    label_status = lv_label_create(bg_top, NULL);
    lv_label_set_long_mode(label_status, LV_LABEL_LONG_SROLL_CIRC);
    lv_obj_set_width(label_status, LV_HOR_RES - 20);
    lv_label_set_text(label_status, "TOP STATUS");
    lv_obj_align(label_status, NULL, LV_ALIGN_CENTER, 0, 0);
    
    bg_middle = lv_obj_create(scr, NULL);
    lv_obj_clean_style_list(bg_middle, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_middle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_middle, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_COLOR_WHITE);
    lv_obj_set_pos(bg_middle, 0, 30);
    lv_obj_set_size(bg_middle, LV_HOR_RES, 400);

    bg_bottom = lv_obj_create(scr, NULL);
    lv_obj_clean_style_list(bg_bottom, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_bottom, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_bottom, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_COLOR_ORANGE);
    lv_obj_set_pos(bg_bottom, 0, 430);
    lv_obj_set_size(bg_bottom, LV_HOR_RES, 50);

    fake_ta = lv_textarea_create(bg_bottom, NULL);
    lv_textarea_set_text(fake_ta, "Enter a message");
    lv_obj_set_pos(fake_ta, 0, 0);
    lv_obj_set_size(fake_ta, LV_HOR_RES, 50);
    lv_obj_set_event_cb(fake_ta, fake_ta_event_handler);

    bg_keyboard = lv_obj_create(scr, NULL);
    lv_obj_clean_style_list(bg_bottom, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(bg_keyboard, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(bg_keyboard, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_COLOR_WHITE);
    lv_obj_set_pos(bg_keyboard, 0, LV_VER_RES - (LV_VER_RES / 2  + 40) );
    lv_obj_set_size(bg_keyboard, LV_HOR_RES, LV_VER_RES / 2  + 40);
    
    lv_page();
    makeKeyboard();
}

static void lv_page(void)
{
    page = lv_page_create(bg_middle, NULL);
    lv_obj_clean_style_list(page, LV_OBJ_PART_MAIN);
    
    lv_obj_set_size(page, LV_HOR_RES, 400);
    lv_obj_set_pos(page, 0, 0);
    //lv_page_set_edge_flash(page, true);
    
    itemIdx = 0;
    
    table = lv_table_create(page, NULL);
    lv_obj_clean_style_list(table, LV_OBJ_PART_MAIN);
    lv_table_set_col_cnt(table, 2);
    lv_obj_set_pos(table, 0, 0);
    lv_obj_set_click(table, false);
}

static void fake_ta_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
       lv_obj_move_foreground(bg_keyboard);
       lv_obj_set_height(page, 180);
       scrollToEndOfTable();
    }
}

static void scrollToEndOfTable(){
   float tableHeight = lv_obj_get_height(table);
   printf("tableHeight: %f\n", tableHeight);
   lv_page_scroll_ver(page, -tableHeight);
}

static void updateTopStatus(lv_color_t color, String text){
  lv_obj_set_style_local_bg_color(bg_top, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,color);
  lv_label_set_text(label_status, text.c_str());
}

static void makeKeyboard(){
    kb_ta = lv_textarea_create(bg_keyboard, NULL);
    lv_obj_set_size(kb_ta, LV_HOR_RES, 40);
    lv_obj_set_pos(kb_ta, 0, 0);
    lv_textarea_set_text(kb_ta, "");

    kb = lv_keyboard_create(bg_keyboard, NULL);
    lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);
    lv_obj_set_pos(kb, 0, 40);
    lv_keyboard_set_cursor_manage(kb, true);
  
    lv_keyboard_set_textarea(kb, kb_ta);
    lv_obj_set_event_cb(kb, keyboard_event_cb);
    lv_obj_move_background(bg_keyboard);
}

static void keyboard_event_cb(lv_obj_t * kb, lv_event_t event){
  lv_keyboard_def_event_cb(kb, event);

  if(event == LV_EVENT_APPLY){
    lv_obj_move_background(bg_keyboard);
    lv_obj_set_height(page, 400);

    //updateTable(true, lv_textarea_get_text(kb_ta));
    checkPeerToSendMsg(lv_textarea_get_text(kb_ta));
    
    scrollToEndOfTable();
    lv_textarea_set_text(kb_ta, "");
    
  }else if(event == LV_EVENT_CANCEL){
    lv_obj_move_background(bg_keyboard);
    lv_obj_set_height(page, 400);
  }
}

static void updateTable(bool isMine, String msg){
  msg.trim();
  if(msg.length() > 0){
    lv_table_set_row_cnt(table, (itemIdx+1));
    lv_table_set_col_width(table, itemIdx, LV_HOR_RES/2 - 10);
    lv_table_set_cell_align(table, itemIdx, isMine? 1 : 0, isMine? LV_LABEL_ALIGN_RIGHT : LV_LABEL_ALIGN_LEFT); 
    lv_table_set_cell_value(table, itemIdx, isMine? 1 : 0, msg.c_str()); 
    itemIdx++;
    scrollToEndOfTable();
  }
}

/*
 * ESP-NOW TASK 
 */
void espnowTask(void *pvParameters) {  

  vTaskDelay(1000); 
  
  if(role == Primary){
    Serial.println("[Primary Mode]");
    WiFi.mode(WIFI_STA);
  }else{
    Serial.println("[Secondary Mode]");
    WiFi.mode(WIFI_AP_STA);
    configDeviceAP();
  }

  InitESPNow();
  esp_now_register_recv_cb(OnDataRecv);

  while (1) {
    if(role == Primary){
      ScanForPeer();
    }   
    sendPing();
    checkLastPacket();
    vTaskDelay(50);
  }
} 

void configDeviceAP() {
  String Prefix = "Peer:";
  String Mac = WiFi.macAddress();
  Serial.print("AP MAC: ");
  Serial.println(Mac);
  
  String SSID = Prefix + Mac;
  String Password = "123456789";
  bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
    ESP.restart();
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == 0) {
    Serial.println("ESPNow Init Success");
    updateTopStatus(LV_COLOR_CYAN, "- ESP-NOW READY -");
  }
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  if(role == Secondary && *peer.peer_addr == NULL){
    char macAddr[6];     
    sprintf(macAddr, "%02X:%02X:%02X:%02X:%02X:%02X",mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
    memcpy(&peer.peer_addr, macAddr, 6 );
    peer.channel = CHANNEL;
    peer.encrypt = 0;

    const esp_now_peer_info_t *peerNode = &peer;
    esp_err_t addStatus = esp_now_add_peer(peerNode);
    if (addStatus == 0) {
      updateTopStatus(LV_COLOR_SILVER, "- Connected -");
      checkPeerToSendMsg("Hello Primary Peer!");  
    }
  }
    
  String receivedString = (char*)data;
  receivedString.trim();
  if(receivedString.length() > 0){
    updateTable(false, receivedString);
  }

  peer_last_packet_time = millis();
}

void checkPeerToSendMsg(String msg){
  msg.trim();
  if(msg.length() == 0){
    return;
  }

  if(*peer.peer_addr == NULL){
    Serial.print("peer.peer_addr == NULL");
    return;
  }

  bool sentResult = sendMsgToPeer(peer.peer_addr, msg);

  if(sentResult){
   updateTable(true, msg);
  }
}

bool sendMsgToPeer(uint8_t *peer_addr, String msg){
  byte byteMsg[msg.length()+1];
  msg.getBytes(byteMsg, msg.length()+1);
  
  esp_err_t result = esp_now_send(peer_addr, (uint8_t *) byteMsg, sizeof(byteMsg));
  return !result;
}

void ScanForPeer() {
  
  if(*peer.peer_addr != NULL){
    return;
  }
  
  int8_t scanResults = WiFi.scanNetworks(); 
  if (scanResults > 0) {
    for (int i = 0; i < scanResults; ++i) {
      String SSID = WiFi.SSID(i);
      String BSSIDstr = WiFi.BSSIDstr(i);
      vTaskDelay(10);
      
      if (SSID.indexOf("Peer") == 0) {
        int mac[6];
        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          char macAddr[6];     
          sprintf(macAddr, "%02X:%02X:%02X:%02X:%02X:%02X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
          
          esp_now_peer_info_t temp;
          memcpy( &temp.peer_addr, macAddr, 6 );

          if(esp_now_is_peer_exist(temp.peer_addr)){
            Serial.println("Existed Peer!");
            continue;  
          }
      
          memcpy(&peer.peer_addr, macAddr, 6 );
          peer.channel = CHANNEL;
          peer.encrypt = 0;

          const esp_now_peer_info_t *peerNode = &peer;
          if(esp_now_add_peer(peerNode) != ESP_OK){
            Serial.println("Failed to add peer");
            continue;
          }

          updateTopStatus(LV_COLOR_SILVER, "- Connected -");
          checkPeerToSendMsg("Hello There~");
        }
      }
    }
  }

  WiFi.scanDelete();
  vTaskDelay(1000);
}

void sendPing(){
  if (millis() > peer_ping_lasttime + peer_ping_interval)  {

    if(*peer.peer_addr != NULL){
        byte pingByteMsg[1];
        String pingMsg = " ";
        pingMsg.getBytes(pingByteMsg, pingMsg.length());
        esp_now_send(peer.peer_addr, (uint8_t *) pingByteMsg, sizeof(pingByteMsg));
    }
    
    peer_ping_lasttime = millis();
  }
}

void checkLastPacket(){
  if(*peer.peer_addr != NULL){
   unsigned long sinceLastPacket = millis() - peer_last_packet_time;
   if(sinceLastPacket > peer_check_interval){
    delPeer();
    *peer.peer_addr = NULL;
    updateTopStatus(LV_COLOR_ORANGE, "- Peer Disconnected! -");    
   }
  }
}

void delPeer(){
  esp_now_del_peer(peer.peer_addr);
}
