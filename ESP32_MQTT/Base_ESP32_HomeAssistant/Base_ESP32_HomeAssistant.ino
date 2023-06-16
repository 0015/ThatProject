/////////////////////////////////////////////////////////////////////////////////////////
/*
  Building a Smart Home with MQTT: Two-Way Communication using Home Assistant and ESP32
  For More Information: https://youtu.be/W0E-juQZDEY
  Created by Eric N. (ThatProject)
*/
/////////////////////////////////////////////////////////////////////////////////////////
/*
- ESP32 Arduino Core: 2.0.9
- TestDevice: ESP32-S3 DevKit-C
https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html

**Required Library**
- MQTT: 2.5.1
https://github.com/256dpi/arduino-mqtt

- ArduinoJSON: 6.21.2
https://arduinojson.org/
*/

#include <WiFiClientSecure.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include "ha_config.h"

const char ssid[] = "<YOUR_WIFI_SSID>";
const char pass[] = "<YOUR_WIFI_PASSWORD>";

WiFiClient net;
MQTTClient client;
unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("ESP32", CONFIG_BROKER_USERNAME, CONFIG_BROKER_PASSWORD)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  client.publish(CONFIG_TOPIC_SWITCH_STATE, payload);
}

void subscribeTopics() {
  client.subscribe(CONFIG_TOPIC_SWITCH_SET);
}

void publishDummyData() {
  Serial.println("------------------------");
  int dummyTemp = random(20, 35);
  client.publish(CONFIG_TOPIC_SENSOR_DATA, makeJsonString(CONFIG_SENSOR_BUILDIN_TEMP_VALUE_KEY, dummyTemp));

  int dummyHummy = random(0, 50);
  client.publish(CONFIG_TOPIC_SENSOR_DATA, makeJsonString(CONFIG_SENSOR_BUILDIN_HUMIDITY_VALUE_KEY, dummyHummy));

  int dummyCO2 = random(900, 1200);
  client.publish(CONFIG_TOPIC_SENSOR_DATA, makeJsonString(CONFIG_SENSOR_BUILDIN_CO2_VALUE_KEY, dummyCO2));

  int dummyTVOC = random(0, 500);
  client.publish(CONFIG_TOPIC_SENSOR_DATA, makeJsonString(CONFIG_SENSOR_BUILDIN_TVOC_VALUE_KEY, dummyTVOC));

  Serial.println("-----Dummy Data Sent----");
}

String makeJsonString(String key, int value) {
  StaticJsonDocument<128> doc;
  doc[key.c_str()] = value;
  String output;
  serializeJson(doc, output);
  Serial.println(output);
  return output;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  client.begin(CONFIG_BROKER_URL, 1883, net);
  client.onMessage(messageReceived);

  connect();
  subscribeTopics();
}

void loop() {
  client.loop();
  delay(10);

  if (!client.connected()) {
    connect();
  } else {
    if (millis() - lastMillis > 3000) {
      lastMillis = millis();
      publishDummyData();
    }
  }
}
