#ifndef Network_H_
#define Network_H_

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

typedef enum {
  NETWORK_CONNECTED,
  NETWORK_DISCONNECTED,
  FIREBASE_CONNECTED,
  FIREBASE_DISCONNECTED
} Network_State_t;

class Network{
private:
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;

  typedef void (*FuncPtrInt)(Network_State_t);

  void firebaseInit();
  friend void WiFiEventConnected(WiFiEvent_t event, WiFiEventInfo_t info);
  friend void WiFiEventGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
  friend void WiFiEventDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
  friend void FirestoreTokenStatusCallback(TokenInfo info);

public:
  FuncPtrInt callBackEvent;
  Network();
  Network(FuncPtrInt f);
  void initWiFi();
  void firestoreDataUpdate(double temp, double humi);
};


#endif