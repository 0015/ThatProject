#include <ESP8266WiFi.h>
#include <espnow.h>
#include "SPI.h"
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();

typedef struct coordinate {
  int x;
  int y;
}coordinate_t;

typedef struct esp_now_peer_info {
  u8 peer_addr[6];
  uint8_t channel;
  uint8_t encrypt;
  coordinate_t coord;
}esp_now_peer_info_t;

// Global copy of slave
#define NUMSLAVES 20
esp_now_peer_info_t slaves[NUMSLAVES] = {};
int SlaveCnt = 0;

#define CHANNEL 3
#define PRINTSCANRESULTS 0

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == 0) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    ESP.restart();
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
}

// Scan for slaves in AP mode
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  //reset slaves
  memset(slaves, 0, sizeof(slaves));
  SlaveCnt = 0;
  Serial.println("");
  if (scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      if (PRINTSCANRESULTS) {
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
      }
      delay(10);
      // Check if the current device starts with `Slave`
      if (SSID.indexOf("Slave") == 0) {
        // SSID of interest
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];

        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            slaves[SlaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
          }
        }

        int xCoord = map(RSSI, -20, -150, 100, 300);
        int yCoord = (SlaveCnt+1) * 70;
        slaves[SlaveCnt].coord.x = xCoord;
        slaves[SlaveCnt].coord.y = yCoord;
        slaves[SlaveCnt].channel = CHANNEL; // pick a channel
        slaves[SlaveCnt].encrypt = 0; // no encryption
        SlaveCnt++;
      }
    }
  }

  if (SlaveCnt > 0) {
    Serial.print(SlaveCnt); Serial.println(" Slave(s) found, processing..");
  } else {
    Serial.println("No Slave Found, trying again.");
  }

  // clean up ram
  WiFi.scanDelete();
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
void manageSlave() {
  if (SlaveCnt > 0) {
    for (int i = 0; i < SlaveCnt; i++) {
      const esp_now_peer_info_t *peer = &slaves[i];
      u8 *peer_addr = slaves[i].peer_addr;
      
      Serial.print("Processing: ");
      for (int ii = 0; ii < 6; ++ii ) {
        Serial.print((uint8_t) slaves[i].peer_addr[ii], HEX);
        if (ii != 5) Serial.print(":");
      }
      Serial.print(" Status: ");
      // check if the peer exists
      bool exists = esp_now_is_peer_exist((u8*)peer_addr);
      if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
      } else {
        // Slave not paired, attempt pair
        int addStatus = esp_now_add_peer((u8*)peer_addr, ESP_NOW_ROLE_CONTROLLER, CHANNEL, NULL, 0);
        if (addStatus == 0) {
          // Pair success
          Serial.println("Pair success");
        } else {
          Serial.println("Pair failed");
        }
        delay(100);
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
  }
}



// send data
void sendData() {

  for (int i = 0; i < SlaveCnt; i++) {
    u8 *peer_addr = slaves[i].peer_addr;

    char macStr_1[9];
    snprintf(macStr_1, sizeof(macStr_1), "%02X:%02X:%02X",
           slaves[i].peer_addr[0], slaves[i].peer_addr[1], slaves[i].peer_addr[2]);

    char macStr_2[9];
    snprintf(macStr_2, sizeof(macStr_2), "%02X:%02X:%02X",
           slaves[i].peer_addr[3], slaves[i].peer_addr[4], slaves[i].peer_addr[5]);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(macStr_1, slaves[i].coord.x, slaves[i].coord.y, 1);
    tft.drawString(macStr_2, slaves[i].coord.x, slaves[i].coord.y + 10, 1);
    
    uint8_t data = random(1, 100);
    int result = esp_now_send(peer_addr, &data, sizeof(data));

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString(String(data), slaves[i].coord.x + 55, slaves[i].coord.y, 2);
    
    Serial.print("Send Status: ");
    if (result == 0) {
      Serial.println("Success");
      tft.fillCircle(slaves[i].coord.x + 50, slaves[i].coord.y -5, 6, TFT_GREEN);
    } else {
      Serial.println("Failed");
      tft.fillCircle(slaves[i].coord.x + 50, slaves[i].coord.y -5, 6, TFT_RED);
    }
    delay(100);
  }
}

// callback when data is sent from Master to Slave
esp_now_send_cb_t OnDataSent([](uint8_t *mac_addr, uint8_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == 0 ? "Delivery Success" : "Delivery Fail");
});

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.fillCircle(-180, 120, 260, TFT_WHITE);
  tft.fillCircle(-200, 120, 260, TFT_BLACK);
  tft.drawString("MASTER", 6, 108, 2);
  
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESPNow/Multi-Slave/Master Example");
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
}

void loop() {
  // In the loop we scan for slave
  ScanForSlave();

  tft.fillRect(90, 0, 320, 240, TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  String numOfSlaves = "SLAVE COUNT: ";
  numOfSlaves += String(SlaveCnt);
  tft.drawString(numOfSlaves, 200, 4, 2);
  // If Slave is found, it would be populate in `slave` variable
  // We will check if `slave` is defined and then we proceed further
  if (SlaveCnt > 0) { // check if slave channel is defined
    // `slave` is defined
    // Add slave as peer if it has not been added already
    manageSlave();
    // pair success or already paired
    // Send data to device
    sendData();
  } else {
    // No slave found to process
  }

  // wait for 1 second to run the logic again
  delay(1000);
}
