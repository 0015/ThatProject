/////////////////////////////////////////////////////////////////
/*
  ESP32 | LVGL8 | Ep 3. Base Project for WiFi Settings
  Video Tutorial: https://youtu.be/r62vfOhWXeo
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

// This project was made with a resolution of 480x320.
// If your display's size is different from this,
// you'll need to adjust the size and position of each widget a bit to fit it.

// 0. Setup your display LovyanGFX or TFT_eSPI
// 1. Setup your Touch driver
// 1. Setup your LVGL Config(lv_conf.h)
// 2. Setup your Timezone

#include "WiFi.h"
#include "MyDisplaySetup.h"
#include <lvgl.h>
#include <vector>

#include "time.h"
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -8 * 60 * 60;  // Set your timezone here
const int daylightOffset_sec = 0;

#include <EEPROM.h>
#define EEPROM_SIZE 128
#define EEPROM_ADDR_WIFI_FLAG 0
#define EEPROM_ADDR_WIFI_CREDENTIAL 4

#include <FT6236.h>
#define SDA_FT6236 21
#define SCL_FT6236 22
FT6236 ts = FT6236();

typedef enum {
  NONE,
  NETWORK_SEARCHING,
  NETWORK_CONNECTED_POPUP,
  NETWORK_CONNECTED,
  NETWORK_CONNECT_FAILED
} Network_Status_t;
Network_Status_t networkStatus = NONE;

/*Change to your screen resolution*/
static const uint32_t screenWidth = 480;
static const uint32_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];
static lv_style_t label_style;
static lv_obj_t *headerLabel;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touch*/
void my_touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (ts.touched()) {
    data->state = LV_INDEV_STATE_PR;
    TS_Point p = ts.getPoint();
    data->point.x = p.y;
    data->point.y = tft.height() - p.x;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

static lv_style_t border_style;
static lv_style_t popupBox_style;
static lv_obj_t *timeLabel;
static lv_obj_t *settings;
static lv_obj_t *settingBtn;
static lv_obj_t *settingCloseBtn;
static lv_obj_t *settingWiFiSwitch;
static lv_obj_t *wfList;
static lv_obj_t *settinglabel;
static lv_obj_t *mboxConnect;
static lv_obj_t *mboxTitle;
static lv_obj_t *mboxPassword;
static lv_obj_t *mboxConnectBtn;
static lv_obj_t *mboxCloseBtn;
static lv_obj_t *keyboard;
static lv_obj_t *popupBox;
static lv_obj_t *popupBoxCloseBtn;
static lv_timer_t *timer;

static int foundNetworks = 0;
unsigned long networkTimeout = 10 * 1000;
String ssidName, ssidPW;

TaskHandle_t ntScanTaskHandler, ntConnectTaskHandler;
std::vector<String> foundWifiList;

void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(1);
  tft.setBrightness(255);

  /*Touch Screen Input*/
  if (!ts.begin(40, SDA_FT6236, SCL_FT6236)) {
    Serial.println("Unable to start the capacitive touch Screen.");
  }

  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touch_read;
  lv_indev_drv_register(&indev_drv);

  setStyle();
  makeKeyboard();
  buildStatusBar();
  buildPWMsgBox();
  buildBody();
  buildSettings();

  tryPreviousNetwork();
}

void loop() {
  lv_timer_handler();
  delay(5);
}

void tryPreviousNetwork() {

  if (!EEPROM.begin(EEPROM_SIZE)) {
    delay(1000);
    ESP.restart();
  }

  loadWIFICredentialEEPROM();
}

void saveWIFICredentialEEPROM(int flag, String ssidpw) {
  EEPROM.writeInt(EEPROM_ADDR_WIFI_FLAG, flag);
  EEPROM.writeString(EEPROM_ADDR_WIFI_CREDENTIAL, flag == 1 ? ssidpw : "");
  EEPROM.commit();
}

void loadWIFICredentialEEPROM() {
  int wifiFlag = EEPROM.readInt(EEPROM_ADDR_WIFI_FLAG);
  String wifiCredential = EEPROM.readString(EEPROM_ADDR_WIFI_CREDENTIAL);

  if (wifiFlag == 1 && wifiCredential.length() != 0 && wifiCredential.indexOf(" ") != -1) {
    char preSSIDName[30], preSSIDPw[30];
    if (sscanf(wifiCredential.c_str(), "%s %s", preSSIDName, preSSIDPw) == 2) {

      lv_obj_add_state(settingWiFiSwitch, LV_STATE_CHECKED);
      lv_event_send(settingWiFiSwitch, LV_EVENT_VALUE_CHANGED, NULL);

      popupMsgBox("Welcome Back!", "Attempts to reconnect to the previously connected network.");
      ssidName = String(preSSIDName);
      ssidPW = String(preSSIDPw);
      networkConnector();
    } else {
      saveWIFICredentialEEPROM(0, "");
    }
  }
}

static void setStyle() {
  lv_style_init(&border_style);
  lv_style_set_border_width(&border_style, 2);
  lv_style_set_border_color(&border_style, lv_color_black());

  lv_style_init(&popupBox_style);
  lv_style_set_radius(&popupBox_style, 10);
  lv_style_set_bg_opa(&popupBox_style, LV_OPA_COVER);
  lv_style_set_border_color(&popupBox_style, lv_palette_main(LV_PALETTE_BLUE));
  lv_style_set_border_width(&popupBox_style, 5);
}

static void buildStatusBar() {

  static lv_style_t style_btn;
  lv_style_init(&style_btn);
  lv_style_set_bg_color(&style_btn, lv_color_hex(0xC5C5C5));
  lv_style_set_bg_opa(&style_btn, LV_OPA_50);

  lv_obj_t *statusBar = lv_obj_create(lv_scr_act());
  lv_obj_set_size(statusBar, tft.width(), 30);
  lv_obj_align(statusBar, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_remove_style(statusBar, NULL, LV_PART_SCROLLBAR | LV_STATE_ANY);

  timeLabel = lv_label_create(statusBar);
  lv_obj_set_size(timeLabel, tft.width() - 50, 30);

  lv_label_set_text(timeLabel, "WiFi Not Connected!    " LV_SYMBOL_CLOSE);
  lv_obj_align(timeLabel, LV_ALIGN_LEFT_MID, 8, 4);

  settingBtn = lv_btn_create(statusBar);
  lv_obj_set_size(settingBtn, 30, 30);
  lv_obj_align(settingBtn, LV_ALIGN_RIGHT_MID, 0, 0);

  lv_obj_add_event_cb(settingBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_t *label = lv_label_create(settingBtn); /*Add a label to the button*/
  lv_label_set_text(label, LV_SYMBOL_SETTINGS);  /*Set the labels text*/
  lv_obj_center(label);
}

static void btn_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED) {
    if (btn == settingBtn) {
      lv_obj_clear_flag(settings, LV_OBJ_FLAG_HIDDEN);
    } else if (btn == settingCloseBtn) {
      lv_obj_add_flag(settings, LV_OBJ_FLAG_HIDDEN);
    } else if (btn == mboxConnectBtn) {
      ssidPW = String(lv_textarea_get_text(mboxPassword));

      networkConnector();
      lv_obj_move_background(mboxConnect);
      popupMsgBox("Connecting!", "Attempting to connect to the selected network.");
    } else if (btn == mboxCloseBtn) {
      lv_obj_move_background(mboxConnect);
    } else if (btn == popupBoxCloseBtn) {
      lv_obj_move_background(popupBox);
    }

  } else if (code == LV_EVENT_VALUE_CHANGED) {
    if (btn == settingWiFiSwitch) {

      if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {

        if (ntScanTaskHandler == NULL) {
          networkStatus = NETWORK_SEARCHING;
          networkScanner();
          timer = lv_timer_create(timerForNetwork, 1000, wfList);
          lv_list_add_text(wfList, "WiFi: Looking for Networks...");
        }

      } else {

        if (ntScanTaskHandler != NULL) {
          networkStatus = NONE;
          vTaskDelete(ntScanTaskHandler);
          ntScanTaskHandler = NULL;
          lv_timer_del(timer);
          lv_obj_clean(wfList);
        }

        if (WiFi.status() == WL_CONNECTED) {
          WiFi.disconnect(true);
          lv_label_set_text(timeLabel, "WiFi Not Connected!    " LV_SYMBOL_CLOSE);
        }
      }
    }
  }
}

static void timerForNetwork(lv_timer_t *timer) {
  LV_UNUSED(timer);

  switch (networkStatus) {

    case NETWORK_SEARCHING:
      showingFoundWiFiList();
      break;

    case NETWORK_CONNECTED_POPUP:
      popupMsgBox("WiFi Connected!", "Now you'll get the current time soon.");
      networkStatus = NETWORK_CONNECTED;
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      break;

    case NETWORK_CONNECTED:

      showingFoundWiFiList();
      updateLocalTime();
      break;

    case NETWORK_CONNECT_FAILED:
      networkStatus = NETWORK_SEARCHING;
      popupMsgBox("Oops!", "Please check your wifi password and try again.");
      break;

    default:
      break;
  }
}

static void showingFoundWiFiList() {
  if (foundWifiList.size() == 0 || foundNetworks == foundWifiList.size())
    return;

  lv_obj_clean(wfList);
  lv_list_add_text(wfList, foundWifiList.size() > 1 ? "WiFi: Found Networks" : "WiFi: Not Found!");

  for (std::vector<String>::iterator item = foundWifiList.begin(); item != foundWifiList.end(); ++item) {
    lv_obj_t *btn = lv_list_add_btn(wfList, LV_SYMBOL_WIFI, (*item).c_str());
    lv_obj_add_event_cb(btn, list_event_handler, LV_EVENT_CLICKED, NULL);
    delay(1);
  }

  foundNetworks = foundWifiList.size();
}


static void buildBody() {
  lv_obj_t *bodyScreen = lv_obj_create(lv_scr_act());
  lv_obj_add_style(bodyScreen, &border_style, 0);
  lv_obj_set_size(bodyScreen, tft.width(), tft.height() - 34);
  lv_obj_align(bodyScreen, LV_ALIGN_BOTTOM_MID, 0, 0);

  lv_obj_t *label = lv_label_create(bodyScreen);
  lv_label_set_text(label, "Your Contents");
  lv_obj_center(label);
}

static void buildSettings() {
  settings = lv_obj_create(lv_scr_act());
  lv_obj_add_style(settings, &border_style, 0);
  lv_obj_set_size(settings, tft.width() - 100, tft.height() - 40);
  lv_obj_align(settings, LV_ALIGN_TOP_RIGHT, -20, 20);

  settinglabel = lv_label_create(settings);
  lv_label_set_text(settinglabel, "Settings " LV_SYMBOL_SETTINGS);
  lv_obj_align(settinglabel, LV_ALIGN_TOP_LEFT, 0, 0);

  settingCloseBtn = lv_btn_create(settings);
  lv_obj_set_size(settingCloseBtn, 30, 30);
  lv_obj_align(settingCloseBtn, LV_ALIGN_TOP_RIGHT, 0, -10);
  lv_obj_add_event_cb(settingCloseBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_t *btnSymbol = lv_label_create(settingCloseBtn);
  lv_label_set_text(btnSymbol, LV_SYMBOL_CLOSE);
  lv_obj_center(btnSymbol);

  settingWiFiSwitch = lv_switch_create(settings);
  lv_obj_add_event_cb(settingWiFiSwitch, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_align_to(settingWiFiSwitch, settinglabel, LV_ALIGN_TOP_RIGHT, 60, -10);
  lv_obj_add_flag(settings, LV_OBJ_FLAG_HIDDEN);

  wfList = lv_list_create(settings);
  lv_obj_set_size(wfList, tft.width() - 140, 210);
  lv_obj_align_to(wfList, settinglabel, LV_ALIGN_TOP_LEFT, 0, 30);
}

static void list_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);


  if (code == LV_EVENT_CLICKED) {

    String selectedItem = String(lv_list_get_btn_text(wfList, obj));
    for (int i = 0; i < selectedItem.length() - 1; i++) {
      if (selectedItem.substring(i, i + 2) == " (") {
        ssidName = selectedItem.substring(0, i);
        lv_label_set_text_fmt(mboxTitle, "Selected WiFi SSID: %s", ssidName);
        lv_obj_move_foreground(mboxConnect);
        break;
      }
    }
  }
}

/*
 * NETWORK TASKS
 */

static void networkScanner() {
  xTaskCreate(scanWIFITask,
              "ScanWIFITask",
              4096,
              NULL,
              1,
              &ntScanTaskHandler);
}

static void networkConnector() {
  xTaskCreate(beginWIFITask,
              "beginWIFITask",
              2048,
              NULL,
              1,
              &ntConnectTaskHandler);
}

static void scanWIFITask(void *pvParameters) {
  while (1) {
    foundWifiList.clear();
    int n = WiFi.scanNetworks();
    vTaskDelay(10);
    for (int i = 0; i < n; ++i) {
      String item = WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") " + ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      foundWifiList.push_back(item);
      vTaskDelay(10);
    }
    vTaskDelay(5000);
  }
}


void beginWIFITask(void *pvParameters) {

  unsigned long startingTime = millis();
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  vTaskDelay(100);

  WiFi.begin(ssidName.c_str(), ssidPW.c_str());
  while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < networkTimeout) {
    vTaskDelay(250);
  }

  if (WiFi.status() == WL_CONNECTED) {
    networkStatus = NETWORK_CONNECTED_POPUP;
    saveWIFICredentialEEPROM(1, ssidName + " " + ssidPW);
  } else {
    networkStatus = NETWORK_CONNECT_FAILED;
    saveWIFICredentialEEPROM(0, "");
  }

  vTaskDelete(NULL);
}

static void buildPWMsgBox() {

  mboxConnect = lv_obj_create(lv_scr_act());
  lv_obj_add_style(mboxConnect, &border_style, 0);
  lv_obj_set_size(mboxConnect, tft.width() * 2 / 3, tft.height() / 2);
  lv_obj_center(mboxConnect);

  mboxTitle = lv_label_create(mboxConnect);
  lv_label_set_text(mboxTitle, "Selected WiFi SSID: ThatProject");
  lv_obj_align(mboxTitle, LV_ALIGN_TOP_LEFT, 0, 0);

  mboxPassword = lv_textarea_create(mboxConnect);
  lv_obj_set_size(mboxPassword, tft.width() / 2, 40);
  lv_obj_align_to(mboxPassword, mboxTitle, LV_ALIGN_TOP_LEFT, 0, 30);
  lv_textarea_set_placeholder_text(mboxPassword, "Password?");
  lv_obj_add_event_cb(mboxPassword, text_input_event_cb, LV_EVENT_ALL, keyboard);

  mboxConnectBtn = lv_btn_create(mboxConnect);
  lv_obj_add_event_cb(mboxConnectBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_align(mboxConnectBtn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  lv_obj_t *btnLabel = lv_label_create(mboxConnectBtn);
  lv_label_set_text(btnLabel, "Connect");
  lv_obj_center(btnLabel);

  mboxCloseBtn = lv_btn_create(mboxConnect);
  lv_obj_add_event_cb(mboxCloseBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_align(mboxCloseBtn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  lv_obj_t *btnLabel2 = lv_label_create(mboxCloseBtn);
  lv_label_set_text(btnLabel2, "Cancel");
  lv_obj_center(btnLabel2);
}

static void text_input_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *ta = lv_event_get_target(e);

  if (code == LV_EVENT_FOCUSED) {
    lv_obj_move_foreground(keyboard);
    lv_keyboard_set_textarea(keyboard, ta);
    lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
  }

  if (code == LV_EVENT_DEFOCUSED) {
    lv_keyboard_set_textarea(keyboard, NULL);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
  }
}

static void makeKeyboard() {
  keyboard = lv_keyboard_create(lv_scr_act());
  lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
}

static void popupMsgBox(String title, String msg) {

  if (popupBox != NULL) {
    lv_obj_del(popupBox);
  }

  popupBox = lv_obj_create(lv_scr_act());
  lv_obj_add_style(popupBox, &popupBox_style, 0);
  lv_obj_set_size(popupBox, tft.width() * 2 / 3, tft.height() / 2);
  lv_obj_center(popupBox);

  lv_obj_t *popupTitle = lv_label_create(popupBox);
  lv_label_set_text(popupTitle, title.c_str());
  lv_obj_set_width(popupTitle, tft.width() * 2 / 3 - 50);
  lv_obj_align(popupTitle, LV_ALIGN_TOP_LEFT, 0, 0);

  lv_obj_t *popupMSG = lv_label_create(popupBox);
  lv_obj_set_width(popupMSG, tft.width() * 2 / 3 - 50);
  lv_label_set_text(popupMSG, msg.c_str());
  lv_obj_align(popupMSG, LV_ALIGN_TOP_LEFT, 0, 40);

  popupBoxCloseBtn = lv_btn_create(popupBox);
  lv_obj_add_event_cb(popupBoxCloseBtn, btn_event_cb, LV_EVENT_ALL, NULL);
  lv_obj_align(popupBoxCloseBtn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  lv_obj_t *btnLabel = lv_label_create(popupBoxCloseBtn);
  lv_label_set_text(btnLabel, "Okay");
  lv_obj_center(btnLabel);
}

void updateLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }

  char hourMin[6];
  strftime(hourMin, 6, "%H:%M", &timeinfo);
  String hourMinWithSymbol = String(hourMin);
  hourMinWithSymbol += "   ";
  hourMinWithSymbol += LV_SYMBOL_WIFI;
  lv_label_set_text(timeLabel, hourMinWithSymbol.c_str());
}