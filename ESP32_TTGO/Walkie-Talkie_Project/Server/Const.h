typedef enum STATES {Idle, Listening, Speaking};
STATES states = Idle;

typedef enum ROLE {Server, Client};
ROLE deviceRole = Server;
    
const char* ssid = "ESP32-THAT-PROJECT";
const char* password = "California";