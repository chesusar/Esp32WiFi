#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID "CHESUS"
#define WIFI_PSW "hahahaha"

#define NTP_SERVER "pool.ntp.org"

// WiFi server
bool isServerWifi = false;
void serverWifi_start();

// Time
long int timeOffset = 0;
String getLocalTime();

// Server creado
IPAddress server(192, 168, 137, 36);
const int SERVER_PORT = 4321;

// Cliente
bool clientEnviando = false;
WiFiClient client;

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

  // Server
  if (client.connect(server, SERVER_PORT))
  {
    Serial.println("Conectado a servidor");
  }
}

void loop()
{
  delay(250);

  if (!isServerWifi)
    return;

  if (!client.connected())
  {
    client.stop();
    Serial.println("No conectado a servidor");
    client.connect(server, SERVER_PORT); // Intentar reconectar
    delay(750);
    return;
  }

  // Envio del cliente
  if (clientEnviando)
  {
    client.println(getLocalTime());
    delay(750);
  }

  // Lectura del server
  int serverBytes = client.available();
  String serverData = "";

  if (serverBytes <= 0)
    return;

  for (int i = 0; i < serverBytes; i++)
  {
    serverData.concat((char)client.read());
  }
  client.flush();
  serverData.trim();
  Serial.print("Server data: ");
  Serial.println(serverData);

  // Estado
  if (serverData.equals("start"))
  {
    clientEnviando = true;
    Serial.println("Enviando a servidor");
  }
  else if (serverData.equals("stop"))
  {
    clientEnviando = false;
    Serial.println("Fin de envio a servidor");
  }
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

  char buff[100];
  // sprintf(buff, "%d/%d/%d %d:%d:%d", 1900 + t.tm_year, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
  sprintf(buff, "%d:%d:%d", t.tm_hour, t.tm_min, t.tm_sec);

  String s = buff;
  return s;
}