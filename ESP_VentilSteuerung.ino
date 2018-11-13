// Version ohne OTA
#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include <ArduinoOTA.h>

ESP8266WebServer server(80);

const char* ssid = "0024A5C6D897";
const char* password = "u5rr1xembpu1c";

bool LED_An = false;

void handleRoot() {
  char temp[600];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  Serial.print("HandleRoot");
  int _sensorWert = 0;
  Serial.print(" Sensor: ");
  Serial.print(_sensorWert);
  //<p>Wert: %d</p>\n\  analogRead(0),
  snprintf(temp, 600,
           "<html><head><meta http-equiv='refresh' content='5'/><title>ESP-01S</title><style>body{background-color: #cccccc; Color: #000088; }</style></head>\
<body><h1>ESP-01S</h1><p>Uptime: %02d:%02d:%02d</p>\n\
<p>Sensor: %d</p>\n\
<form action='/Schalte' method='POST'>LED %s<input type='submit' name='Schalten' value='%s'></form>\
</html>",
           hr, min % 60, sec % 60, _sensorWert, !LED_An ? "an" : "aus", LED_An ? "an" : "aus"
          );
  Serial.print(" Msg fertig");
  server.send(200, "text/html", temp);
  Serial.println(" und gesendet");
}

void handleSchalte() {
  if (server.args() == 1) {
//    digitalWrite(LED_BUILTIN, LED_An ? LOW : HIGH); // Turn the LED on (Note that LOW is the voltage level
//    LED_An = !LED_An;
    server.sendHeader("Location", "/");
    server.send(303, "text/html", "Location: /");
  } else {
    String message = "Fehler in Anzahl Args\n\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(400, "text/plain", message);
  }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void setup() {
//  pinMode(LED_BUILTIN, OUTPUT);
//  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  Serial.println("Booting");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.printf("Ready\n\nConnected to %s\nIP Address %s\n", ssid, WiFi.localIP().toString().c_str());

  if (MDNS.begin("esp01s")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/Schalte", handleSchalte);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
 
//  digitalWrite(LED_BUILTIN, LOW);

//  ArduinoOTA.begin();
//  Serial.println("OTA started");
}

void loop() {
//  ArduinoOTA.handle();
  server.handleClient();
}

