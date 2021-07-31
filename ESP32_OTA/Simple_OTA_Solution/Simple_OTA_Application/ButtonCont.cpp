#include "ButtonCont.h"
#define RIGHT_BUTTON_PIN 35

static ButtonCont* instance = NULL;

void released(Button2& btn) {
  instance->released_cb();
}

ButtonCont::ButtonCont() {}

ButtonCont::ButtonCont(FuncPtrVoid f) {
  instance = this;
  button = new Button2(RIGHT_BUTTON_PIN);
  button->setReleasedHandler(released);
  released_cb = f;
}

ButtonCont::~ButtonCont() {
  delete button;
}

void ButtonCont::loop() {
  button->loop();
}