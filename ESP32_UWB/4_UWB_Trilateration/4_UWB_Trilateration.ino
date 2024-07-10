/////////////////////////////////////////////////////////////////
/*
  UWB Trilateration with ESP32, DW3000 #LocalPositioning
  For More Information: https://youtu.be/NjHMXEOxNRE
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////
// Version Info
// ESP32                v3.0.2
// Hardware Info
// https://github.com/Makerfabs/MaUWB_DW3000-with-STM32-AT-Command/

#include "config.h"
#include "Trilateration.hpp"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

HardwareSerial SERIAL_AT(2);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

String response = "";

Anchor anchor0(-340, 240);
Anchor anchor1(340, 240);
Anchor anchor2(0, -240);

Trilateration trilateration(&anchor0, &anchor1, &anchor2);

void setup() {
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, HIGH);

  SERIAL_LOG.begin(115200);

  xTaskCreatePinnedToCore(Task_UWB,
                          "Task_UWB",
                          1024 * 20,
                          NULL,
                          tskIDLE_PRIORITY,
                          NULL,
                          0);
}

void loop() {}

void drawScreen() {
  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.println(F("UWB Trilateration"));
  display.setCursor(0, 10);

  String freqText = "Freq: ";

#ifdef FREQ_850K
  freqText = freqText + "850k";
#else
  freqText = freqText + "6.8M";
#endif
  display.println(freqText);
  display.setTextSize(2);
  display.setCursor(0, 20);

  String infoText = "";

#ifdef TAG
  infoText = infoText + "Tag: " + UWB_INDEX;
#else
  infoText = infoText + "Anchor: " + UWB_INDEX;
#endif
  display.println(infoText);

#ifdef TAG
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println("User Location:  ");

#else
  display.setCursor(0, 50);
  display.setTextSize(1);
  display.println("......Updating......");
#endif
  display.display();

  delay(1000);
}

String sendData(String command, const int timeout, boolean debug) {
  String response = "";

  SERIAL_LOG.println(command);
  SERIAL_AT.println(command);  // send the read character to the SERIAL_LOG

  long int time = millis();

  while ((time + timeout) > millis()) {
    while (SERIAL_AT.available()) {

      // The esp has data so display its output to the serial window
      char c = SERIAL_AT.read();  // read the next character.
      response += c;
    }
  }
  return response;
}

String config_cmd() {
  String temp = "AT+SETCFG=";

  // Set device id
  temp = temp + UWB_INDEX;

  // Set device role
#ifdef TAG
  temp = temp + ",0";
#endif
#ifdef ANCHOR
  temp = temp + ",1";
#endif

  // Set frequence 850k or 6.8M
#ifdef FREQ_850K
  temp = temp + ",0";
#endif
#ifdef FREQ_6800K
  temp = temp + ",1";
#endif

  // Set range filter
  temp = temp + ",1";

  return temp;
}

String cap_cmd() {
  String temp = "AT+SETCAP=";

  // Set Tag capacity
  temp = temp + UWB_TAG_COUNT;

  //  Time of a single time slot
#ifdef FREQ_850K
  temp = temp + ",15";
#endif
#ifdef FREQ_6800K
  temp = temp + ",10";
#endif

  return temp;
}



void Task_UWB(void *pvParameters) {

  SERIAL_AT.begin(115200, SERIAL_8N1, IO_RXD2, IO_TXD2);

  SERIAL_AT.println("AT");
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(1000);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3C for 128x32
    SERIAL_LOG.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();

  drawScreen();

  sendData("AT?", 2000, 1);
  //sendData("AT+RESTORE", 5000, 1);

  sendData(config_cmd(), 2000, 1);
  sendData(cap_cmd(), 2000, 1);

  sendData("AT+SETRPT=1", 2000, 1);
  //sendData("AT+SAVE", 2000, 1);
  sendData("AT+RESTART", 2000, 1);


  while (1) {

#ifdef TAG
    tag_action();
#else
    vTaskDelay(10);
#endif
  }
}

void tag_action() {
  while (SERIAL_LOG.available() > 0) {
    SERIAL_AT.write(SERIAL_LOG.read());
    yield();
  }
  while (SERIAL_AT.available() > 0) {
    char c = SERIAL_AT.read();

    if (c == '\r')
      continue;
    else if (c == '\n' || c == '\r') {
      SERIAL_LOG.println(response);

      float rangeValues[3];
      // Parse data, update distances, and calculate user location
      if (parseData(response, rangeValues)) {
        updateAnchorDistances(rangeValues);
        calculateUserLocation();
      }

      response = "";
      vTaskDelay(10);
    } else
      response += c;
  }
}

bool parseData(String dataString, float rangeValues[3]) {
  // Find the position of "range:("
  int rangeStart = dataString.indexOf("range:(");
  if (rangeStart == -1) return false;  //  Return false if "range:(" not found

  // Extract the substring containing the range values
  int rangeEnd = dataString.indexOf(")", rangeStart);
  String rangePart = dataString.substring(rangeStart + 7, rangeEnd);

  // Parse the range values
  char charArray[rangePart.length() + 1];
  rangePart.toCharArray(charArray, rangePart.length() + 1);
  char *token = strtok(charArray, ",");
  int valueIndex = 0;
  while (token != NULL && valueIndex < 3) {
    rangeValues[valueIndex] = atoi(token);
    valueIndex++;
    token = strtok(NULL, ",");
  }

  return true;
}

// Function to update the distances for the anchors
void updateAnchorDistances(float rangeValues[3]) {
  anchor0.updateDistance(rangeValues[0]);
  anchor1.updateDistance(rangeValues[1]);
  anchor2.updateDistance(rangeValues[2]);
}

// Function to perform trilateration and calculate user location
void calculateUserLocation() {
  float user_x, user_y;
  trilateration.calcUserLocation(user_x, user_y);

  char buffer[100];
  snprintf(buffer, sizeof(buffer), "x = %.2f\ny = %.2f", user_x, user_y);
  Serial.println(buffer);

  display.fillRect(0, 48, 128, 16, SSD1306_BLACK);
  display.setCursor(0, 48);
  display.print(buffer);
  display.display();
}