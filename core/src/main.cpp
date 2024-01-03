#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiType.h>

#define RED D1
#define GREEN D2
#define BLUE D3

const char *ssid = "busya";
const char *password = "0677170801";

ESP8266WebServer server(80);

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>RGB LED Control</h1>";
  html += "<form action='/setColor' method='GET'>";
  html += "Red: <input type='text' name='red' value='0'><br>";
  html += "Green: <input type='text' name='green' value='0'><br>";
  html += "Blue: <input type='text' name='blue' value='0'><br>";
  html += "<input type='submit' value='Set Color'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

void handleSetColor() {
  int red = server.arg("red").toInt();
  int green = server.arg("green").toInt();
  int blue = server.arg("blue").toInt();

  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);

  server.send(200, "text/html", "Color Set");
}

void setup() {
  Serial.begin(115200);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  analogWrite(RED, 0);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.on("/", HTTP_GET, handleRoot);
  server.on("/setColor", HTTP_GET, handleSetColor);

  server.begin();
  Serial.println("HTTP server started");
  Serial.print("IP addr: ");
  Serial.println(WiFi.localIP().toString());
}

void loop() { server.handleClient(); }