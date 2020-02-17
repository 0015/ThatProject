#include <EEPROM.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "FS.h"
#define GPSMODE 1
#define SERVERMODE 0
static const int RXPin = 5, TXPin = 16;
static const uint32_t GPSBaud = 9600;
const long GPSinterval = 15; //15sec Time_t

const int ADDR_EEPROM = 0;
const long LEDinterval = 1000; // 1sec

const char* ssid = "<YOUR_SSID>";
const char* password = "<YOUR_PASSWORD>";

bool isGPSMODE;
long prevMillis = 0;
int ledState = LOW;

const int offset = -8; // Pacific Standard Time
time_t prevDisplay = 0;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

bool isGPSDataAvailable;
char timeBuffer[16];
char fileNameBuffer[16];
String latitude, longitude;

ESP8266WebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  getModeFromEEPROM();

  if(!SPIFFS.begin()){
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  if(isGPSMODE){
    ss.begin(GPSBaud);
  }else{
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }

    Serial.print("\n\n Connected to ");
    Serial.println(ssid);
    Serial.print("Local Ip address: ");
    Serial.println(WiFi.localIP());

    if(MDNS.begin("esp8266")){
      Serial.println("MDNS responder started");
    }

    server.on("/", handleRoot);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started!");
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  isGPSMODE ? gpsMode() : serverMode();
}

void gpsMode(){
   while (ss.available() > 0){
      if (gps.encode(ss.read())){

        isGPSDataAvailable = false;
          
        int Year = gps.date.year();
        byte Month = gps.date.month();
        byte Day = gps.date.day();
        byte Hour = gps.time.hour();
        byte Minute = gps.time.minute();
        byte Second = gps.time.second();

        setTime(Hour, Minute, Second, Day, Month, Year);
        adjustTime(offset * SECS_PER_HOUR);

        if(gps.location.isValid()){
          
          latitude = String(gps.location.lat(), 6);
          longitude = String(gps.location.lng(), 6);
          isGPSDataAvailable = true;
        }       
      }

      if(timeStatus() != timeNotSet){
        if(now() - prevDisplay >= GPSinterval){
          prevDisplay = now();
          if(isGPSDataAvailable){
            dataLogging();
          }
        }
      }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void dataLogging(){
  Serial.println("\n\n Data Logging");
  sprintf(fileNameBuffer, "/%02u-%02u-%04u.csv", month(), day(), year());
  bool isFileExist = SPIFFS.exists(fileNameBuffer);
  Serial.print("isFileExist: ");
  Serial.println(isFileExist);

  File fileToAppend = SPIFFS.open(fileNameBuffer, "a");
  if(!fileToAppend){
    Serial.println("Error opening the file for appending");
    return;
  }

  if(!isFileExist){ //Header
    fileToAppend.print("Time");
    fileToAppend.print(",");
    fileToAppend.print("Latitude");
    fileToAppend.print(",");
    fileToAppend.println("Longitude");
  }
  
  sprintf(timeBuffer, "%02u:%02u:%02u", hour(), minute(), second());
  fileToAppend.print(timeBuffer);
  fileToAppend.print(",");
  fileToAppend.print(latitude);
  fileToAppend.print(",");
  fileToAppend.println(longitude);
  fileToAppend.close();
}

void serverMode(){
  ledBlink();
  server.handleClient();
  MDNS.update();
}

String listSPIFFS(){
  Serial.println("\n\n listing spiffs files");
  String response = "";
  fs::Dir dir = SPIFFS.openDir("/");
  while(dir.next()){
    String fileName = dir.fileName();
    fs:File f = dir.openFile("r");
    String fileSize = (String)f.size();
    response += String("<a href='") + fileName + String("'>") + fileName + "_" + fileSize + " bytes" + String("</a>") + String("</br>");
  }
  return String("List files:</br>") + response + String("</br>");
}

void handleRoot(){
  String res = listSPIFFS();
  server.send(200, "text/html", res);
}

void handleNotFound(){
  if(loadFromSPIFFS(server.uri())) return;
  String message = "SPIFFS not detected\n";
  server.send(404, "text/plain", message);
}

bool loadFromSPIFFS(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.htm";
  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if(!dataFile) return false;
  if(server.streamFile(dataFile, dataType) != dataFile.size()){
    Serial.println("Sent less data than expected");
  }
  dataFile.close();
  return true;
}

void ledBlink(){
  long currentMillis = millis();
  if(currentMillis - prevMillis >= LEDinterval){
    prevMillis = currentMillis;
    ledState = ledState == LOW ? HIGH : LOW;
    digitalWrite(LED_BUILTIN, ledState);
  }
}

void getModeFromEEPROM(){
  EEPROM.begin(128);

  int modeValue = readIntEEPROM(ADDR_EEPROM);
  if(modeValue != GPSMODE){
    Serial.println("\n\n=============");
    Serial.println("SERVERMODE");
    writeIntEEPROM(ADDR_EEPROM, GPSMODE);
    isGPSMODE = false;

    digitalWrite(LED_BUILTIN, HIGH);
  }else{
    Serial.println("\n\n=============");
    Serial.println("GPSMODE");
    writeIntEEPROM(ADDR_EEPROM, SERVERMODE);
    isGPSMODE = true;

    digitalWrite(LED_BUILTIN, LOW);
  }
}

int readIntEEPROM(int address){
  int val;
  byte* p = (byte*) &val;
  *p = EEPROM.read(address);
  *(p+1) = EEPROM.read(address + 1);
  *(p+2) = EEPROM.read(address + 2);
  *(p+3) = EEPROM.read(address + 3);
  return val;
}

void writeIntEEPROM(int address, int value){
  byte *p = (byte*) &value;
  EEPROM.write(address, *p);
  EEPROM.write(address+1, *(p+1));
  EEPROM.write(address+2, *(p+2));
  EEPROM.write(address+3, *(p+3));
  EEPROM.commit();
}
