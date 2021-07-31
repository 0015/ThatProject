#include "Button2.h"
class ButtonCont {
private:
  Button2* button;
  friend void receiveCallback(Button2& btn);
  typedef void (*FuncPtrVoid)(void);

public:
  FuncPtrVoid released_cb;
  ButtonCont(FuncPtrVoid f);
  ButtonCont();
  ~ButtonCont();

  void loop();
  void eventTrigger();
};