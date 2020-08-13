#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D3 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
uint8_t sensor2[8] = { 0x28, 0xFF, 0x13, 0x0E, 0xA1, 0x15, 0x03, 0xC3 };

int tempa=0;

const char* ssid = "Sokol_office";
const char* password = "12345679";
int LED=D3;



ESP8266WebServer server(80);
WebSocketsServer webSocket=WebSocketsServer(88);
String webSite, JSONtxt;
boolean LEDonoff=true;

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
    }
  }


  function button(){
    btn = 'LEDonoff=ON';
      if(document.getElementById('btn').innerHTML === 'ON')
      {
        btn = 'LEDonoff=OFF';
      }
      websock.send(btn);
  }

</SCRIPT>

<style>
  #btn{
    display: inline-block;
    text-decoration:none;
    background: #8cd460;
    color: rgba(255,255,255, 0.80);
    font-weight: bold;
    font: 100px arial, sans-serif;
    width: 320px;
    height: 320px;
    line-height: 320px;
    border-radius: 50%;
    margin: 30%;
    margin-top: 60px;
    text-align: center;
    vertical-align: middle;
    overflow: hidden;
    box-shadow: 0px 0px 0px 8px #8cd460;
    border: solid 2px rgba(255,255,255, 0.47);
    transition: 0.4s;
    }
  p {
    color: rgb(255, 0, 128);
    text-align: center;
    } 
  
  div.temperature {
    position: relative;
    margin: 0 auto;
    height: 200px;
    width: 50%;
    background-color: rgb(169, 207, 212);
    }


</style>

<BODY>
  <h1><p>Чудо - коробка V.2.01</p></h1>
  <!-- <a href="#" id="btn" ONCLICK='button()'> </a> -->
  <div class="temperature"><h2><p>Снаружи</p></h2></div>
  <div id="t_1"> </div>
  <div class="temperature"><h2><p>Внутри</p></h2></div>
  <div id="t_2"> </div>

  


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

  if(type == WStype_TEXT){
    byte separator=payloadString.indexOf('=');
    String var = payloadString.substring(0,separator);
    Serial.print("var= ");
    Serial.println(var);
    String val = payloadString.substring(separator+1);
    Serial.print("val= ");
    Serial.print(val);
    Serial.println(" ");

    if (var == "LEDonoff"){
      LEDonoff = false;
      if(val =="ON")LEDonoff=true;
    }
  }
}

void setup(){

  Serial.begin(115200);
  sensors.begin();
  WiFi.begin(ssid, password);
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
  int tempC_2 = sensors.getTempC(sensor2);
  Serial.println(tempC_2);
  webSocket.loop();
  server.handleClient();

  if (tempa != tempC_2){
    
    JSONtxt = "{\"temp_1\":\""+String(tempa)+"\"}";
    tempa = tempC_2;
    webSocket.broadcastTXT(JSONtxt);
    }

    Serial.println(JSONtxt);


  // if(LEDonoff == true){
  //   digitalWrite(LED, LOW);
  // }
  // else{
  //   digitalWrite(LED, HIGH);
  // }
  //   String LEDswitch = "OFF";
  //   if(LEDonoff ==true) LEDswitch = "ON";
  //   JSONtxt = "{\"LEDonoff\":\""+LEDswitch+"\"}";
  //   webSocket.broadcastTXT(JSONtxt);
  
}