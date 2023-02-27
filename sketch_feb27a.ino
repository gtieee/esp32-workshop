#include "WiFi.h"

#define LED 2

void sendHTML(WiFiClient* client);
int decodeRequest(String request);
void buttonOn(WiFiClient* client);

const char* ssid = "myesp32";
const char* password = "aaronpass";

WiFiServer server(80);

String header;
int ledState = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  digitalWrite(LED, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String currLine = "";
    String req = "";
    while (client.connected()) {
      if (client.available()) {
          char c = client.read();
          Serial.write(c);
          req += c;

        if (c == '\n') {
          if (currLine.length() != 0) {
            currLine = "";
          }
          else {
            int decode = decodeRequest(req);
            if (decode == 1) {
              sendHTML(&client);
            } else if (decode == 2) {
              buttonOn(&client);
            }
            break;
          }
        }
        else if (c != '\r'){
          currLine += c;
        }
      }
    }
    client.stop();
  }
}

int decodeRequest(String request) {
  if (request.substring(0, 3) == "GET") {
    return 1;
  } else if (request.substring(0,4) == "POST") {
      Serial.println(request.substring(5, 9));
      if (request.substring(5,8) == "/on") {
        return 2;
      } else if (request.substring(5,8) == "\off") {
        return 3;
      }
  }
}

void sendHTML(WiFiClient* client) {
    client->println("HTTP/1.1 200 OK");
    client->println("Content-type:text/html");
    client->println("Connection: close");
    client->println();

    // Display the HTML web page
    client->println("<!DOCTYPE html><html>");
    client->println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client->println("<link rel=\"icon\" href=\"data:,\">");
    // CSS to style the on/off buttons 
    // Feel free to change the background-color and font-size attributes to fit your preferences
    client->println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client->println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
    client->println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
    client->println(".button2 {background-color: #555555;}</style></head>");
    
    // Web Page Heading
    client->println("<body><h1>ESP32 Web Server</h1>");

    // Button
    client->println("<form action=\"/on\" method=\"POST\">");
    client->println("<input type=\"submit\" value=\"Turn LED On\"></form></body></html>");
    client->println();
}

void buttonOn(WiFiClient* client) {
    digitalWrite(LED, HIGH);
    client->println("HTTP/1.1 200 OK");
    client->println("Content-type: application/json");
    client->println("Connection: close");
    client->println();
    client->println("{success: true}");
    client->println();
}