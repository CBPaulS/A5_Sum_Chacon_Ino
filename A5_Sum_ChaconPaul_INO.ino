#include <Arduino.h>

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

const char* WIFI_SSID = "Paul";
const char* WIFI_PASSWORD = "1234";

const char* API_KEY = "AIzaSyDIq9yYfYAn-g5vObYmQIEUXMFXuIm4D9I";
const char* DATABASE_URL = "https://termometro-b70ed-default-rtdb.firebaseio.com/";

const char* USER_EMAIL= "paulchacon@gmail.com";
const char* USER_PASSWORD= "hola123";

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
const int LDR_PIN = 12; 
const int minLDRValue = 100; // Valor mínimo del LDR 
const int maxLDRValue = 700; // Valor máximo del LDR
int LDRValor= 0;
float ValorMapeado = 0.0; // Valor mapeado entre 1 y 100

void setup_WIFI() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a Wi-Fi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
}

void setupFirebase() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback; 
  config.signer.tokens.legacy_token = "<token_secreto_base_de_datos>";
  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(2048);
  Firebase.begin(&config, &auth); 
  Firebase.setDoubleDigits(5);
  config.timeout.serverResponse = 10 * 1000;
  Serial.printf("Cliente Firebase v%s\n\n", FIREBASE_CLIENT_VERSION);
}

void setup() {
  Serial.begin(115200);
  setup_WIFI();
  setupFirebase();
  pinMode(LDR_PIN, INPUT);
}

void loop() {
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    LDRValor= analogRead(LDR_PIN);

    ValorMapeado = map(LDRValor, minLDRValue, maxLDRValue, 1.0, 100.0);

    if (Firebase.RTDB.setFloat(&fbdo, F("/Temp"), ValorMapeado)) {
      Serial.println("¡Enviado exitosamente!");
    } else {
      Serial.println("Error al enviar datos:");
      Serial.println(fbdo.errorReason().c_str());
    }
    sendDataPrevMillis = millis();
  }
}

