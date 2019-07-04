#include "SPIFFS.h"

void setup(void) {
  Serial.begin(115200);
  Serial.println("File Test!"); 

  if (!SPIFFS.begin()) {
      Serial.println("SPIFFS initialisation failed!");
      while (1) yield(); // Stay here twiddling thumbs waiting
    }
    Serial.println("\r\nInitialisation done.");
    Serial.println("\n----DIR: /:");
    listDir("/");

}


void loop(void) {
  
}


void listDir(char * dir){
 
  File root = SPIFFS.open(dir);
  File file = root.openNextFile();
  while(file){
 
      Serial.print("FILE: ");
      Serial.println(file.name());
      file = root.openNextFile();
  }
}
