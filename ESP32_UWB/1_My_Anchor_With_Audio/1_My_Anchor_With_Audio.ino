/////////////////////////////////////////////////////////////////
/*
  ESP32 + UWB | Warning sounds depending on the distance
  For More Information: https://youtu.be/xrDvu8QT1Qs
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////
//ESP version 1.0.6
/////////////////////////////////////////////////////////////////

#include <SPI.h>
#include "DW1000Ranging.h"
#include "Audio.h"
#include "SPIFFS.h"
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

// connection pins
const uint8_t PIN_SCK = 18;
const uint8_t PIN_MOSI = 23;
const uint8_t PIN_MISO = 19;
const uint8_t PIN_SS = 15;
const uint8_t PIN_RST = 2;
const uint8_t PIN_IRQ = 22;

char *audioFiles[] = { "Connected.wav", "Disconnected.wav", "Over10m.wav", "Over20m.wav"};

Audio audio;

static const unsigned long REFRESH_INTERVAL = 10 * 1000; // 10sec
static unsigned long lastRefreshTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);

  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachBlinkDevice(newBlink);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);

  DW1000.enableDebounceClock();
  DW1000.enableLedBlinking();
  DW1000.setGPIOMode(MSGP3, LED_MODE);

  DW1000Ranging.startAsAnchor("82:17:5B:D5:A9:9A:E2:9C", DW1000.MODE_LONGDATA_RANGE_LOWPOWER);

  SPIFFS.begin();
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(21); // 0...21
}

void loop() {
  audio.loop();
  delay(2);
  if (!audio.isRunning()) {
    DW1000Ranging.loop();
  }
}

void newRange() {
  updateRange(DW1000Ranging.getDistantDevice()->getRange());
}

void newBlink(DW1000Device* device) {
  audio.connecttoFS(SPIFFS, audioFiles[0]);
}

void inactiveDevice(DW1000Device* device) {
  audio.connecttoFS(SPIFFS, audioFiles[1]);
}

void updateRange(float range) {
  float projectedRange = range * 2 / 5;
  selectAudioFile(projectedRange);
}

void selectAudioFile(float projectedRange) {

  if (millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    lastRefreshTime += REFRESH_INTERVAL;
    Serial.println(projectedRange);

    if (projectedRange > 20) {
      audio.connecttoFS(SPIFFS, audioFiles[3]);
    } else if (projectedRange > 10) {
      audio.connecttoFS(SPIFFS, audioFiles[2]);
    }
  }
}
