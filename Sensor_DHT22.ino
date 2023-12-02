#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>

// pinos
const int DHTPIN = 25; // pino conectado no sensor
const int  DHTTYPE = DHT22; // tipo do sensor

//wifi
//const char* WIFI_SSID = "ALEX_2Ghz"; // rede
//const char* WIFI_PASSWORD = ""; // senha
//const char* WIFI_SSID = "ws_wlan";
//const char* WIFI_PASSWORD = "";
//const char* WIFI_SSID = "FILIPE";
//const char* WIFI_PASSWORD = "";

//mqtt
const char* MQTT_SERVER = ""; // url
const int MQTT_PORT = 8883; // porta com segurança
const char* MQTT_USERNAME = ""; 
const char* MQTT_PASSWORD = "";
const char* MQTT_TOPIC = "Temperatura e Umidade"; // topico onde será publicado
// certificado necessário
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

//ntp
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -10800; // fuso do Brasil (-3*3600)

DHT dht(DHTPIN, DHTTYPE);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec);
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup() 
{
  Serial.begin(115200);
  dht.begin();
  connectWifi();
  espClient.setCACert(root_ca);
  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop() 
{
  delay(60000);  // 1 min entre leituras

  timeClient.update();

  //leituras
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!client.connected()) {
    connectMQTT();
  }

  // publicação da mensagem
  Serial.println("Publicando mensagem...");
  String mensagem = "[" + data_hora() + " " + String(temp) + "°C " + String(humidity) + "%]";
  client.publish(MQTT_TOPIC, mensagem.c_str());
}

// conectando ao wifi
void connectWifi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a rede...");
  }

  Serial.println("Conectado a rede");
}

// conectando ao mqtt
void connectMQTT() 
{
  while (!client.connected()) {
    Serial.println("Conectando ao MQTT...");
    if (client.connect("ESP32Client", MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Conectado ao MQTT");
    } else {
      Serial.print("Erro, rc=");
      Serial.print(client.state());
      Serial.println("Tentando novamente");
      delay(5000);
    }
  }
}

// data e hora no formato
String data_hora() 
{
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  time_t timeConverted = static_cast<time_t>(epochTime);
  struct tm *timeinfo;
  timeinfo = gmtime(&timeConverted);

  char buffer[20];
  snprintf(buffer, sizeof(buffer), "%02d/%02d/%02d %02d:%02d:%02d",
           timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year % 100,
           timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

  return String(buffer);
}