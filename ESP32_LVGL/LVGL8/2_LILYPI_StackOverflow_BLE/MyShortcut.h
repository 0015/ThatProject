#pragma once

class BleKeyboard;
class MyShortcut {
private:
  BleKeyboard *bleKeyboard;
  int shortcutId;

public:
  MyShortcut(BleKeyboard *bleKeyboard, int caseId);
  void Action();
  void RelaseAllkey();
};