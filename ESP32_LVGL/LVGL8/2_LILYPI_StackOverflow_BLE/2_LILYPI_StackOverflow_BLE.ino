/////////////////////////////////////////////////////////////////
/*
  ESP32 | LVGL8 | Ep 2. Shortcuts for Pull Stack Developer
  Video Tutorial: https://youtu.be/i4qrWbHXH3U
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <lvgl.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <FT6236.h>
#define SDA_FT6236 21
#define SCL_FT6236 22
FT6236 ts = FT6236();

#include "BLECont.h"
BLECont *bleCont;
bool isBLEConnected;

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796 _panel_instance;
  lgfx::Bus_SPI _bus_instance;  // SPIバスのインスタンス
  lgfx::Light_PWM _light_instance;

public:
  LGFX(void) {
    {                                          // バス制御の設定を行います。
      auto cfg = _bus_instance.config();       // バス設定用の構造体を取得します。
      cfg.spi_host = VSPI_HOST;                // 使用するSPIを選択  (VSPI_HOST or HSPI_HOST)
      cfg.spi_mode = 0;                        // SPI通信モードを設定 (0 ~ 3)
      cfg.freq_write = 40000000;               // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
      cfg.freq_read = 16000000;                // 受信時のSPIクロック
      cfg.spi_3wire = false;                   // 受信をMOSIピンで行う場合はtrueを設定
      cfg.use_lock = true;                     // トランザクションロックを使用する場合はtrueを設定
      cfg.dma_channel = 1;                     // Set the DMA channel (1 or 2. 0=disable)   使用するDMAチャンネルを設定 (0=DMA不使用)
      cfg.pin_sclk = 18;                       // SPIのSCLKピン番号を設定
      cfg.pin_mosi = 19;                       // SPIのMOSIピン番号を設定
      cfg.pin_miso = 23;                       // SPIのMISOピン番号を設定 (-1 = disable)
      cfg.pin_dc = 27;                         // SPIのD/Cピン番号を設定  (-1 = disable)
      _bus_instance.config(cfg);               // 設定値をバスに反映します。
      _panel_instance.setBus(&_bus_instance);  // バスをパネルにセットします。
    }

    {                                       // 表示パネル制御の設定を行います。
      auto cfg = _panel_instance.config();  // 表示パネル設定用の構造体を取得します。
      cfg.pin_cs = 5;                       // CSが接続されているピン番号   (-1 = disable)
      cfg.pin_rst = -1;                     // RSTが接続されているピン番号  (-1 = disable)
      cfg.pin_busy = -1;                    // BUSYが接続されているピン番号 (-1 = disable)
      cfg.memory_width = 320;               // ドライバICがサポートしている最大の幅
      cfg.memory_height = 480;              // ドライバICがサポートしている最大の高さ
      cfg.panel_width = 320;                // 実際に表示可能な幅
      cfg.panel_height = 480;               // 実際に表示可能な高さ
      cfg.offset_x = 0;                     // パネルのX方向オフセット量
      cfg.offset_y = 0;                     // パネルのY方向オフセット量
      cfg.offset_rotation = 0;              // 回転方向の値のオフセット 0~7 (4~7は上下反転)
      cfg.dummy_read_pixel = 8;             // ピクセル読出し前のダミーリードのビット数
      cfg.dummy_read_bits = 1;              // ピクセル以外のデータ読出し前のダミーリードのビット数
      cfg.readable = true;                  // データ読出しが可能な場合 trueに設定
      cfg.invert = false;                   // パネルの明暗が反転してしまう場合 trueに設定
      cfg.rgb_order = false;                // パネルの赤と青が入れ替わってしまう場合 trueに設定
      cfg.dlen_16bit = false;               // データ長を16bit単位で送信するパネルの場合 trueに設定
      cfg.bus_shared = true;                // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

      _panel_instance.config(cfg);
    }

    {                                       // バックライト制御の設定を行います。（必要なければ削除）
      auto cfg = _light_instance.config();  // バックライト設定用の構造体を取得します。

      cfg.pin_bl = 12;      // バックライトが接続されているピン番号
      cfg.invert = false;   // バックライトの輝度を反転させる場合 true
      cfg.freq = 44100;     // バックライトのPWM周波数
      cfg.pwm_channel = 7;  // 使用するPWMのチャンネル番号

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);  // バックライトをパネルにセットします。
    }

    setPanel(&_panel_instance);  // 使用するパネルをセットします。
  }
};

LGFX tft;

/*Change to your screen resolution*/
static const uint32_t screenWidth = 480;
static const uint32_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];
static lv_style_t label_style;
static lv_obj_t *headerLabel;

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

/*Read the touch*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  if (ts.touched()) {
    data->state = LV_INDEV_STATE_PR;
    TS_Point p = ts.getPoint();
    data->point.x = p.y;
    data->point.y = tft.height() - p.x;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup() {
  Serial.begin(115200);
  
  tft.begin();
  tft.setRotation(1);
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

  makeHeader();
  makeButtonMatrix();
  bleInit();
}

void loop() {
  lv_timer_handler();
  bleCont->loop();
  delay(5);
}

void updateConnection(bool isOn) {
  Serial.printf("\n->UpdateConnection : %d", isOn);
  isBLEConnected = isOn;
  lv_style_set_text_color(&label_style, isOn ? lv_color_hex(0x0080FF) : lv_color_hex(0xFF0000));
  lv_obj_add_style(headerLabel, &label_style, 0);
}

void makeHeader() {
  //Header Image
  LV_IMG_DECLARE(header_img);
  lv_obj_t * connectionImg = lv_img_create(lv_scr_act());
  lv_img_set_src(connectionImg, &header_img);

  //Label Style
  lv_style_reset(&label_style);
  lv_style_init(&label_style);
  lv_style_set_text_color(&label_style, lv_color_hex(0xFF0000));
  lv_style_set_text_font(&label_style, &lv_font_montserrat_34);

  //Header Label
  headerLabel = lv_label_create(lv_scr_act());
  lv_obj_set_size(headerLabel, 70, 70);
  lv_obj_set_style_text_align(headerLabel, LV_TEXT_ALIGN_LEFT, 0);
  lv_label_set_text(headerLabel, LV_SYMBOL_BLUETOOTH);
  lv_obj_add_style(headerLabel, &label_style, 0);
  lv_obj_align(headerLabel, LV_ALIGN_TOP_RIGHT, 6, 24);
}

//Button Event Handler
static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *obj = lv_event_get_target(e);
  if (code == LV_EVENT_VALUE_CHANGED) {
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    const char *txt = lv_btnmatrix_get_btn_text(obj, id);

    Serial.printf("\n- %d, %s was pressed\n", id, txt);
    if (isBLEConnected)
      bleCont->triggerTask(id);
  }
}

//Make Button Matrix View
void makeButtonMatrix() {
  static lv_style_t button_matrix_style;
  lv_style_reset(&button_matrix_style);
  lv_style_init(&button_matrix_style);
  lv_style_set_border_width(&button_matrix_style, 0);
  lv_style_set_radius(&button_matrix_style, 5);
  lv_style_set_pad_all(&button_matrix_style, 4);
  lv_style_set_text_opa(&button_matrix_style, LV_OPA_COVER);
  lv_style_set_text_font(&button_matrix_style, &lv_font_montserrat_20);
  lv_style_set_bg_color(&button_matrix_style, lv_color_hex(0x000000));
  lv_style_set_bg_opa(&button_matrix_style, LV_OPA_TRANSP);

  static const char *btn_matrix_map[] = { 
    "Open Browser!", "stackoverflow.com","\n", 
    "Select All", "Copy", "Paste", "" };

  lv_obj_t *btn_matrix = lv_btnmatrix_create(lv_scr_act());
  lv_obj_set_size(btn_matrix, tft.width() - 60, tft.height() / 2);
  lv_obj_add_style(btn_matrix, &button_matrix_style, 0);
  lv_btnmatrix_set_map(btn_matrix, btn_matrix_map);
  lv_obj_align(btn_matrix, LV_ALIGN_CENTER, 0, 30);
  lv_obj_add_event_cb(btn_matrix, event_handler, LV_EVENT_ALL, NULL);
}

void bleInit(){
  isBLEConnected = false;
  bleCont = new BLECont();
  bleCont->bindCallback(updateConnection);
  bleCont->begin();
}
