#include <ESP8266WiFi.h>   //#include <ESP8266WiFi.h>
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTUtility.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHT_PIN 14
#define DHT_TYPE DHT11

char *ssid = "dlink-95DB";
char *pass = "gwxyj36867";
char *conn = "HostName=IOTHUB20-20.azure-devices.net;DeviceId=Esp8266;SharedAccessKey=BS4aYncxIIfEWvihwr5fiRdoMZwlrulpUAazgEVsJCA=";

bool messagePending = false;
int interval = 1000;
unsigned long prevMillis = 0;
time_t epochTime;

DHT dht(DHT_PIN, DHT_TYPE);
IOTHUB_CLIENT_LL_HANDLE deviceClient;

void setup() {
  initSerial();
  initWifi();
  initEpochTime();
  initDHT();
  initDevice();
}

float PrevTemp;
float temperature;
float humidity;

void loop() {
  unsigned long currentMillis = millis();


  if (!messagePending) {
    if ((currentMillis - prevMillis) >= interval) {
      prevMillis = currentMillis;

      epochTime = time(NULL);
      Serial.printf("Current Time: %lu. ", epochTime);
      Serial.println("\n");

      temperature = dht.readTemperature();
      humidity = dht.readHumidity();

      if (!(std::isnan(temperature)) && !(std::isnan(humidity)))  {
        char payload[256];

        StaticJsonBuffer<sizeof(payload)> buf;
        JsonObject &root = buf.createObject();
        root["type"] = "DHT";
        root["deviceId"] = "ESP8266";
        //root["ts"] = epochTime;
        root["temperature"] = temperature;
        root["humidity"] = humidity;
        root.printTo(payload, sizeof(payload));

        if (temperature > (PrevTemp + 1.0) || temperature < (PrevTemp - 1.0)) {
          sendMessage(payload);
          Serial.println("Message Sent...");
          PrevTemp = temperature;
        }
      }
    }
  }
  IoTHubClient_LL_DoWork(deviceClient);
  delay(1000);
}
