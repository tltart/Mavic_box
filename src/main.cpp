#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "Sokol_office";
const char* password = "12345679";
int LED=2;

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
      document.getElementById('btn').innerHTML = JSONobj.LEDonoff;

        if (JSONobj.LEDonoff == 'ON')
        {
          document.getElementById('btn').style.background='#FFA200';
          document.getElementById('btn').style["boxShadow"] = "0px 0px 0px 8px #FFA200";
        }
        else
        {
          document.getElementById('btn').style.background='#111111';
          document.getElementById('btn').style["boxShadow"] = "0px 0px 0px 8px #111111";
        }
    } // end of message
  }   // end of InitWebSocket

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
  <div class="temperature"><h2><p>Внутри</p></h2></div>

  


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
  pinMode(LED,OUTPUT);
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
  
  webSocket.loop();
  server.handleClient();

  if(LEDonoff == true){
    digitalWrite(LED, LOW);
  }
  else{
    digitalWrite(LED, HIGH);
  }
    String LEDswitch = "OFF";
    if(LEDonoff ==true) LEDswitch = "ON";
    JSONtxt = "{\"LEDonoff\":\""+LEDswitch+"\"}";
    webSocket.broadcastTXT(JSONtxt);
  
}