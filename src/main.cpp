#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D4                                                        // линия данных подключена к цифровому выводу 2 Arduino

OneWire oneWire(ONE_WIRE_BUS);                                                // настройка объекта oneWire для связи с любым устройством OneWire
DallasTemperature sensors(&oneWire);                                          // передать ссылку на oneWire библиотеке DallasTemperature

uint8_t sensor1[8] = { 0x28, 0xFF, 0x9E, 0x45, 0xA1, 0x15, 0x03, 0x90 };      // адреса трех датчиков DS18B20
uint8_t sensor2[8] = { 0x28, 0xFF, 0x65, 0x60, 0xA1, 0x15, 0x04, 0x34 };      // адреса трех датчиков DS18B20
int sen_2 = 30;                                                               // порог включения пельтье на охлаждение
int sen_1 = 50;


byte TEC = D6;
byte FAN = D5;

void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
  pinMode(TEC, OUTPUT);                                    // TEC init
  analogWrite(TEC, 0);
  pinMode(FAN, OUTPUT);
  pinMode(FAN, 0);
 }

void loop(void)
{
  sensors.requestTemperatures();                      // sensors     
  // void printTemperature(DeviceAddress deviceAddress); // sensors                         
  // Serial.print("Sensor 1: ");                         // sensors 
  // printTemperature(sensor1);                          // sensors 
  // Serial.print("Sensor 2: ");                         // sensors 
  // printTemperature(sensor2);                          // sensors 
  // Serial.println();                                   // sensors
  // delay(1000);                                        // sensors
  void tec_power(DeviceAddress deviceAddress);
  tec_power(sensor2);
  void fan_speed(DeviceAddress deviceAddress);
  fan_speed(sensor1);

}



// void printTemperature(DeviceAddress deviceAddress)         //сбор и контроль датчиков температуры// {
//   float tempC = sensors.getTempC(deviceAddress);
//   Serial.print(tempC);
//   Serial.print((char)176);
//   Serial.print("C  |  ");
//   }

void tec_power(DeviceAddress deviceAddress){
  int tempC = sensors.getTempC(deviceAddress);
    if (tempC > sen_2) {  
      analogWrite(TEC, 1000);
      Serial.print(tempC);
      Serial.println("TEC is working...");}
    else{
      Serial.println(tempC);
      analogWrite(TEC, 0);}
  }

  void fan_speed(DeviceAddress deviceAddress){
      int tempC = sensors.getTempC(deviceAddress);
      if(tempC > sen_1){
        analogWrite(FAN, 1000);
        Serial.println("FAN is working on 99%...");
        }
      else
      {
        analogWrite(FAN, 400);
        Serial.println("FAN is working on 40%...");
      }
  }
      