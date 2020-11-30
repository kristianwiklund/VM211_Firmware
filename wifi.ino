#include <SPI.h>
#include <WiFiEspAT.h>
#include <PubSubClient.h>


void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (client.connect("arduinoClient")) {
    Serial.println("connected");
    // Once connected, publish an announcement...
    client.publish("outTopic","hello world");
    // ... and resubscribe
    client.subscribe("inTopic");
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    // Wait 5 seconds before retrying
  }
  
}


void network_setup() {

  // Start by connecting to the ESP module
  
  Serial3.begin(115200);
  WiFi.init(Serial3);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    return;
  }
  
  // waiting for connection to Wifi network set with the SetupWiFiConnection sketch
  Serial.println("Waiting for connection to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print('.');
  }
  wifienabled = true;
  
  Serial.println();

  IPAddress ip = WiFi.localIP();
  Serial.println();
  Serial.println("Connected to WiFi network.");
  Serial.print("IP: ");
  Serial.println(ip);

  // once wifi is done, connect to mqtt
  
  client.setServer(MQTT_HOST, 1883);
  client.setCallback(mqtt_callback);

  
}
