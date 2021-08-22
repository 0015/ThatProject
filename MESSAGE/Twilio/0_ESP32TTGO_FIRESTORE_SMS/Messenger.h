#ifndef Messenger_H_
#define Messenger_H_

#include "twilio.hpp"

class Messenger {
private:
  Twilio *twilio;

public:
  Messenger();
  ~Messenger();
  void sendMessage(String msg);
};

#endif