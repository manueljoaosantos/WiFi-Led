#include <Arduino.h>
#include <Ticker.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "credentials.h" // Network name, password, and private API key

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

ESP8266WebServer server(80);

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int LDR = A0;
const int BUTTON = 4;
const int RED = D7;
const int GREEN = D5;
const int BLUE = D6;

void initWiFi(void);
void initWebServer(void);
void handleRoot(void);
void initDisplay(void);
void testscrolltext(void);
void displayMessage(String _message);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(LDR, INPUT);
  pinMode(BUTTON, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  delay(1000);
  Serial.begin(115200);

  initDisplay();

  initWiFi();
  initWebServer();
  server.begin();

  server.on("/", handleRoot);
  digitalWrite(LED_BUILTIN, HIGH);

  //  ticker.attach(TICKER_PERIOD, tickerHandler);
}

void loop()
{
  server.handleClient();
}

void initWiFi()
{
  Serial.println("Initialize WiFi...");

  if (!WiFi.begin(ssid, password))
  // if (!WiFi.begin(WIFI_SSID, WIFI_PWD))
  {
    Serial.println("ERROR: WiFi.begin");
    return;
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("OK: WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  //  displayMessage("IP: ");
  // delay(1000);
  displayMessage("IP: " + WiFi.localIP().toString());
}

void initWebServer()
{
  Serial.println("Initialize SPIFFS...");

  if (!SPIFFS.begin())
  {
    Serial.println("ERROR: SPIFFS.begin");
    return;
  }
  Serial.println("Initialize Web Server...");
}

void handleRoot()
{
  if (server.hasArg("redon"))
  {
    digitalWrite(RED, HIGH);
    Serial.printf("Red - ON");
    displayMessage("Red - ON");
  }
  else if (server.hasArg("redoff"))
  {
    digitalWrite(RED, LOW);
    Serial.printf("Red - OFF");
    displayMessage("Red - OFF");
  }
  else if (server.hasArg("greenon"))
  {
    Serial.printf("Green - ON");
    digitalWrite(GREEN, HIGH);
    // digitalWrite(LED_BUILTIN, HIGH);
    displayMessage("Green - ON");
  }
  else if (server.hasArg("greenoff"))
  {
    Serial.printf("Green - OFF");
    digitalWrite(GREEN, LOW);
    // digitalWrite(LED_BUILTIN, LOW);
    displayMessage("Green - OFF");
  }
  else if (server.hasArg("blueon"))
  {
    digitalWrite(BLUE, HIGH);
    Serial.printf("Blue - ON");
    displayMessage("Blue - ON");
  }
  else if (server.hasArg("blueoff"))
  {
    digitalWrite(BLUE, LOW);
    Serial.printf("Blue - OFF");
    displayMessage("Blue - OFF");
  }
  else if (server.hasArg("off"))
  {
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(RED, LOW);
    displayMessage("All of");
  }
  else if (server.hasArg("on"))
  {
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, HIGH);
    displayMessage("All on");
  }

  int sensorValue = analogRead(LDR);
  char lightSensorValue[30];
  dtostrf(sensorValue, 4, 3, lightSensorValue);

  int buttonValue = digitalRead(BUTTON);
  char buttonSensorValue[30];
  dtostrf(buttonValue, 4, 3, buttonSensorValue);

  char request[1024];
  strcpy(request, "<html><head>");
  strcat(request, "<title>ESP8266 WittyCloud ESP12 Board</title>");
  strcat(request, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/>");
  strcat(request, "</head><body><ul>");
  strcat(request, "<li>Light sensor: ");
  strcat(request, lightSensorValue);
  strcat(request, "</li>");
  strcat(request, "<li>Button: ");
  strcat(request, buttonSensorValue);
  strcat(request, "</li><hr />");
  strcat(request, "<li>All: <a href=\"?off=1\">Off</a></li>");
  strcat(request, "<li>All: <a href=\"?on=1\">On</a></li>");
  strcat(request, "<li>White: <a href=\"?redon=1\">On</a> <a href=\"?redoff=1\">Off</a></li>");
  strcat(request, "<li>Green: <a href=\"?greenon=1\">On</a> <a href=\"?greenoff=1\">Off</a></li>");
  strcat(request, "<li>Blue: <a href=\"?blueon=1\">On</a> <a href=\"?blueoff=1\">Off</a></li>");
  strcat(request, "</body></html>");
  server.send(200, "text/html", request);
}
void initDisplay()
{
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);

  // Invert and restore display, pausing in-between
  // display.invertDisplay(true);
}

void testscrolltext()
{
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display(); // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

void displayMessage(String _message)
{
  display.clearDisplay();

  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(_message);
  delay(100);
  display.display();
}
