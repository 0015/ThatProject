#include <Arduino.h>
#include "SimpleOTA.h"

static SimpleOTA *instance = NULL;

SimpleOTA::SimpleOTA() {
  t1 = 0;
  t2 = 0;
  currentState = NONE;
  instance = this;
}

SimpleOTA::~SimpleOTA() {
}

void buttonCallbackEvent() {
  Serial.println("buttonCallbackEvent");
  if (instance->currentState == SERVER_FOUND && instance->version->hasNewUpdate()) {
    instance->startDownload();
  }
}

void networkDownloadEvent(int percent) {
  Serial.print("networkDownloadEvent downloadPercent: ");
  if (percent == 0) {
    instance->currentState = FIRMWARE_DOWNLOAD_START;
  }
  Serial.println(percent);
  instance->display->downloadScreen(percent);
}

void SimpleOTA::begin() {
  this->initDisplay();
  this->initVersion();
  this->initNetwork();
  this->initFileIO();
  this->initButton();
}

void SimpleOTA::loop() {

  switch (currentState) {
    case NETWORK_CONNECTED:

      if (millis() - t1 >= 1000) {
        t1 = millis();
        this->requestLocalServerTime();
      }
      break;

    case SERVER_FOUND:
      if (millis() - t1 >= 1000) {
        t1 = millis();
        this->updateTime();
        display->showVersionBelow(version->getCurrentVersion());
      }

      if (millis() - t2 >= 1000 * 10) {
        t2 = millis();
        this->serverFirmwareCheck();
      }
      break;

    default: break;
  }

  buttonCont->loop();
}

void SimpleOTA::initDisplay() {
  Serial.println("initDisplay");
  display = new Display();
  display->initTFT();
}

void SimpleOTA::initVersion() {
  Serial.println("initVersion");
  version = new VersionCont();
  display->showVersion(version->getCurrentVersion());
}

void SimpleOTA::initNetwork() {
  Serial.println("initNetwork");
  network = new Network();
  currentState = NETWORK_BEGIN;
  network->WiFiBegin();
  currentState = NETWORK_CONNECTED;
  this->requestLocalServerTime();
}

void SimpleOTA::initFileIO() {
  Serial.println("initFileIO");
  fileIO = new FileIO();
  //fileIO->format();
  fileIO->listSPIFFS();
}

void SimpleOTA::initButton() {
  Serial.println("initButton");

  void (*ptr)(void) = &buttonCallbackEvent;
  buttonCont = new ButtonCont(ptr);
}

void SimpleOTA::requestLocalServerTime() {
  network->fetchLocalServerTime();

  if (network->getLocalServerTime() != 0) {
    rtc.setTime(network->getLocalServerTime());
    display->fillBlackScreen();
    currentState = SERVER_FOUND;
  }
}

void SimpleOTA::updateTime() {
  struct tm timeinfo = rtc.getTimeStruct();
  display->timeUpdate(rtc.getDate(), rtc.getTime());
}

void SimpleOTA::serverFirmwareCheck() {
  version->setNewFirmware(network->checkVersion());
  if (version->newFirmwareVersion() == -1) {
    display->newMessage("Server Not Responding");
  } else {
    if (version->hasNewUpdate()) {
      display->newMessage("New Build Available! ->");
    }
  }
}

void SimpleOTA::startDownload() {

  void (*ptr)(int) = &networkDownloadEvent;

  bool compareMD5Checksum = version->md5CompareTo(network->fileDownload(ptr, &fileIO, version->getFirmwareServerPath()));
  bool compareFileSize = version->fileSizeCompareTo(fileIO->getFileSize(FileIO::TEMP_BIN_FILE));

  Serial.println("======compareMD5Checksum");
  Serial.println(compareMD5Checksum);
  Serial.println("======Downloaded File SIze");
  Serial.println(compareFileSize);

  if (compareMD5Checksum && compareFileSize) {
    display->downloadSuccess();
    this->updateFirmware();
  } else {
    display->downloadFailure(version->getCPName());
    delay(5000);
    display->fillBlackScreen();
    currentState = SERVER_FOUND;
  }
}

void SimpleOTA::updateFirmware() {
  currentState = FIRMWARE_DOWNLOAD_START;
  display->firmwareScreen(true, false);
  Updater *updater = new Updater();
  if (updater->updateFromFS(&fileIO)) {
    Serial.println("UPDATE DONE");

    currentState = FIRMWARE_UPDATED;
    version->saveVersion(version->newFirmwareVersion());
    display->firmwareScreen(false, true);

    ESP.restart();
  } else {
    Serial.println("UPDATE FAILURE");
    display->firmwareScreen(false, false);
    delay(3000);
    display->fillBlackScreen();
    currentState = SERVER_FOUND;
  }
  delete updater;
}
