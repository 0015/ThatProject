/////////////////////////////////////////////////////////////////
/*
  ESP32 | LVGL7 = My Shortcuts for Windows 10 = (ft. BLE HID Keyboard)
  Video Tutorial: https://youtu.be/EWKAyQBdcYM
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <BleKeyboard.h>
#include "BLECont.h"
#include "MyShortcut.h"

#define ALIVE_INTERVAL 100

BLECont::BLECont(){
  bleKeyboard = new BleKeyboard("My Shortcuts", "That Project", 100);
  isConnected = false;
  previousTimestamp = 0;
}

void BLECont::begin(){
  bleKeyboard->begin();
}

void BLECont::loop(){
  unsigned long currentTimestamp = millis();
  if (currentTimestamp - previousTimestamp > ALIVE_INTERVAL) {
    previousTimestamp = currentTimestamp;
    isConnected = bleKeyboard->isConnected();
    callback(isConnected); 
  }
}

void sendTask(void* pvParam){
  MyShortcut* _myShortcut = reinterpret_cast<MyShortcut*>(pvParam);
  _myShortcut->Action();
  vTaskDelay(100);
  _myShortcut->RelaseAllkey();
  
  delete _myShortcut;
  
  vTaskDelete(NULL);
}

void BLECont::triggerTask(int caseNum){
  if(!isConnected) return;

  MyShortcut* myShortcut = new MyShortcut(bleKeyboard, caseNum);
  
  xTaskCreate(
    sendTask, 
    "sendTask",
    4096,     
    (void*) myShortcut,
    1,
    NULL);
}