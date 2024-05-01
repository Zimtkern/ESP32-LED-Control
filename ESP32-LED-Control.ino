#include <WiFi.h>

const char* ssid = "ssid";
const char* password = "key";
const char* hostname = "esp32-led-01"; // Hostname festlegen
const int ledPin1 = 23; // Pin für LED1(PWM)
const int ledPin2 = 22; // Pin für LED2(PWM)
WiFiServer server(80);
int brightness1 = 50; // Standardhelligkeit für LED1 auf 50% setzen
int brightness2 = 50; // Standardhelligkeit für LED2 auf 50% setzen
int lastBrightness1 = 50; // Speichert den letzten eingestellten Dimmerwert für LED1
int lastBrightness2 = 50; // Speichert den letzten eingestellten Dimmerwert für LED2

void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Verbindung zum WiFi herstellen...");
  }

  server.begin();
  Serial.println("Server gestartet!");

  // Feste IP-Adresse zuweisen
  IPAddress ip(192, 168, 1, 99); 
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(ip, gateway, subnet);

  Serial.print("IP-Adresse des ESP32: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  while (!client.available()) {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  client.flush();

  if (request.indexOf("/led1/on") != -1) {
    digitalWrite(ledPin1, HIGH);
    brightness1 = lastBrightness1; // Verwende den letzten eingestellten Dimmerwert beim Einschalten des Schalters
    analogWrite(ledPin1, map(brightness1, 0, 100, 0, 255)); // Setze PWM-Helligkeit entsprechend
    client.println("LED1 turned on");
  } else if (request.indexOf("/led1/off") != -1) {
    digitalWrite(ledPin1, LOW);
    brightness1 = 0; // Setze Helligkeit auf 0%
    analogWrite(ledPin1, 0); // Sicherstellen, dass der LED-Streifen ausgeschaltet ist
    client.println("LED turned off");
  } else if (request.indexOf("/led2/on") != -1) {
    digitalWrite(ledPin2, HIGH);
    brightness2 = lastBrightness2; // Verwende den letzten eingestellten Dimmerwert beim Einschalten des Schalters
    analogWrite(ledPin2, map(brightness2, 0, 100, 0, 255)); // Setze PWM-Helligkeit entsprechend
    client.println("LED2 turned on");
  } else if (request.indexOf("/led2/off") != -1) {
    digitalWrite(ledPin2, LOW);
    brightness2 = 0; // Setze Helligkeit auf 0%
    analogWrite(ledPin2, 0); // Sicherstellen, dass der LED-Streifen ausgeschaltet ist
    client.println("LED2 turned off");
  } else {
    int dimmerIndex = request.indexOf("/dimmer1/");
    if (dimmerIndex != -1) {
      int brightnessStart = dimmerIndex + 9; // Position des Helligkeitswerts in der URL
      String brightnessValueStr = request.substring(brightnessStart); // Extrahiere den Teil der URL, der den Helligkeitswert enthält
      brightness1 = brightnessValueStr.toInt(); // Konvertiere den Helligkeitswert von String zu Integer
      brightness1 = constrain(brightness1, 0, 100); // Begrenze Helligkeit auf Wertebereich von 0 bis 100
      lastBrightness1 = brightness1; // Speichere den letzten eingestellten Dimmerwert
      analogWrite(ledPin1, map(brightness1, 0, 100, 0, 255)); // Setze PWM-Helligkeit entsprechend
      client.print("LED1 brightness set to " + String(brightness1));
    } else {
      int dimmerIndex2 = request.indexOf("/dimmer2/");
      if (dimmerIndex2 != -1) {
        int brightnessStart2 = dimmerIndex2 + 9; // Position des Helligkeitswerts in der URL
        String brightnessValueStr2 = request.substring(brightnessStart2); // Extrahiere den Teil der URL, der den Helligkeitswert enthält
        brightness2 = brightnessValueStr2.toInt(); // Konvertiere den Helligkeitswert von String zu Integer
        brightness2 = constrain(brightness2, 0, 100); // Begrenze Helligkeit auf Wertebereich von 0 bis 100
        lastBrightness2 = brightness2; // Speichere den letzten eingestellten Dimmerwert
        analogWrite(ledPin2, map(brightness2, 0, 100, 0, 255)); // Setze PWM-Helligkeit entsprechend
		client.print("LED2 brightness set to " + String(brightness2));
      } else {
        client.println("<h1>ESP32 LED Control</h1>");
        client.println("<div style='margin-bottom: 20px;'>");
        client.println("<h2>LED1</h2>");
        client.println("<p><a href='http://" + WiFi.localIP().toString() + "/led1/on'>http://" + WiFi.localIP().toString() + "/led1/on</a> - turn on</p>");
        client.println("<p><a href='http://" + WiFi.localIP().toString() + "/led1/off'>http://" + WiFi.localIP().toString() + "/led1/off</a> - turn off</p>");
        client.println("<p><a href='http://" + WiFi.localIP().toString() + "/dimmer1/value'>http://" + WiFi.localIP().toString() + "/dimmer1/value</a> - brightness level (0-100%)</p>");
        client.println("<p>Pin: " + String(ledPin1) + "</p>");
        client.println("<p>Current brightness: " + String(brightness1) + "</p>");
        client.println("</div><br>");
        client.println("<div style='margin-bottom: 20px;'>");
        client.println("<h2>LED2</h2>");
        client.println("<p><a href='http://" + WiFi.localIP().toString() + "/led2/on'>http://" + WiFi.localIP().toString() + "/led2/on</a> - turn on</p>");
        client.println("<p><a href='http://" + WiFi.localIP().toString() + "/led2/off'>http://" + WiFi.localIP().toString() + "/led2/off</a> - turn off</p>");
        client.println("<p><a href='http://" + WiFi.localIP().toString() + "/dimmer2/value'>http://" + WiFi.localIP().toString() + "/dimmer2/value</a> - brightness level (0-100%)</p>");
        client.println("<p>Pin: " + String(ledPin2) + "</p>");
        client.println("<p>Current brightness: " + String(brightness2) + "</p>");
        client.println("</div>");
      }
    }
  }

  delay(1);
  client.stop();
}

