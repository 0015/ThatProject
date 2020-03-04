void setup() {
  Serial.begin(115200);
  xTaskCreate(task_1, "task_1", 2048, NULL, 1, NULL);
  delay(500);
  xTaskCreate(task_2, "task_2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}

void task_1(void *pvParameter){
  int i=0;
  while(true){
    if(i>10) i=0;

    //Serial.print("[1] TASK value: ");
    //Serial.println(i);
    printf("[1] TASK value: %d\n", i);

    i++;

    vTaskDelay(125);
  }
}

void task_2(void *pvParameter){
  int i=0;
  while(true){
    if(i>10) i=0;

    Serial.print("\t\t\t\t\t[2] TASK value: ");
    Serial.println(i);

    i++;

    vTaskDelay(333);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
