#ifndef MyFirmware_H_
#define MyFirmware_H_
#include <Arduino.h>

struct firmware_t {
  String company;
  int build_num;
  String build_date;
  String server_file_path;
  int file_size;
  String md5_checksum;
};

typedef struct firmware_t Firmware;

#endif
