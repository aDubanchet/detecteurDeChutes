#include <WiFi.h>
#include <HTTPClient.h>

void sendFallAlert() {
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected");

    HTTPClient http;
    http.begin("http://your_elasticsearch_server:9200/fall_detection/_doc/");
    http.addHeader("Content-Type", "application/json");

    String jsonMessage = "{\"user\":\"user123\", \"event\":\"fall\", \"timestamp\":\"" + String(millis()) + "\"}";
    
    int httpResponseCode = http.POST(jsonMessage);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.println("Error on sending POST");
    }

    http.end();
  }
}