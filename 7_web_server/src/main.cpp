#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#define WIFI_SSID "CHESUS"
#define WIFI_PSW "hahahaha"

#define NTP_SERVER "pool.ntp.org"

// WiFi server
bool isServerWifi = false;
void serverWifi_start();

// Time
time_t timeOffset = 0;
String getLocalTime();
void setLocalTime();

// Web server
AsyncWebServer server(80);
String processor(const String &);

void setup()
{
  // Serial
  Serial.begin(9600);

  // WiFi
  serverWifi_start();
  if (!isServerWifi)
  {
    Serial.println("No pudo conectarse WIFI");
    return;
  }
  Serial.print("Conectado IP ");
  Serial.println(WiFi.localIP());

  // Tiempo
  configTime(3600, 3600, NTP_SERVER);
  delay(5000);
  Serial.println(getLocalTime());

  // SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/style.css", "text/css"); });

  server.on("/RST", HTTP_GET, [](AsyncWebServerRequest *request)
            { setLocalTime();
              request->send(SPIFFS, "/index.html", String(), false, processor); });
  server.on("/RL", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false, processor); });

  server.begin();
}

void loop()
{
}

String processor(const String &var)
{
  Serial.println(var);
  if (var == "TIME_HOUR")
  {
    return getLocalTime();
  }
  return String();
}

void serverWifi_start()
{
  Serial.println("Conectando");
  WiFi.begin(WIFI_SSID, WIFI_PSW);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECT_FAILED)
    {
      break;
    }
  }

  isServerWifi = (WiFi.status() == WL_CONNECTED);
  Serial.println("");
}

String getLocalTime()
{
  struct tm t;
  if (!getLocalTime(&t))
  {
    Serial.println("ERROR tiempo");
    return "";
  }
  time_t currentTime = mktime(&t);
  currentTime -= timeOffset;

  String s = ctime(&currentTime);
  return s;
}

void setLocalTime()
{
  struct tm t;
  getLocalTime(&t);
  timeOffset = mktime(&t);
}