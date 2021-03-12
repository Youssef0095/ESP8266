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
char *conn = "HostName=IOTHUB20-20.azure-devices.net;DeviceId=84:CC:A8:85:C2:BE;SharedAccessKey=XVGjiUP1iKFO4QShkdc96FszJeneztCgs5tGI8FNmks=";

bool messagePending = false;
int interval = 1000 * 6;
unsigned long prevMillis = 0;
time_t epochTime;

DHT dht(DHT_PIN, DHT_TYPE);
IOTHUB_CLIENT_LL_HANDLE deviceClient;

void setup() {
  initSerial();
  initWifi();
  initEpochTime();
  Serial.println("\n");
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
      temperature = dht.readTemperature();
      humidity = dht.readHumidity();

      if (!(std::isnan(temperature)) && !(std::isnan(humidity)) && epochTime > 28800)  {
        
        char payload[256];
        char epochTimeBuf[12];

        StaticJsonBuffer<sizeof(payload)> buf;
        JsonObject &root = buf.createObject();
        root["temperature"] = temperature;
        root["humidity"] = humidity;
        root.printTo(payload, sizeof(payload));

        if (temperature > (PrevTemp + 1.0) || temperature < (PrevTemp - 1.0)) {
          sendMessage(payload, itoa(epochTime, epochTimeBuf, 10));
          Serial.println("Message Sent...");
          PrevTemp = temperature;
        }
      }
    }
  }
  IoTHubClient_LL_DoWork(deviceClient);
  delay(1000);
}
