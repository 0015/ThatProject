#include "Network.h"
#include <WiFi.h>
#define WIFI_SSID "ThatProject"
#define WIFI_PASS "California"
#define BASE_URL "http://192.168.0.2:9001/api"

const String API_KEY = "THIS_IS_MY_OWN_API_KEY";

Network::Network() {
  localServerTime = 0;
}

void Network::WiFiBegin() {
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");
}

void Network::fetchLocalServerTime() {
  if ((WiFi.status() == WL_CONNECTED)) {
    String targetURL = BASE_URL;
    targetURL += "/get/time";

    http.begin(targetURL);

    if (http.GET() == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);

      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      String time = doc["timestamp"];
      Serial.println(time);
      Serial.println(time.toInt());
      localServerTime = time.toInt();

    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }
}

long Network::getLocalServerTime() {
  return localServerTime;
}

Firmware Network::checkVersion() {

  Firmware firmware;
  firmware.build_num = -1;

  if ((WiFi.status() == WL_CONNECTED)) {

    String targetURL = BASE_URL;
    targetURL += "/get/version";
    http.begin(targetURL);

    if (http.GET() == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);

      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return firmware;
      }

      firmware.company = doc["companyName"].as<String>();
      firmware.build_num = doc["buildNum"];
      firmware.build_date = doc["buildDate"].as<String>();
      firmware.server_file_path = doc["serverFilePath"].as<String>();
      firmware.file_size = doc["fileSize"];
      firmware.md5_checksum = doc["md5Checksum"].as<String>();

    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }

  return firmware;
}

String Network::fileDownload(FuncPtrInt callback, FileIO** fileIO, String target_path) {

  String md5CheckSum = "wrong";

  if ((WiFi.status() == WL_CONNECTED)) {
    fs::File file = (*fileIO)->openFile(FileIO::TEMP_BIN_FILE, false);

    String targetURL = BASE_URL;
    targetURL += "/post/update";

    http.begin(targetURL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + API_KEY + "&target_path=" + target_path;

    if (file && http.POST(httpRequestData) == HTTP_CODE_OK) {
      callback(0);
      (*fileIO)->mdContextInit();

      int fileSize = http.getSize();
      Serial.print("File Length: ");
      Serial.println(fileSize);

      int unDownloadSize = fileSize;
      int downloadSize = 0;
      int preDownloadedPercent = 0;

      uint8_t buff[128] = { 0 };

      WiFiClient* stream = http.getStreamPtr();
      while (http.connected() && (fileSize > 0 || fileSize == -1)) {
        size_t size = stream->available();
        if (size) {
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          file.write(buff, c);
          (*fileIO)->mdContextUpdate(buff, c);

          if (fileSize > 0) fileSize -= c;

          downloadSize += c;
          int downloadedPercent = int((downloadSize * 100 / unDownloadSize));

          if (preDownloadedPercent != downloadedPercent) {
            callback(downloadedPercent);
            preDownloadedPercent = downloadedPercent;
          }
        }

        delay(1);
      }

      Serial.println();
      Serial.print("[HTTP] connection closed or file end.\n");


      (*fileIO)->closeFile(file);
      (*fileIO)->listSPIFFS();
      Serial.println("======MD5 CHECKSUM");
      md5CheckSum = (*fileIO)->md5Result();
      Serial.println(md5CheckSum);


    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }

  return md5CheckSum;
}
