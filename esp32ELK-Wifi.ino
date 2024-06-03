#include <WiFi.h>
#include <HTTPClient.h>


const char* ssid = "iPhone de cosmik";
const char* password = "marielabest";
const char* serverUrl = "https://27fc5c467ef5498393a0afad9e8bced3.us-gov-east-1.aws.elastic-cloud.com:443/_bulk?pretty";

// Structure pour stocker les informations des patients
struct PatientInfo {
  const char* patient_id;
  const char* location;
};

// Exemples d'infos patients 
PatientInfo patients[] = {
  {"12345", "chambre1"},
  {"12346", "chambre2"},
  {"12347", "chambre3"},
  {"12348", "chambre4"},
  {"12349", "chambre5"}
};

// connexion au WiFi (partage de connexion ici)
void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion à ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connecté au WiFi");
  Serial.println("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

void sendRequestToELK(){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(serverUrl);  // Spécifiez l'URL du serveur
    http.addHeader("Content-Type", "application/json");  // Ajoutez les en-têtes nécessaires
    http.addHeader("Authorization", "ApiKey YWZrLTNZOEJaVDJxSVhaNUdPVWI6RTZKV01sQlVSM3lFVXdrWXlqbnNWdw==");

    // Construire la requête bulk
    String bulkData = "";
    for (int i = 0; i < sizeof(patients) / sizeof(patients[0]); i++) {
      bulkData += "{ \"index\" : { \"_index\" : \"falls\", \"pipeline\": \"add_timestamp\" } }\n";
      bulkData += "{ \"patient_id\": \"" + String(patients[i].patient_id) + "\", \"location\": \"" + String(patients[i].location) + "\" }\n";
    }

    int httpResponseCode = http.POST(bulkData);

    if (httpResponseCode > 0) {
      String response = http.getString();  // Si la réponse contient des données, vous pouvez les obtenir
      Serial.println(httpResponseCode);  // Code de réponse HTTP
      Serial.println(response);  // Réponse du serveur
    } else {
      Serial.print("Erreur dans l'envoi de la requête POST : ");
      Serial.println(httpResponseCode);
    }

    http.end();  // Libérer les ressources
  } else {
    Serial.println("Erreur de connexion WiFi");
  }
}

void setup() {
  Serial.begin(19200);
  setupWiFi();
}

void loop() {
  sendRequestToELK();
  delay(10000);  // Attendre 10 secondes avant d'envoyer une autre requête
}
