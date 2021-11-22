/////////////////////////////////////////////////////////////////
/*
  ESP32CAM | Sending images via UDP?
  Video Tutorial: https://youtu.be/1kHxd5FOUEU
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "WiFi.h"
#include "AsyncUDP.h"
#include <vector>
#define CHUNK_LENGTH 1460

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
      cfg.freq_read = 20000000;
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
std::vector<uint8_t> byte_vector;

const char* ssid = "That-Project";
const char* password = "California";
const int udpPort = 8000;

AsyncUDP udp;

uint32_t fpsLastTime = 0;
int nbFrames = 0;

void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(1);
  tft.setBrightness(255);

  tft.fillScreen(TFT_BLACK);
  tft.setFont(&fonts::Orbitron_Light_24);
  tft.setTextColor(TFT_GREEN);

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  tft.drawString(myIP.toString(), tft.width() / 2, tft.height() / 2);

  if (udp.listen(udpPort)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());

    udp.onPacket([](AsyncUDPPacket packet) {
      Serial.printf("packet Length: %d\n", packet.length());

      std::vector<uint8_t> temp_vector(&packet.data()[0], &packet.data()[packet.length()]);

      if (packet.length() == CHUNK_LENGTH && 
      packet.data()[0] == 255 && 
      packet.data()[1] == 216 &&
      packet.data()[2] == 255) { // FF D8 FF
        byte_vector.clear();
      }

      byte_vector.insert(byte_vector.end(), temp_vector.begin(), temp_vector.end());

      if (packet.length() != CHUNK_LENGTH && 
      packet.data()[packet.length() - 2] == 255 && 
      packet.data()[packet.length() - 1] == 217) { // FF D9
        uint8_t* jpgData = byte_vector.data();
        tft.drawJpg(jpgData, byte_vector.size(), 40, 12);
      
        nbFrames++;
        if (millis() - fpsLastTime >= 1000) {
          drawingFPSText(nbFrames);
          nbFrames = 0;
          fpsLastTime += 1000;
        }
      }
    });
  }
}

void loop() {
}

void drawingFPSText(int fps) {
  tft.fillRect(0, 0, 40, 40, TFT_BLUE);
  tft.setCursor(8, 4);
  tft.printf("%d", fps);
}