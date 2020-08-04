#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define ONE_WIRE_BUS D4                                                        // линия данных подключена к цифровому выводу 2 Arduino

OneWire oneWire(ONE_WIRE_BUS);                                                // настройка объекта oneWire для связи с любым устройством OneWire
DallasTemperature sensors(&oneWire);                                          // передать ссылку на oneWire библиотеке DallasTemperature

uint8_t sensor1[8] = { 0x28, 0xFF, 0x9E, 0x45, 0xA1, 0x15, 0x03, 0x90 };      // адреса трех датчиков DS18B20
uint8_t sensor2[8] = { 0x28, 0xFF, 0x65, 0x60, 0xA1, 0x15, 0x04, 0x34 };      // адреса трех датчиков DS18B20
int sen_2_on = 20;                                                            // порог включения пельтье на охлаждение
int sen_2_off = 15;                                                           // выключение пельтье
int sen_1_high = 50;                                                          // вентилятора на 100%
int sen_1_low = 30;                                                           // вентилятор 40%
int tempC_1;
int tempC_2;
int TEC_work;
int FAN_speed;



const char* ssid     = "Sokol_office";
const char* password = "12345679";
WiFiServer server(80);
WiFiClient client;
boolean flagEmptyLine = true;
char tempChar;

byte TEC = D6;
byte FAN = D5;

void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
  pinMode(TEC, OUTPUT);                                    // TEC init
  analogWrite(TEC, 0);
  pinMode(FAN, OUTPUT);                                    // FAN init
  pinMode(FAN, 0);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
 }

void loop(void)
{
  sensors.requestTemperatures();                            
  // void printTemperature(DeviceAddress deviceAddress); // sensors                         
  // Serial.print("Sensor 1: ");                         // sensors 
  // printTemperature(sensor1);                          // sensors 
  // Serial.print("Sensor 2: ");                         // sensors 
  // printTemperature(sensor2);                          // sensors 
  // Serial.println();                                   // sensors
  // delay(1000);                                        // sensors
  int tec_power(DeviceAddress deviceAddress);
  tec_power(sensor2);
  int fan_speed(DeviceAddress deviceAddress);
  fan_speed(sensor1);

  client = server.available(); // ожидаем объект клиент
  if (client) {
    flagEmptyLine = true;
    Serial.println("New request from client:");

    while (client.connected()) {
      if (client.available()) {
        tempChar = client.read();
        Serial.write(tempChar);

        if (tempChar == '\n' && flagEmptyLine) {
          // пустая строка, ответ клиенту
          client.println("HTTP/1.1 200 OK"); // стартовая строка
          client.println("Content-Type: text/html; charset=utf-8"); // тело передается в коде HTML, кодировка UTF-8
          client.println("Connection: close"); // закрыть сессию после ответа
          client.println(F("Refresh: 2"));  // обновить страницу автоматически
          client.println(); // пустая строка отделяет тело сообщения
          client.println("<!DOCTYPE HTML>"); // тело сообщения
          client.println("<html>");
          client.println("<div style=\"text-align: center;\"><font face=\"Calibri\" size=\"3\"><b>Чудо-коробка.</b></font></div><div style=\"text-align: center;\"><font face=\"Calibri\"><br></font></div><div style=\"text-align: center;\"><font face=\"Calibri\" size=\"3\"><b>");
          client.print("t° радиатора: ");
          client.print(tempC_1);
          client.print("</b></font></div><div style=\"text-align: center;\"><font face=\"Calibri\">&nbsp;");
          client.print("работа FAN на: ");
          client.print(FAN_speed);
          client.print("&nbsp;</font></div><div style=\"text-align: center;\"><font face=\"Calibri\" size=\"3\"><b>");
          client.print("t° внутри: ");
          client.print(tempC_2);
          client.print("</b></font></div><div style=\"text-align: center;\"><font face=\"Calibri\">");
          client.print("TEC работает на: ");
          client.print(TEC_work);
          client.println("</font></div><div style=\"text-align: center;\"><br></div><span style=\"white-space:pre\"><div style=\"text-align: center;\">    </div></span><div style=\"text-align: center;\"><br></div>");
          client.println("</html>");
          break;
        }
        if (tempChar == '\n') {
          // новая строка
          flagEmptyLine = true;
        }
else if (tempChar != '\r') {
          // в строке хотя бы один символ
          flagEmptyLine = false;
        }
      }
    }
    delay(1);
    // разрываем соединение
    client.stop();
    Serial.println("Break");
  }

}



// void printTemperature(DeviceAddress deviceAddress)         //сбор и контроль датчиков температуры// {
//   float tempC = sensors.getTempC(deviceAddress);
//   Serial.print(tempC);
//   Serial.print((char)176);
//   Serial.print("C  |  ");
//   }

int tec_power(DeviceAddress deviceAddress){
    tempC_2 = sensors.getTempC(deviceAddress);
    if (tempC_2 > sen_2_on) {  
      analogWrite(TEC, 1000);
      Serial.print(tempC_2);
      Serial.println("TEC is working...");
      TEC_work = 98;}
    if(tempC_2 < sen_2_off){
      Serial.println(tempC_2);
      analogWrite(TEC, 200);
      TEC_work = 20;}
    return 0;
  }

int fan_speed(DeviceAddress deviceAddress){
    
    tempC_1 = sensors.getTempC(deviceAddress);
    if(tempC_1 > sen_1_high){
      analogWrite(FAN, 1000);
      FAN_speed = 98;
      // Serial.println("FAN is working on 99%...");
      }
    if(tempC_1 < sen_1_low){
      Serial.println(tempC_1);
      analogWrite(FAN, 400);
      FAN_speed = 40;}
      // Serial.println("FAN is working on 40%...");
    return 0;
    }
  
      