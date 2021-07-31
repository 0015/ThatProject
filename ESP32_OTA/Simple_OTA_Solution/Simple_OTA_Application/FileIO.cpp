#include "FileIO.h"

FileIO::FileIO() {
  if (!SPIFFS.begin()) {
    //if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield();
  }
}

void FileIO::format() {
  Serial.println("SPIFFS Format!");
  SPIFFS.format();
}

void FileIO::listSPIFFS() {
  Serial.println(F("\r\nListing SPIFFS files:"));
  static const char line[] PROGMEM = "=================================================";

  Serial.println(FPSTR(line));
  Serial.println(F("  File name                              Size"));
  Serial.println(FPSTR(line));

  fs::File root = SPIFFS.open("/");
  if (!root) {
    Serial.println(F("Failed to open directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("Not a directory"));
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      // File path can be 31 characters maximum in SPIFFS
      int spaces = 33 - fileName.length();  // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String)file.size();
      spaces = 10 - fileSize.length();  // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }

  Serial.println(FPSTR(line));
  Serial.println();
}

fs::File FileIO::openFile(String fileName, bool isReadOnly) {
  if (isReadOnly) {
    return SPIFFS.open(fileName, "r");
  } else {
    this->removeFile(fileName);
    return SPIFFS.open(fileName, "w");
  }
}

void FileIO::closeFile(fs::File file) {
  file.close();
}

void FileIO::removeFile(String fileName) {
  if (SPIFFS.exists(fileName)) {
    SPIFFS.remove(fileName);
  }
}

int FileIO::getFileSize(String fileName) {
  fs::File file = SPIFFS.open(fileName, "r");

  if (!file) {
    return 0;
  }

  int fileSize = file.size();
  file.close();
  return fileSize;
}

void FileIO::mdContextInit() {
  mbedtls_md_type_t md_type = MBEDTLS_MD_MD5;
  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
}

void FileIO::mdContextUpdate(const unsigned char* buff, int c) {
  mbedtls_md_update(&ctx, buff, c);
}

String FileIO::md5Result() {
  byte md5Result[16];
  mbedtls_md_finish(&ctx, md5Result);
  mbedtls_md_free(&ctx);

  String checksum = "";
  for (int i = 0; i < sizeof(md5Result); i++) {
    char str[3];

    sprintf(str, "%02x", (int)md5Result[i]);
    checksum += str;
  }
  return checksum;
}