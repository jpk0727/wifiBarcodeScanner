/*
 *  HTTP over TLS (HTTPS) example sketch
 *
 *  This example demonstrates how to use
 *  WiFiClientSecure class to access HTTPS API.
 *  We fetch and display the status of
 *  esp8266/Arduino project continuous integration
 *  build.
 *
 *  Created by Ivan Grokhotkov, 2015.
 *  This example is in public domain.
 */
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>
int dataRate = 16;
int bufferLength = 64;
int incomingByte = 0;
char buf[64] = "";

SoftwareSerial mySerial(14, 12); // RX, TX

const char* ssid = "400FERRY";
const char* password = "0987654321";

const char* host = "aerofarms.hubnest.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "CF 05 98 89 CA FF 8E D8 5E 5C E0 C2 E4 F7 E6 C3 C7 50 DD 5C";

void setup() {
  pinMode(dataRate, OUTPUT);     
  digitalWrite(dataRate, HIGH);                                                                                                                                     
  mySerial.begin(115200);
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
 

}

void loop() {
  while (mySerial.available() > 0) {
    char ch = (char)mySerial.read();
    Serial.print("ch as integer");
    Serial.println(int(ch));

    if ((int)ch == 129) {
      Serial.print("Reached end of barcode: ");
      Serial.println(buf);
      send(buf);
      mySerial.read();
      memset(buf,'\0',64);
    } else {
    
      size_t len = strlen(buf);
      Serial.print("length of buf: ");
      Serial.println(len);
    
      Serial.print("Direct read: ");
      Serial.println(ch);
    
      buf[len++] = ch;
      Serial.print("printing current state of buffer: ");
      Serial.println(buf);
    }
  }
}

void send(char *code) {
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }
  String PostData = "barcode={\"code\":1,";
  unsigned char i;

  String url = "/api/?action=farm.list&amp;token=ACCESS_TOKEN&amp;nonce=NONCE";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");

}
  

