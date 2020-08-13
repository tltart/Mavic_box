#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
// #include <JSON_R_W.h>

const char* ssid = "Sokol_office";
const char* password = "12345679";

// String configJson = "{}";

ESP8266WebServer server(80);
WebSocketsServer webSocket=WebSocketsServer(81);

char webpage[] PROGMEM = R"=====(
<!DOCTYPE html>
    <html>
   <head>
      <script type = "text/javascript">
        var ws = new WebSocket('ws://'+window.location.hostname+':81/');
 
        ws.onopen = function() {
            window.alert("Connected");
         };
 
         ws.onmessage = function(evt) {
            document.getElementById("display").innerHTML  = "temperature: " + evt.data + " C";
        };
 
      </script>
   </head>
 
   <body>
      <div>
         <p id = "display">Not connected</p>
      </div>
   </body>
</html>


)=====";

void Website(){
  server.send(200, "text/html", webpage);
}
//===============================================================================================================================//
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t welenght){
 
  if(type == WStype_TEXT){
 
    Serial.println("Websocket client connection received");
    String payloadString = (const char *)payload;
    Serial.println(payloadString);
 
  } else if(type == WStype_DISCONNECTED){
 
    Serial.println("Websocket client connection finished");
    // webSocket.sendTXT = NULL;
 
  }
}
void setup()
{
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", Website);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop(){
   if(!WStype_DISCONNECTED){
      String randomNumber = String(random(0,20));
      webSocket.broadcastTXT(randomNumber);
   }
   delay(4000);
}