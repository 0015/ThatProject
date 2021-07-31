#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "FileIO.h"
#include "MyFirmware.h"

class Network {
private:
  HTTPClient http;
  StaticJsonDocument<300> doc;
  long localServerTime;
  typedef void (*FuncPtrInt)(int);


public:
  Network();
  void WiFiBegin();
  void fetchLocalServerTime();
  long getLocalServerTime();
  Firmware checkVersion();
  String fileDownload(FuncPtrInt callback, FileIO** fileIO, String target_path);
};
