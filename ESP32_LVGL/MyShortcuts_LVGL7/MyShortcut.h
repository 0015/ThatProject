/////////////////////////////////////////////////////////////////
/*
  ESP32 | LVGL7 = My Shortcuts for Windows 10 = (ft. BLE HID Keyboard)
  Video Tutorial: https://youtu.be/EWKAyQBdcYM
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#pragma once

class BleKeyboard;

class MyShortcut
{
  private:
    BleKeyboard* bleKeyboard;
    int shortcutId;

  public:
    MyShortcut(BleKeyboard* bleKeyboard, int caseId);
    void Action();
    void RelaseAllkey();
};