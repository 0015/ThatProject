#include "Messenger.h"

// Values from Twilio (find them on the dashboard)
static const char *account_sid = "";
static const char *auth_token = "";
// Phone number should start with "+<countrycode>"
static const char *from_number = "";

// You choose!
// Phone number should start with "+<countrycode>"
static const char *to_number = "";

Messenger::Messenger() {
  twilio = new Twilio(account_sid, auth_token);
}

Messenger::~Messenger() {
  delete twilio;
}

void Messenger::sendMessage(String msg) {

  if (WiFi.status() != WL_CONNECTED) return;

  String response;
  bool success = twilio->send_message(to_number, from_number, msg, response);
  if (success) {
    Serial.println("Sent message successfully!");
  } else {
    Serial.println(response);
  }
}