#include "Network.h"
#include "addons/TokenHelper.h"

#define WIFI_SSID "<YOUR_WIFI_SSID>"
#define WIFI_PASSWORD "<YOUR_WIFI_PASSWORD>"

#define API_KEY "<YOUR_WEB_API_KEY>"
#define FIREBASE_PROJECT_ID "<YOUR_PROJECT_ID>"
#define USER_EMAIL "<YOUR_USER_EMAIL>"
#define USER_PASSWORD "<YOUR_USER_PASSWORD>"

static Network *instance = NULL;

Network::Network(){}

Network::Network(FuncPtrInt f){
  instance = this;
  callBackEvent = f;
}

void WiFiEventConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WIFI CONNECTED! BUT WAIT FOR THE LOCAL IP ADDR");
}

void WiFiEventGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.print("LOCAL IP ADDRESS: ");
  Serial.println(WiFi.localIP());
  instance->callBackEvent(NETWORK_CONNECTED);
  instance->firebaseInit();
}

void WiFiEventDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WIFI DISCONNECTED!");
  instance->callBackEvent(NETWORK_DISCONNECTED);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void FirestoreTokenStatusCallback(TokenInfo info){
  Serial.printf("Token Info: type = %s, status = %s\n", getTokenType(info).c_str(), getTokenStatus(info).c_str());
  if(info.status == token_status_ready){
    instance->callBackEvent(FIREBASE_CONNECTED);
  }else{
    instance->callBackEvent(FIREBASE_DISCONNECTED);
  }
}

void Network::initWiFi(){
  WiFi.disconnect();
  WiFi.onEvent(WiFiEventConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiEventGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiEventDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void Network::firebaseInit(){
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.token_status_callback = FirestoreTokenStatusCallback;

  Firebase.begin(&config, &auth);
}

void Network::firestoreDataUpdate(double temp, double humi){
  if(WiFi.status() == WL_CONNECTED && Firebase.ready()){
    String documentPath = "House/Room_1";

    FirebaseJson content;

    content.set("fields/temperature/doubleValue", String(temp).c_str());
    content.set("fields/humidity/doubleValue", String(humi).c_str());

    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "temperature,humidity")){
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }

    if(Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw())){
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
  }
}













