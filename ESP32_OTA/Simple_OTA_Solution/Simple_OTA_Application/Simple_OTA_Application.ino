#include "SimpleOTA.h"

SimpleOTA *simpleOTA;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  simpleOTA = new SimpleOTA();
  simpleOTA->begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  simpleOTA->loop();
  delay(10);
}
