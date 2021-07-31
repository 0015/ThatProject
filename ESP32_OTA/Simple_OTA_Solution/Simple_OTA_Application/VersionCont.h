
#include "MyFirmware.h"
class VersionCont {
private:
  int firmwareVersion;
  void loadVersion();
  Firmware newFirmware;
  bool hasNewFirmware;

public:
  VersionCont();
  int getCurrentVersion();
  void saveVersion(int buildNum);
  void setNewFirmware(Firmware firmware);
  bool hasNewUpdate();
  bool md5CompareTo(String checksum);
  bool fileSizeCompareTo(int fileSize);
  int newFirmwareVersion();
  String getCPName();
  String getFirmwareServerPath();
};
