typedef enum STATES {Idle, Listening, Speaking};
STATES states = Idle;

typedef enum ROLE {Server, Client};
ROLE deviceRole = Client;
    
const char* ssid = "ESP32-THAT-PROJECT";
const char* password = "California";

const char* websockets_server_host = "192.168.4.1";
const uint16_t websockets_server_port = 8888;