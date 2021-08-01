/////////////////////////////////////////////////////////////////
/*
  OTA Solution - Build your own OTA platform (2/2, ESP32 OTA Application)
  Video Tutorial: https://youtu.be/oNFqwvYYWe4
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////


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
