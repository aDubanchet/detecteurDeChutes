#include <NimBLEDevice.h>

// Les différents modes possibles
enum Menu {
  ROOM,
  EXT,
  PARAM
};

// Stocke l'état actuel
// au démarrage l'esp32 est toujours en mode PARAM
// il faut changer de mode pour tester l'app
Menu currentMode = PARAM;

// Bouton simulant l'accéléromètre
const int buttonPin = 5;


// Config BLE
NimBLEServer* pServer = nullptr;
NimBLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

// UUIDs pour le service et la caractéristique
// c'est plus sécurisé de les définir dans le programme avant compilation ? 
#define SERVICE_UUID        "0000180F-0000-1000-8000-00805F9B34FB"
#define CHARACTERISTIC_UUID "00002A19-0000-1000-8000-00805F9B34FB"

// Variables pour stocker les informations
int fallCount = 0;
String networkConfig = "";
String connectionStatus = "";

// Fonction pour envoyer une alerte de chute via HTTP
void sendFallAlert() {
  Serial.println("Chute détectée ! Alerte envoyée.");
  fallCount++;
}

// ----------------------------------------
// PARTIES Bluetooth low energy 
// ----------------------------------------

void initBLE(){
  Serial.println("----------------------------");
  Serial.println("Initialisation BLE .. ");
  // Initialisation du périphérique BLE
  NimBLEDevice::init("ESP32-C3");
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ConnexionServerCallbacks());

  // Configuration du service et de la caractéristique BLE
  NimBLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      NIMBLE_PROPERTY::READ |
                      NIMBLE_PROPERTY::WRITE
                    );
  pCharacteristic->setCallbacks(new EventsCallbacks());
  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE prêt");
}


// Callbacks de connexion et déconnexion du BLE
class ConnexionServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Périphérique connecté");
    };

    void onDisconnect(NimBLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Périphérique déconnecté");
    }
};

// Callbacks BLE : 
// gère les évents liés à l'écriture et la lecture de 
// l'esp32 <-> téléphone en BLE 
class EventsCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {

      // value == "M=1

      std::string value = pCharacteristic->getValue();
      if (value == "M=0") {
        currentMode = ROOM;
        Serial.println("Mode changé : Chambre");
      } else if (value == "M=1") {
        currentMode = EXT;
        Serial.println("Mode changé : Extérieur");
      } else if (value == "M=2") {
        currentMode = PARAM;
        Serial.println("Mode changé : Paramétrage");
      } else if (value == "RESET") {
        fallCount = 0;
        Serial.println("Compteur de chutes remis à zéro");
      }
    }

    void onRead(NimBLECharacteristic* pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      if (value == "fallCount") {
        pCharacteristic->setValue(std::to_string(fallCount));
        Serial.println("Nombre de chutes : " + String(fallCount));
      } else if (value == "networkConfig") {
        pCharacteristic->setValue(networkConfig);
        Serial.println("Configuration réseau : " + networkConfig);
      } else if (value == "connectionStatus") {
        pCharacteristic->setValue(connectionStatus);
        Serial.println("État de connexion : " + connectionStatus);
      }
    }
};




void setup() {
  
  Serial.begin(9600);
  delay(1000); 

    // Configuration
  pinMode(buttonPin, INPUT_PULLDOWN);
  Serial.println("Port accéléromètre prêt.");

  // initialisation de BLE 
  initBLE();

}

void loop() {

  delay(100); // il ne faut pas que le délai soit trop faible

  // différents Modes 
  switch (currentMode) {
    case ROOM:


      if (digitalRead(buttonPin) == HIGH) {
        // envoi de la requête 
        sendFallAlert();
      }
      break;

    case PARAM:
      // Active le bluetooth si PARAM
      // BLE pour le technicien 
      if (deviceConnected) {
        Serial.println("Périphérique connecté");
      } else {
        Serial.println("En attente de connexion...");
      }
      break;

    case EXT:
      // Active le fichier esp32-elk 
      // setupWiFi();
      if (digitalRead(buttonPin) == HIGH) {
        // envoi de la requête 
        sendFallAlert();
      }
      break;
  }
}
