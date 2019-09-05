#include "EEPROM.h"
#define EEPROM_SIZE 64

#include <WiFi.h>
#include <ESP32Ping.h>
const int wifiAddr = 0;
const char* remote_host = "www.google.com";
const unsigned long period = 30000; //30 seconds
unsigned long startMillis;
unsigned long currentMillis;
String incoming = "";
String savedData;

void setup() {
  Serial.begin(115200);
  Serial.println("Update WIFI credential via USB");

  if(!EEPROM.begin(EEPROM_SIZE)){
    delay(1000);
  }

  startMillis = millis();
  currentMillis = millis();
  savedData = read_String(wifiAddr);
  wifiConnect();
}

void loop() {
  while(Serial.available() > 0){
    incoming = Serial.readString();
    Serial.println(incoming);
    if(incoming.length() >0 && incoming.indexOf(",") >0){
      String wifiName = getValue(incoming,',',0);
      String wifiPW = getValue(incoming,',',1);
      Serial.print("NEW Wifi Name : ");
      Serial.println(wifiName);

      Serial.print("NEW Wifi PW : ");
      Serial.println(wifiPW);
      writeString(wifiAddr, incoming);
    }
  }

}

void writeString(int add, String data){
  int _size = data.length();
  for(int i=0; i<_size; i++){
    EEPROM.write(add+i, data[i]);
  }
  EEPROM.write(add+_size, '\0');
  EEPROM.commit();
}

String read_String(int add){
  char data[100];
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  while(k!='\0' && len<500){
    k = EEPROM.read(add+len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}

void wifiConnect(){
  if(savedData.length()>0){
    String wifiName = getValue(savedData,',',0);
    String wifiPW = getValue(savedData,',',1);
    Serial.print("Wifi Name : ");
    Serial.println(wifiName);

    Serial.print("Wifi PW : ");
    Serial.println(wifiPW);
    
    WiFi.begin(wifiName.c_str(), wifiPW.c_str());
    Serial.print("Connecting to WIFI");

    while(WiFi.status() != WL_CONNECTED && currentMillis - startMillis <= period){
      delay(300);
      currentMillis = millis();
      Serial.print("Time Lapse: ");
      Serial.println(currentMillis - startMillis);
    }

    if(WiFi.status() == WL_CONNECTED){
      Serial.println();
      Serial.print("Connected with IP : ");
      Serial.println(WiFi.localIP());

      Serial.print("Ping Host: ");
      Serial.println(remote_host);

      if(Ping.ping(remote_host)){
        Serial.println("Success!!");
      }else{
        Serial.println("Error!!");
      }
      
    }else{
      Serial.println();
      Serial.println("Please check your wifi ssid & pw");
    }
  }
}

String getValue(String data,char separator, int index){
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found <=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i==maxIndex)? i+1: i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]): "";
}
