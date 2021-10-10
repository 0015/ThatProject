#pragma once
class BleKeyboard;
class BLECont {
private:
  bool isConnected;
  void (*callback)(bool);
  BleKeyboard *bleKeyboard;
  unsigned long previousTimestamp;
  friend void sendTask(void *pvParameters);

public:
  BLECont();
  void begin();
  void loop();
  void triggerTask(int caseNum);
  void bindCallback(void (*fn)(bool)) {
    callback = fn;
  }
};