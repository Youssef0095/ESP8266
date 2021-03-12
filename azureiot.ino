void initDevice() {
  deviceClient = IoTHubClient_LL_CreateFromConnectionString(conn, MQTT_Protocol);
}

void sendCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback) {
  if(IOTHUB_CLIENT_CONFIRMATION_OK == result) {
    Serial.println("Sending message to Azure IoT Hub - succeeded.");
  }
  
  messagePending = false;
}

void sendMessage(char *payload, char *epochTime) {
  IOTHUB_MESSAGE_HANDLE message = IoTHubMessage_CreateFromByteArray((const unsigned char *) payload, strlen(payload));

  MAP_HANDLE properties = IoTHubMessage_Properties(message);
  Map_Add(properties, "deviceType", "dht");
  Map_Add(properties, "latitude", "59.2465954");
  Map_Add(properties, "longitude", "18.0609175");
  Map_Add(properties, "epochTime", epochTime);
  
  if(IoTHubClient_LL_SendEventAsync(deviceClient, message, sendCallback, NULL) == IOTHUB_CLIENT_OK) {
    messagePending = true;
  }

  IoTHubMessage_Destroy(message);
}
