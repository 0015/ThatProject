void setup() {
  Serial.begin(115200);
  Serial.println("Serial Comm Via USB UART");
  Serial.flush();

}

void loop() {
  
  while(Serial.available() > 0){
    uint8_t byteFromSerial = Serial.read();
    uint8_t buff[100] = {byteFromSerial};
    String str = (char*)buff;
    Serial.print(str);
  }

}
