/////////////////////////////////////////////////////////////////
/*
  ESP32 | LVGL7 = My Shortcuts for Windows 10 = (ft. BLE HID Keyboard)
  Video Tutorial: https://youtu.be/EWKAyQBdcYM
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <BleKeyboard.h>
#include "MyShortcut.h"

MyShortcut::MyShortcut(BleKeyboard* _bleKeyboard, int caseId){
  bleKeyboard = _bleKeyboard;
  shortcutId = caseId;
}

void MyShortcut::RelaseAllkey(){
  bleKeyboard->releaseAll();
}

void MyShortcut::Action(){
  switch(shortcutId){
    //Task View
    case 0:  
      bleKeyboard->press(KEY_LEFT_CTRL);
      bleKeyboard->press(KEY_LEFT_SHIFT);
      bleKeyboard->press(KEY_ESC);
    break;

     // Close
    case 1:  
      bleKeyboard->press(KEY_LEFT_ALT);
      bleKeyboard->press(KEY_F4);
    break;

     // Minimize
    case 2:  
      bleKeyboard->press(KEY_LEFT_GUI);
      bleKeyboard->press(KEY_DOWN_ARROW);
    break;

    // Maximize
    case 3:  
      bleKeyboard->press(KEY_LEFT_GUI);
      bleKeyboard->press(KEY_UP_ARROW);
    break;

     // Task Switcher
    case 4:  
      bleKeyboard->press(KEY_LEFT_GUI);
      bleKeyboard->press(KEY_TAB);
    break;

    // Desktop
    case 5:  
      bleKeyboard->press(KEY_LEFT_GUI);
      bleKeyboard->press('d');

    break;

    // File Explore
    case 6:  
      bleKeyboard->press(KEY_MEDIA_LOCAL_MACHINE_BROWSER);
    break;

    // Lock 
    case 7:  
      bleKeyboard->press(KEY_LEFT_GUI);
      bleKeyboard->press('l');
    break;

    default:
    break;
  }
}