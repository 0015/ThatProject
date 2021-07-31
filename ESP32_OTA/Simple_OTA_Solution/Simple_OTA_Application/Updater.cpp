#include "Updater.h"

void Updater::performUpdate(Stream& updateSource, size_t updateSize) {
  if (Update.begin(updateSize)) {
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      Serial.println("Written : " + String(written) + " successfully");
    } else {
      Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
    }
    if (Update.end()) {
      Serial.println("OTA done!");
      if (Update.isFinished()) {
        Serial.println("Update successfully completed. Rebooting.");
      } else {
        Serial.println("Update not finished? Something went wrong!");
      }
    } else {
      Serial.println("Error Occurred. Error #: " + String(Update.getError()));
    }

  } else {
    Serial.println("Not enough space to begin OTA");
  }
}

// check given FS for valid update.bin and perform update if available
bool Updater::updateFromFS(FileIO** fileIO) {

  fs::File updateBin = (*fileIO)->openFile(FileIO::TEMP_BIN_FILE, true);

  if (updateBin) {
    if (updateBin.isDirectory()) {
      Serial.println("Error, update.bin is not a file");
      return false;
    }
    bool updateResult = false;
    size_t updateSize = updateBin.size();

    if (updateSize > 0) {
      Serial.println("Try to start update");
      this->performUpdate(updateBin, updateSize);
      updateResult = true;
    } else {
      Serial.println("Error, file is empty");
      updateResult = false;
    }

    (*fileIO)->closeFile(updateBin);
    (*fileIO)->removeFile(FileIO::TEMP_BIN_FILE);
    return updateResult;
  } else {
    Serial.println("Could not load update.bin from SPIFFS");
    return false;
  }
}