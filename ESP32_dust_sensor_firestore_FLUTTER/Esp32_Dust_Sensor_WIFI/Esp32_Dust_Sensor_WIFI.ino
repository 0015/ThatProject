#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#define        COV_RATIO                       0.2            //ug/mmm / mv
#define        NO_DUST_VOLTAGE                 400            //mv
#define        SYS_VOLTAGE                     5000           

const char* ssid = "ANSBRICK";
const char* password = "dostmqmflr223"; 

const int iled = 12;                                            //drive the led of sensor
const int vout = 36;                                            //analog input
float density, voltage;
int   adcvalue;

int Filter(int m)
{
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;
  
  if(flag_first == 0)
  {
    flag_first = 1;

    for(i = 0, sum = 0; i < _buff_max; i++)
    {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  }
  else
  {
    sum -= _buff[0];
    for(i = 0; i < (_buff_max - 1); i++)
    {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];
    
    i = sum / 10.0;
    return i;
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(iled, OUTPUT);
  digitalWrite(iled, LOW);                                     //iled default closed
  
  Serial.begin(115200);                                         //send and receive at 115200 baud

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print("Connected to this Wifi with IP: ");
    Serial.println(WiFi.localIP());
  }

}

void UpdateDustSensorValue(){
   digitalWrite(iled, HIGH);
  delayMicroseconds(280);
  adcvalue = analogRead(vout);
  digitalWrite(iled, LOW);
  
  adcvalue = Filter(adcvalue);
  
  /*
  covert voltage (mv)
  */
  voltage = (SYS_VOLTAGE / 1024.0) * adcvalue * 11;
  
  /*
  voltage to density
  */
  if(voltage >= NO_DUST_VOLTAGE)
  {
    voltage -= NO_DUST_VOLTAGE;
    
    density = voltage * COV_RATIO;
  }
  else
    density = 0;
    
  /*
  display the result
  */
  Serial.print("The current dust concentration is: ");
  Serial.print(density);
  Serial.print(" ug/m3\n"); 
}

void loop() {
  // put your main code here, to run repeatedly:
  if(WiFi.status() == WL_CONNECTED){
    UpdateDustSensorValue();
    HTTPClient http;
    http.begin("https://dust-sensor-cloud.firebaseapp.com/api/v1/sensor/9vga20P652xhl6pgLidS");
    http.addHeader("Content-Type", "application/json");
    
    StaticJsonDocument<20> tempDocument;
    tempDocument["dust"] = density;
    char buffer[20];
    serializeJson(tempDocument, buffer);
    http.PATCH(buffer);
    http.end();
    
    
  }else{
    Serial.println("Check your wifi connection");
  }

  delay(2000);

}
