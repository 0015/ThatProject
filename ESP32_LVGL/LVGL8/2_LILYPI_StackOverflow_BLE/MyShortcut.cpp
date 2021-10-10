#include <Arduino.h>
#include <BleKeyboard.h>
#include "MyShortcut.h"
typedef uint8_t MediaKeyReport[2];

MyShortcut::MyShortcut(BleKeyboard * _bleKeyboard, int caseId)
{
  bleKeyboard = _bleKeyboard;
  shortcutId = caseId;
}

void MyShortcut::RelaseAllkey(){
  bleKeyboard->releaseAll();
}

void MyShortcut::Action(){
  switch(shortcutId){
    //Execute shortcut
    case 0:  
      bleKeyboard->press(KEY_LEFT_SHIFT);
      bleKeyboard->press(KEY_LEFT_ALT);
      bleKeyboard->press('c');
    break;

    //Print String with return
    case 1:  
      bleKeyboard->print("www.stackoverflow.com");
      delay(500);
      bleKeyboard->write(KEY_RETURN);
    break;

     //Select All
    case 2:  
      bleKeyboard->press(KEY_LEFT_CTRL);
      bleKeyboard->press('a');
    break;

     //Copy
    case 3:  
      bleKeyboard->press(KEY_LEFT_CTRL);
      bleKeyboard->press('c');
    break;

    //Paste
    case 4:  
      bleKeyboard->press(KEY_LEFT_CTRL);
      bleKeyboard->press('v');
    break;
    default:
    break;
  }
}