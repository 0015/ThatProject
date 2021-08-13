#include "Network.h"

#define WIFI_SSID "ThatProject"
#define WIFI_PASSWORD "California"

Network::Network() {
}

void WiFiEventConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WIFI CONNECTED! BUT WAIT FOR THE LOCAL IP ADDR");
}

void WiFiEventGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.print("LOCAL IP ADDRESS: ");
  Serial.println(WiFi.localIP());
}

void WiFiEventDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WIFI DISCONNECTED!");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void Network::initWiFi() {
  WiFi.disconnect();
  WiFi.onEvent(WiFiEventConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiEventGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiEventDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}