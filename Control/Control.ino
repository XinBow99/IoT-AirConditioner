#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <IRsend.h>

#ifndef STASSID
#define STASSID "BUFFALO-F8C98C"
#define STAPSK  "b3229931543"
#endif

IRsend irsend(4);

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);
/******************************************/
void rootRouter() {
  File file = SPIFFS.open("/index.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}
void sendRedLaser(int action[]) {
  //0x2B948B7
  unsigned long HEX_A[] = {
    0x2B948B7,
    0x2B938C7,
    0x2B9A857,
    0x2B9F00F,
    0x2B9C837
  };
  unsigned long HEX_TEMP[] = {
    0x2B9807F,
    0x2B940BF,
    0x2B9C03F,
    0x2B958A7,
    0x2F920DF,
    0x2B9A05F,
    0x2F9609F,
    0x2B97887,
    0x2F9E01F,
    0x2B910EF,
    0x2B9906F,
    0x2B9D827
  };
  unsigned long HEX_TIME[] = {
    0x2B9B04F,
    0x2B9708F,
    0x2B9B847,
    0x2B99867,
    0x2BD6897,
    0x2B928D7,
    0x2B918E7,
    0x2B930CF
  };
  unsigned long HEX_F[] = {
    0x2B950AF,
    0x2B900FF,
    0x2BDE817,
    0x2B9F807
  };
  if (action[0] == 0) {
    irsend.sendNEC(HEX_A[action[1]], 32 );
  } else if (action[0] == 1) {
    irsend.sendNEC(HEX_TEMP[action[1]], 32 );
  }
  else if (action[0] == 2) {
    irsend.sendNEC(HEX_TIME[action[1]], 32 );
  }
  else if (action[0] == 3) {
    irsend.sendNEC(HEX_F[action[1]], 32 );
  }
}
/******************************************/
void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  /*server on*/
  /*other
    server.on("/about", []() {

    });
    server.on("/information", []() {

    });
  */
  /*function*/
  server.on("/", rootRouter);//index

  server.on("/action", []() {
    //action = method,index
    int action[] = {0,0};
    if(server.arg(0) == "A"){
      action[0] = 0;
      action[1] = server.arg(1).toInt();
    }else if(server.arg(0) == "TEMP"){
      action[0] = 1;
      action[1] = server.arg(1).toInt();
    }else if(server.arg(0) == "TIME"){
      action[0] = 2;
      action[1] = server.arg(1).toInt();
    }else if(server.arg(0) == "F"){
      action[0] = 3;
      action[1] = server.arg(1).toInt();
    }
    sendRedLaser(action);
    server.send(200, "text/plain", "Success");
  });
  /******************************************/
  server.begin();
  Serial.println("HTTP server started");
  irsend.begin();
  Serial.println( F("IR Emitter Ready...") );
  SPIFFS.begin();
  Serial.println("SPIFFS started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
