#include "Display.h"
#include "VersionCont.h"
#include "Network.h"
#include "ButtonCont.h"
#include "Updater.h"
#include <ESP32Time.h>

typedef enum {
  NONE,
  NETWORK_BEGIN,
  NETWORK_CONNECTED,
  SERVER_FOUND,
  FIRMWARE_DOWNLOAD_START,
  FIRMWARE_UPDATED
} SimpleOTA_State_t;

class SimpleOTA {
private:
  Display *display;
  VersionCont *version;
  Network *network;
  FileIO *fileIO;
  ButtonCont *buttonCont;
  ESP32Time rtc;

  long t1, t2;

  void initDisplay();
  void initVersion();
  void initNetwork();
  void initFileIO();
  void initButton();
  void requestLocalServerTime();
  void updateTime();
  void serverFirmwareCheck();
  void startDownload();
  void updateFirmware();
  friend void buttonCallbackEvent();
  friend void networkDownloadEvent(int percent);

  SimpleOTA_State_t currentState;

public:
  SimpleOTA();
  ~SimpleOTA();
  void begin();
  void loop();
};