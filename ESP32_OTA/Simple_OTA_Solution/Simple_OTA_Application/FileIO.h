#ifndef FileIO_H_
#define FileIO_H_

#include "SPIFFS.h"
#include "mbedtls/md.h"
class FileIO {
private:
  mbedtls_md_context_t ctx;

public:
  static constexpr const char* TEMP_BIN_FILE = "/download_firmware.bin";
  FileIO();
  void format();
  void listSPIFFS();
  fs::File openFile(String fileName, bool isReadOnly);
  void closeFile(fs::File file);
  void removeFile(String fileName);
  int getFileSize(String fileName);
  void mdContextInit();
  void mdContextUpdate(const unsigned char* buff, int c);
  String md5Result();
};
#endif