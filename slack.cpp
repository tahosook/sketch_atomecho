#include "slack.h"
#include "secret.h"
const char* SLACK_HOST = "hooks.slack.com";
const int HTTPS_PORT = 443;

void slack_setup() {
  // Wifi Setting
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int wifi_status = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    if (wifi_status == 0) {
      wifi_status = 1;
    } else {
      wifi_status = 0;
    }
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  wifi_status = 1;
}

void slack_senddata(String message) {
    // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(SLACK_HOST);
  if (!client.connect(SLACK_HOST, HTTPS_PORT)) {
    Serial.println("connection failed");
    return;
  }

  Serial.print("requesting URL: ");
  Serial.println(SLACK_WEBHOOK);

  client.print(String("POST ") + SLACK_WEBHOOK + " HTTP/1.1\r\n" +
      "Host: " + SLACK_HOST + "\r\n" +
      "User-Agent: ESP32Client\r\n" +
      "Content-Type: application/json\r\n" +
      "Content-Length: "+ String(message.length()) +"\r\n\r\n" +
      message+"\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readString();
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}