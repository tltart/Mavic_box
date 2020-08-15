#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define pult D7
#define ONE_WIRE_BUS D1 

byte TEC = D6;
byte FAN = D5;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
uint8_t sensor1[8] = { 0x28, 0xFF, 0x9E, 0x45, 0xA1, 0x15, 0x03, 0x90 };
uint8_t sensor2[8] = { 0x28, 0xFF, 0x65, 0x60, 0xA1, 0x15, 0x04, 0x34 };

int tempa_tmp_2=0;
int tempa_tmp_1=0;
int temp_2_json=0;
int temp_1_json=0;

const char* ssid = "First Floor1";
const char* password = "12345677";

int sen_2_on = 20;                                                            // порог включения пельтье на охлаждение
int sen_2_off = 15;                                                           // выключение пельтье
int sen_1_high = 50;                                                          // вентилятора на 100%
int sen_1_low = 30;                                                           // вентилятор 40%
int tempC_1;
int tempC_2;
int TEC_work;
int FAN_speed;


ESP8266WebServer server(80);
WebSocketsServer webSocket=WebSocketsServer(88);
String webSite, JSONtxt;
boolean pult_on_off=false;
boolean pult_res=false;
String stat_power = "OFF";

void on_pult();
void off_pult();

const char webSiteCont[] PROGMEM = 
  R"=====(
    
  <!DOCTYPE HTML>
  <HTML>
  <META name='viewport' content='width=device-width, initial-scale=1', charset='UTF-8'>

  <SCRIPT>

    InitWebSocket()
    function InitWebSocket()
    {
      websock = new WebSocket('ws://'+window.location.hostname+':88/');
      websock.onmessage=function(evt)
      {
        JSONobj = JSON.parse(evt.data);
        document.getElementById('t_1').innerHTML = JSONobj.temp_1;
        document.getElementById('t_2').innerHTML = JSONobj.temp_2;
        document.getElementById('tec').innerHTML = JSONobj.TEC;
        document.getElementById('fan').innerHTML = JSONobj.FAN;
        document.getElementById('stat_').innerHTML = JSONobj.stat_power;

        if(JSONobj.stat_power == 'ON')
        {
         document.getElementById('stat_').style.background='#4CAF50';
        }
       else{document.getElementById('stat_').style.background='#3f1fce';}
      }
    }

    function button_on(){
      btn = 'pult_on_off=ON';
      if(document.getElementById('stat_').innerHTML === 'ON')
      {
        btn = 'pult_on_off=OFF';
      }
      websock.send(btn);
    }

    function button_res(){
      btn = 'pult_on_off=RES';
      if(document.getElementById('stat_').innerHTML === 'ON')
      {
        btn = 'pult_on_off=RES';
      }
      websock.send(btn);
    }


  </SCRIPT>

  <style>

    table, td, th {
      margin: auto;    
      table-layout: fixed;
      width: 35%;
      text-align: center;
    }

    table {
      border-collapse: collapse;
    }

    th{
      background-color: #4CAF50;
      width: 2%;
      }
    td{
      width: 100px;
      background-color: #00e1ff;
      }
    p {
      color: rgb(146, 26, 5);
      text-align: center;
      } 

      .button {
      background-color: #4CAF50;
      border: none;
      color: white;
      padding: 15px 32px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 4px 2px;
      border-radius: 8px;
      cursor: pointer;
    }

    .center {
      margin: auto;
      width: 50%;
      padding: 10px;
      text-align: center;
    } 


  </style>

  <BODY>
    <h1><p>Чудо - коробка V.2.01</p></h1>
    <table>
      <tr>
        <th><h2><p>Снаружи</p></h2></th>
        <th><h2><p>Работа вентилятора</p></h2></th>
      </tr>
      <tr>    
         <th><div id="t_1"><h3><p></p></h3> </div></th>
         <th><div id="fan"><h3><p></p></h3> </div></th>
      </tr>
      <tr>
        <td><h2><p>Внутри</p></h2> </td>
        <td><h2><p>Работа TEC</p> </h2></td>
      </tr>
      <tr>
        <td><div id="t_2"><h3><p></p></h3> </div></td>
        <td><div id="tec"><h3><p></p></h3> </div></td>
      </table>
    <div class="center">
    <a href="#" class="button" id="button" ONCLICK='button_on()'> ON/OFF </a>
    <a href="#" class="button" id="button" ONCLICK='button_res()'> Restart </a>
    <button class="button" id="stat_"></a>
    </div>



  </BODY>

  </HTML>  
  )=====";



void Website(){
  server.send(200, "text/html", webSiteCont);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t welenght){
  String payloadString = (const char *)payload;
  Serial.print("payloadString= ");
  Serial.println(payloadString);
  Serial.println(type);
  Serial.print("WStype = "); Serial.println(type); 
  Serial.print("WS payload = ");

  if(type == WStype_TEXT){
    webSocket.broadcastTXT(JSONtxt);
    byte separator=payloadString.indexOf('=');
    String var = payloadString.substring(0,separator);
    Serial.print("var= ");
    Serial.println(var);
    String val = payloadString.substring(separator+1);
    Serial.print("val= ");
    Serial.print(val);
    Serial.println(" ");

    if (var == "pult_on_off"){
      // pult_on_off = true;
      if(val =="ON")
      {
        pult_on_off=true;
        on_pult();
      }
      if(val=="OFF")
      {
        pult_on_off=false;
        off_pult();
      }
      if(val=="RES" && pult_on_off==true)
      {
          off_pult();
          delay(5000);
          on_pult();
      }
    }
  }
}

void setup(){

  Serial.begin(115200);
  sensors.begin();
  WiFi.begin(ssid, password);
  pinMode(TEC, OUTPUT);                                    // TEC init
  digitalWrite(TEC, LOW);
  pinMode(FAN, OUTPUT);                                    // FAN init
  analogWrite(FAN, 400);
  pinMode(pult, OUTPUT);
  digitalWrite(pult, LOW);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  WiFi.mode(WIFI_STA);
  Serial.println(" Start ESP ");
  Serial.println(WiFi.localIP());
  server.on("/", Website);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop(){
  
  sensors.requestTemperatures();
  int tec_power(DeviceAddress deviceAddress);
  tec_power(sensor2);
  int fan_speed(DeviceAddress deviceAddress);
  fan_speed(sensor1);
  webSocket.loop();
  server.handleClient();



    if (tempa_tmp_2 != tempC_2){
      temp_2_json = tempa_tmp_2;  
      tempa_tmp_2 = tempC_2;
      }
    if (tempa_tmp_1 != tempC_1){
      temp_1_json = tempa_tmp_1;
      tempa_tmp_1 = tempC_1;
      }

    if(pult_on_off) {stat_power = "ON";}
    else {stat_power = "OFF";}


  JSONtxt = "{\"temp_1\":\""+String(temp_1_json)+"\""+","+"\"temp_2\":\""+String(temp_2_json)+
              "\""+","+"\"TEC\":\""+String(TEC_work)+"\""+","+"\"FAN\":\""+String(FAN_speed)+"\","+"\"stat_power\":\""+stat_power+"\"}";
  Serial.println(JSONtxt);
  webSocket.broadcastTXT(JSONtxt);


}
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
      