#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>

#define pult D7
#define ONE_WIRE_BUS D1                                                        // линия данных подключена к цифровому выводу 2 Arduino

OneWire oneWire(ONE_WIRE_BUS);                                                // настройка объекта oneWire для связи с любым устройством OneWire
DallasTemperature sensors(&oneWire);                                          // передать ссылку на oneWire библиотеке DallasTemperature

uint8_t sensor1[8] = { 0x28, 0xFF, 0x9E, 0x45, 0xA1, 0x15, 0x03, 0x90 };      // адреса трех датчиков DS18B20
uint8_t sensor2[8] = { 0x28, 0xFF, 0x65, 0x60, 0xA1, 0x15, 0x04, 0x34 };      // адреса трех датчиков DS18B20
// uint8_t sensor2[8] = { 0x28, 0xFF, 0x13, 0x0E, 0xA1, 0x15, 0x03, 0xC3 };
int sen_2_on = 20;                                                            // порог включения пельтье на охлаждение
int sen_2_off = 15;                                                           // выключение пельтье
int sen_1_high = 50;                                                          // вентилятора на 100%
int sen_1_low = 30;                                                           // вентилятор 40%
int tempC_1;
int tempC_2;
int TEC_work;
int FAN_speed;

const char* off_ = "OFF";
const char* ON_ = "ON";
const char* res_ = "RES";

const char* ssid     = "Sokol_office";
const char* password = "12345679";
WiFiServer server(80);
WiFiClient client;
boolean flagEmptyLine = true;
char tempChar;
char urnFromRequest[51];  // строка URN из запроса
boolean urnReceived= false; // признак URN принят
unsigned int indUrn;  // адрес в строке URN

boolean on_flag = false; // признак пульт выключен

byte TEC = D6;
byte FAN = D5;

void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
  pinMode(TEC, OUTPUT);                                    // TEC init
  digitalWrite(TEC, LOW);
  pinMode(FAN, OUTPUT);                                    // FAN init
  analogWrite(FAN, 400);
  pinMode(pult, OUTPUT);
  digitalWrite(pult, LOW);
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
    urnReceived = false;
    indUrn=0xffff;
    Serial.println("New request from client:");

    while (client.connected()) {
      if (client.available()) {
        tempChar = client.read();
        // Serial.write(tempChar);
         if( urnReceived == false ) {
          
          if( indUrn == 0xffff ) {
            // пропуск метода
            if( tempChar == '/' ) indUrn=0;}
          else {
            // запись строки
            if( tempChar == ' ' ) {
              // URN закончен
                urnFromRequest[indUrn]=0;
                urnReceived = true;              
            }
            else {
              // загрузка символа URN в строку
              urnFromRequest[indUrn] = tempChar;
              indUrn++;
              if( indUrn > 49 ) {
                // переполнение
                urnFromRequest[50]=0;
                urnReceived = true;
              }
            }            
          }          
        }
        void on_pult();
        void off_pult();
        if (tempChar == '\n' && flagEmptyLine) {
          if( (strcmp(urnFromRequest, "ON") == 0) && on_flag == false) {
            on_pult();
            on_flag = true;}
          else if( (strcmp(urnFromRequest, "OFF") == 0) && on_flag == true){ 
            off_pult();
            on_flag = false;}
          else if( (strcmp(urnFromRequest, "RES") == 0) && on_flag == true){ 
            off_pult();
            delay(4000);
            on_pult();}
          // пустая строка, ответ клиенту
          client.println("HTTP/1.1 200 OK"); // стартовая строка
          client.println("Content-Type: text/html; charset=utf-8"); // тело передается в коде HTML, кодировка UTF-8
          client.println("Connection: close"); // закрыть сессию после ответа
          // client.println(F("Refresh: 2"));  // обновить страницу автоматически
          client.println(); // пустая строка отделяет тело сообщения
          client.println("<!DOCTYPE HTML>"); // тело сообщения
          client.println("<html>");
          client.println("<div style=\"text-align: center;\"><font face=\"Calibri\" size=\"3\"><b>Чудо-коробка.</b></font></div><div style=\"text-align: center;\"><font face=\"Calibri\"><br></font></div><div style=\"text-align: center;\"><font face=\"Calibri\" size=\"3\"><b>");

          client.print("</b></font></div><div style=\"text-align: center;\"><font face=\"Calibri\">&nbsp;");
          client.print("<p><span style=\"background-color: #55FF00; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;\">");
          client.print("t° радиатора: ");
          client.print(tempC_1);
          // client.print("</span>&nbsp;</p>");
          client.print("<p><span style=\"background-color: #55FF00; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;\">");
          client.print("работа FAN на: ");
          client.print(FAN_speed);
          client.print("</span>&nbsp;</p>");
          // client.print("работа FAN на: ");
          // client.print(FAN_speed);
          client.print("</b></font></div><div style=\"text-align: center;\"><font face=\"Calibri\">&nbsp;");
          client.print("<p><span style=\"background-color: #0008FF; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;\">");
          client.print("t° внутри: ");
          client.print(tempC_2);
          // client.print("</span>&nbsp;</p>");
          client.print("<p><span style=\"background-color: #0008FF; color: #fff; display: inline-block; padding: 3px 10px; font-weight: bold; border-radius: 5px;\">");
          client.print("TEC работает на: ");
          client.print(TEC_work);
          client.print("</span>&nbsp;</p>");

          // client.print("&nbsp;</font></div><div style=\"text-align: center;\"><font face=\"Calibri\" size=\"3\"><b>");
          // client.print("t° внутри: ");
          // client.print(tempC_2);
          // client.print("</b></font></div><div style=\"text-align: center;\"><font face=\"Calibri\">");
          // client.print("TEC работает на: ");
          // client.print(TEC_work);
          client.println("</font></div><div style=\"text-align: center;\"><br></div><span style=\"white-space:pre\"><div style=\"text-align: center;\">    </div></span><div style=\"text-align: center;\"><br></div>");


          // if( digitalRead(2) == HIGH ) client.println("пульт включен");            
          if( strcmp(urnFromRequest, "ON") == 0 ) client.println("пульт включен");            

          else client.println(F("пульт выключен"));            
          client.print("</font><br><br><font size=\"4\"><a href=\"http://192.168.88.245/");
          client.print(ON_);
          client.print("\"><button><font size=\"4\">");
          client.print("Включить пульт");
          client.print("</font></button></a></font><br><br><font size=\"4\"><a href=\"http://192.168.88.245/");
          client.print(off_);
          client.println("\"><button><font size=\"4\">Выключить пульт</font></button></a></font>");
          client.print("</font></button></a></font><br><br><font size=\"4\"><a href=\"http://192.168.88.245/");
          client.print(res_);
          client.println("\"><button><font size=\"4\">Выключить пульт</font></button></a></font>");
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
      digitalWrite(TEC, HIGH);
      // Serial.print(tempC_2);
      // Serial.println("TEC is working...");
      TEC_work = 98;
      }
    else if(tempC_2 < sen_2_off){
      // Serial.println(tempC_2);
      digitalWrite(TEC, LOW);
      TEC_work = 20;
      }
    else{
      return 0;}
  }
    

int fan_speed(DeviceAddress deviceAddress){
    
    tempC_1 = sensors.getTempC(deviceAddress);
    if(tempC_1 > sen_1_high){
      analogWrite(FAN, 1000);
      FAN_speed = 98;
      // Serial.println("FAN is working on 99%...");
      }
    else if(tempC_1 < sen_1_low){
      // Serial.println(tempC_1);
      analogWrite(FAN, 400);
      FAN_speed = 40;
      // Serial.println("FAN is working on 40%...");}
      }
      else {
        return 0;}
    }
  
void on_pult(){
  Serial.println("Вызвана функция рестарта пульта");
  Serial.println("1 нажатие"); 
  digitalWrite(pult, HIGH);
  delay(500);
  digitalWrite(pult, LOW);
  Serial.println("Кнопка отпущена...");
  delay(400);
  Serial.println("2 нажатие");
  digitalWrite(pult, HIGH);
  delay(3000);
  digitalWrite(pult, LOW);
  Serial.println("Отущена после длительного нажатия");
  }
void off_pult(){
  Serial.println("Вызвана функция рестарта пульта");
  Serial.println("1 нажатие"); 
  digitalWrite(pult, HIGH);
  delay(300);
  digitalWrite(pult, LOW);
  Serial.println("Кнопка отпущена...");
  delay(600);
  Serial.println("2 нажатие");
  digitalWrite(pult, HIGH);
  delay(3000);
  digitalWrite(pult, LOW);
  Serial.println("Отущена после длительного нажатия");
  }
      