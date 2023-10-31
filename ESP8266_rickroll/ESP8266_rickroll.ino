#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <FS.h>  // Include the SPIFFS library

const char* SSID_NAME = "Free WiFi"; //Here you can change SSID name 
const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(172, 0, 0, 1);

String currentSSID = "";

DNSServer dnsServer;
ESP8266WebServer webServer(80);

void setup() {
  Serial.begin(115200);

  currentSSID = SSID_NAME;
  Serial.print("Current SSID: ");
  Serial.println(currentSSID);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(currentSSID);

  // Initialize the SPIFFS file system
  SPIFFS.begin();

  // Start webserver
  dnsServer.start(DNS_PORT, "*", APIP);

  // Serve the GIF file from SPIFFS
  webServer.on("/rickroll.gif", HTTP_GET, [](void) {
    File file = SPIFFS.open("/rickroll.gif", "r");
    if (file) {
      webServer.streamFile(file, "image/gif");
      file.close();
    } else {
      webServer.send(404, "text/plain", "File not found");
    }
  });

  //Webserver HTML + CSS
  webServer.onNotFound([]() {
    String content = "<!DOCTYPE html><html>"
                    "<head><title>Captive Portal</title>"
                    "<style>"
                    "  body {"
                    "    display: flex;"
                    "    justify-content: center;"
                    "    align-items: center;"
                    "    height: 100vh;"
                    "    flex-direction: column;"
                    "  }"
                    "</style>"
                    "</head>"
                    "<body><h1>Rick Roll</h1>"
                    "<img src='/rickroll.gif' alt='Rick Roll'></body></html>"
                    "<p>Never gonna give you up</p>"
                    "<p>Never gonna let you down</p>"
                    "<p>Never gonna run around, and desert you</p>"
                    "<p>Never gonna make you cry</p>"
                    "<p>Never gonna say goodbye</p>"
                    "<p>never gonna tell a lie, and hurt you</p>"
                    "<p>Never gonna give you up</p>"
                    "<p>Never gonna let you down</p>"
                    "<p>Never gonna run around, and desert you</p>"
                    "<p>Never gonna make you cry</p>"
                    "<p>Never gonna say goodbye</p>"
                    "<p>never gonna tell a lie, and hurt you</p>";


                    
    webServer.send(HTTP_CODE, "text/html", content);
  });

  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
