#include <Arduino.h>
#include <BleKeyboard.h>
#include "BLECont.h"
#include "MyShortcut.h"

#define ALIVE_INTERVAL 100

BLECont::BLECont() {
  bleKeyboard = new BleKeyboard("My Shortcuts v2", "That Project", 100);
  isConnected = false;
  previousTimestamp = 0;
}

void BLECont::begin() {
  bleKeyboard->begin();
}

void BLECont::loop() {
  unsigned long currentTimestamp = millis();
  if (currentTimestamp - previousTimestamp > ALIVE_INTERVAL) {
    previousTimestamp = currentTimestamp;

    if (isConnected != bleKeyboard->isConnected()) {
      isConnected = bleKeyboard->isConnected();
      callback(isConnected);
    }
  }
}

void sendTask(void *pvParam) {
  MyShortcut *_myShortcut = reinterpret_cast<MyShortcut *>(pvParam);
  _myShortcut->Action();
  vTaskDelay(100);
  _myShortcut->RelaseAllkey();
  delete _myShortcut;
  vTaskDelete(NULL);
}

void BLECont::triggerTask(int caseNum) {
  if (!isConnected) return;

  MyShortcut *myShortcut = new MyShortcut(bleKeyboard, caseNum);
  xTaskCreate(
    sendTask,           /* Function that implements the task. */
    "sendTask",         /* Text name for the task. */
    4096,               /* Stack size in words, not bytes. */
    (void *)myShortcut, /* Parameter passed into the task. */
    1,                  /* Priority at which the task is created. */
    NULL);              /* Used to pass out the created task's handle. */
}
