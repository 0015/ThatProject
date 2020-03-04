#include "slm.h"
#include "ble.h"

void setup() {
  Serial.begin(115200);
  task_slm();
  delay(500);
  task_ble();
}

void task_slm(){
  samples_queue = xQueueCreate(8, sizeof(sum_queue_t));  
  xTaskCreate(mic_i2s_reader_task, "Mic I2S Reader", I2S_TASK_STACK, NULL, I2S_TASK_PRI, NULL);
}

void task_ble(){
  setupBLE();
  xTaskCreate(ble_Notifier, "BLE Notifier", BLE_TASK_STACK, NULL, BLE_TASK_PRI, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}
