/////////////////////////////////////////////////////////////////
/*
  Let's build a GPS Speedometer with ESP32-S3 and BN-880Q(GPS, Compass) [1/2]
  For More Information: https://youtu.be/J0AYSS8xxOY
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <Wire.h>
// TinyGpsPlus
// https://github.com/mikalhart/TinyGPSPlus
#include <TinyGPSPlus.h>
// EspSoftwareSerial
// https://github.com/plerup/espsoftwareserial/
#include <SoftwareSerial.h>
// DFRobot_QMC5883
// https://github.com/DFRobot/DFRobot_QMC5883
#include <DFRobot_QMC5883.h>
// LovyanGFX
// https://github.com/lovyan03/LovyanGFX
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

static const int RXPin = 10, TXPin = 11, sclPin = 12, sdaPin = 13;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
DFRobot_QMC5883 compass(&Wire, QMC5883_ADDRESS);

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7796 _panel_instance;
  lgfx::Bus_Parallel8 _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.freq_write = 40000000;
      cfg.pin_wr = 47;
      cfg.pin_rd = -1;
      cfg.pin_rs = 0;
      cfg.pin_d0 = 9;
      cfg.pin_d1 = 46;
      cfg.pin_d2 = 3;
      cfg.pin_d3 = 8;
      cfg.pin_d4 = 18;
      cfg.pin_d5 = 17;
      cfg.pin_d6 = 16;
      cfg.pin_d7 = 15;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = -1;
      cfg.pin_rst = 4;
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
      cfg.invert = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = 45;
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    {
      auto cfg = _touch_instance.config();
      cfg.i2c_port = 1;
      cfg.i2c_addr = 0x38;
      cfg.pin_sda = 6;
      cfg.pin_scl = 5;
      cfg.freq = 400000;
      cfg.x_min = 0;
      cfg.x_max = 320;
      cfg.y_min = 0;
      cfg.y_max = 480;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};

static LGFX tft;

void setup() {
  Serial.begin(115200);
  initDisplay();
  initQMC5883();
  ss.begin(GPSBaud);
}

void loop() {
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      updateGPS();
    }
  }
  updateDegree();
  delay(10);
}

void initDisplay() {
  tft.begin();
  tft.setRotation(2);
  tft.setBrightness(255);
  tft.fillScreen(TFT_BLACK);
}

void initQMC5883() {
  Wire.begin(sdaPin, sclPin);
  while (!compass.begin()) {
    Serial.println("Could not find a valid 5883 sensor, check wiring!");
    delay(500);
  }

  if (compass.isQMC()) {
    Serial.println("Initialize QMC5883");
    compass.setRange(QMC5883_RANGE_2GA);
    Serial.print("compass range is:");
    Serial.println(compass.getRange());

    compass.setMeasurementMode(QMC5883_CONTINOUS);
    Serial.print("compass measurement mode is:");
    Serial.println(compass.getMeasurementMode());

    compass.setDataRate(QMC5883_DATARATE_200HZ);
    Serial.print("compass data rate is:");
    Serial.println(compass.getDataRate());

    compass.setSamples(QMC5883_SAMPLES_8);
    Serial.print("compass samples is:");
    Serial.println(compass.getSamples());
  }
}

void updateGPS() {

  int satellites = 0;
  double mph = 0;
  double lat = 0;
  double lng = 0;
  double altitude = 0;

  if (gps.satellites.isValid()) {
    satellites = gps.satellites.value();
  }

  if (gps.speed.isValid()) {
    mph = gps.speed.mph();
  }

  if (gps.location.isValid()) {
    lat = gps.location.lat();
    lng = gps.location.lng();
  }

  if (gps.altitude.isValid()) {
    altitude = gps.altitude.feet();
  }

  char data[150] = {};
  sprintf(data, "-Satellites: %d\n-Latitude: %.6lf\n-Longitude: %.6lf\n-Altitude: %.2lf ft\n-Speed: %.1lf mph", satellites, lat, lng, altitude, mph);

  tft.setCursor(0, 10);
  tft.setTextColor(TFT_GREEN);
  tft.setTextSize(4);
  tft.println("GPS Details");
  tft.setTextSize(2);
  tft.fillRect(0, 40, tft.width(), tft.height() / 2 - 40, TFT_BLACK);
  tft.println(data);
}

void updateDegree() {
  sVector_t mag = compass.readRaw();
  compass.getHeadingDegrees();
  float headingDegree = mag.HeadingDegress;
  char data[20] = {};
  sprintf(data, "-Degree: %.1f", headingDegree);

  tft.setCursor(0, tft.height() / 2);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(4);
  tft.println("Compass");
  tft.setTextSize(2);
  tft.fillRect(0, tft.height() / 2 + 30, tft.width(), tft.height(), TFT_BLACK);
  tft.println(data);
}