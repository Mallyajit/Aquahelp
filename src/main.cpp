#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "CHANDRA AUTO first floor";
const char* password = "CHANDRA@777777";

// Start server on port 80
WebServer server(80);

// Define GPIO pins for RGB channels
const int RED_PIN = LED_BUILTIN;    // Adjust as needed
const int GREEN_PIN = 12;  // Adjust as needed
const int BLUE_PIN = 13;   // Adjust as needed

void handleSetColor() {
  Serial.println("🔵 Received POST request");

  server.sendHeader("Access-Control-Allow-Origin", "*");

  if (server.hasArg("plain") == false) {
    Serial.println("❌ No body found");
    server.send(400, "text/plain", "Body not received");
    return;
  }

  String body = server.arg("plain");
  Serial.println("📦 Body: " + body);

  // Extract red value
  int rIndex = body.indexOf("\"r\":");
  int gIndex = body.indexOf("\"g\":");
  int bIndex = body.indexOf("\"b\":");

  if (rIndex == -1 || gIndex == -1 || bIndex == -1) {
    Serial.println("❌ Missing r, g, or b values");
    server.send(400, "text/plain", "Missing color values");
    return;
  }

  int rComma = body.indexOf(",", rIndex);
  int gComma = body.indexOf(",", gIndex);
  int bEnd = body.indexOf("}", bIndex);

  String rVal = body.substring(rIndex + 4, rComma);
  String gVal = body.substring(gIndex + 4, gComma);
  String bVal = body.substring(bIndex + 4, bEnd);

  int red = rVal.toInt();
  int green = gVal.toInt();
  int blue = bVal.toInt();

  Serial.printf("🔴 Red: %d | 🟢 Green: %d | 🔵 Blue: %d\n", red, green, blue);

  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

server.onNotFound([](){
  // This handler catches any request not explicitly handled by server.on(...)
  // Useful for debugging if a route isn't caught, or for serving 404s
  // BUT also for handling OPTIONS preflight requests in some cases.
  if (server.method() == HTTP_OPTIONS) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS"); // Specify allowed methods
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type"); // Specify allowed headers
    server.send(204); // No content
  } else {
    // If it's not an OPTIONS request, and no other handler was found,
    // this would be a true 404. You can send a 404 page or message.
    server.send(404, "text/plain", "Not Found");
  }
});

  // Setup pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  Serial.println("🔌 Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Connected to Wi-Fi!");
  Serial.print("🌐 ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/setcolor", HTTP_POST, handleSetColor);
  server.begin();
  Serial.println("✅ HTTP server started");
}

void loop() {
  server.handleClient();
}
