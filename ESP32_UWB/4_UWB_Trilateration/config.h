#include <Arduino.h>

// UWB config 
#define TAG
//#define ANCHOR
#define UWB_INDEX 1

#define FREQ_850K
//#define FREQ_6800K
#define UWB_TAG_COUNT 10

#define RESET 32
#define IO_RXD2 18
#define IO_TXD2 19
#define I2C_SDA 4
#define I2C_SCL 5

#define SERIAL_LOG Serial
#define SERIAL_AT mySerial2