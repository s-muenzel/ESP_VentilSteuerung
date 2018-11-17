  // Version ohne OTA
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <ESP8266HTTPClient.h>

ESP8266WebServer server(80);

const char* ssid = "0024A5C6D897";
const char* password = "u5rr1xembpu1c";

bool LED_An = false;

void handleRoot() {
  char temp[600];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  Serial.print("HandleRoot: ");
  String _sensorWert;
 
  HTTPClient http;

  Serial.print("[HTTP] begin...");
  // configure traged server and url
  http.begin("http://192.168.2.141/Sensor"); //HTTP

  Serial.print(" GET...");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf(" code: %d ", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      _sensorWert = http.getString();
    }
  } else {
    Serial.printf(" failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
 
  Serial.print(" Sensor: ");
  Serial.print(_sensorWert);
  snprintf(temp, 600,
           "<html><head><meta http-equiv='refresh' content='5'/><title>ESP-01S</title><style>body{background-color: #cccccc; Color: #000088; }</style></head>\
<body><h1>ESP-01S</h1><p>Uptime: %02d:%02d:%02d</p>\n\
<p>Sensor: %s</p>\n\
</html>",
           hr, min % 60, sec % 60, _sensorWert.c_str());
  Serial.print(" Msg fertig");
  server.send(200, "text/html", temp);
  Serial.println(" und gesendet");
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
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  server.handleClient();
}

