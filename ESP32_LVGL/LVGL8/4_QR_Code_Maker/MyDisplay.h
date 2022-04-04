/////////////////////////////////////////////////////////////////
/*
  QR Code Maker (ESP32+LVGL8)
  For More Information: https://youtu.be/Ss3zBO-V9kI
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////
//WT32-SC01(ST7796 Setup)
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
//FT6236 Capacitive Touch Screen
#include <FT6236.h>
#define SDA_FT6236 18
#define SCL_FT6236 19

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7796     _panel_instance;
    lgfx::Bus_SPI       _bus_instance;
    lgfx::Light_PWM     _light_instance;

  public:
    LGFX(void)
    {
      {
        auto cfg = _bus_instance.config();
        cfg.spi_host = HSPI_HOST;
        cfg.spi_mode = 0;
        cfg.freq_write = 80000000;
        cfg.freq_read  = 16000000;
        cfg.spi_3wire  = false;
        cfg.use_lock   = true;
        cfg.dma_channel = 1;
        cfg.pin_sclk = 14;
        cfg.pin_mosi = 13;
        cfg.pin_miso = 12;
        cfg.pin_dc   = 21;
        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
      }

      {
        auto cfg = _panel_instance.config();
        cfg.pin_cs           =    15;
        cfg.pin_rst          =    22;
        cfg.pin_busy         =    -1;
        cfg.memory_width     =   320;
        cfg.memory_height    =   480;
        cfg.panel_width      =   320;
        cfg.panel_height     =   480;
        cfg.offset_x         =     0;
        cfg.offset_y         =     0;
        cfg.offset_rotation  =     0;
        cfg.dummy_read_pixel =     8;
        cfg.dummy_read_bits  =     1;
        cfg.readable         =  true;
        cfg.invert           = false;
        cfg.rgb_order        = false;
        cfg.dlen_16bit       = false;
        cfg.bus_shared       =  true;

        _panel_instance.config(cfg);
      }

      {
        auto cfg = _light_instance.config();

        cfg.pin_bl = 23;
        cfg.invert = false;
        cfg.freq   = 44100;
        cfg.pwm_channel = 7;

        _light_instance.config(cfg);
        _panel_instance.setLight(&_light_instance);
      }

      setPanel(&_panel_instance);
    }
};

static LGFX tft;
FT6236 ts = FT6236();
