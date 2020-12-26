// draw a clock on the screen - requires that a real time clock chip is present. 
//
// (C) 2020 Kristian Wiklund
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
//

// do not add this code unless with have an ESP01 connected...
#ifdef WITH_ESP01

#include <SPI.h>
#include <WiFiEspAT.h>
#include <PubSubClient.h>
#include <NTPClient.h>

// ntp client code

WiFiUDP ntpUDP;
// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, NTP_HOST, 3600, 60000);


// calling this reboots the arduino
void(* resetFunc) (void) = 0;

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // remove this - it is not good. needed for testing though...
  if(!strncmp("reboot", payload, 6)) {
    Serial.println("Attempting restart");
    resetFunc();
  }
    
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (client.connect("arduinoClient")) {
    Serial.println("connected");
    // Once connected, publish an announcement...
    client.publish("vm211/status","online");
    // ... and resubscribe
    client.subscribe("inTopic");
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    Serial.println(" try again in 5 seconds");
    // Wait 5 seconds before retrying
  }
  
}

// convenience function
char *f2mqtt(char *t, float v) {
  char s[50];

    if (wifienabled && client.connected()) {
      dtostrf(v,3,1,s);
      client.publish(t,s);
    }

}

char *i2mqtt(char *t, int v) {
  char s[50];

    if (wifienabled && client.connected()) {
      client.publish(t,itoa(v,s,10));
    }

}




void network_setup() {

  // Start by connecting to the ESP module
  
  Serial3.begin(115200);
  WiFi.init(Serial3);

  bootscreen.cycle();
  
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
    bootscreen.cycle();
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

  // and connect to mqtt

  mqtt_reconnect();

  // finally, set up NTP

  timeClient.begin();
  timeClient.update();
  
  Serial.print("NTP time is ");
  Serial.println(timeClient.getFormattedTime());
}


// endif WITH_ESP01
#endif





