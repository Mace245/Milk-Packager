#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "time.h"
#include <Arduino.h> 
#include "mechanism.h"
#include <Wire.h>

#define WIFI_SSID "KANTOR DESA MARGAMULYAA"
#define WIFI_PASSWORD "baktiaksi24"

#define API_KEY "AIzaSyBmx9spwBUIeBbIpfBo3haK8A8SvChhhAw"
#define DATABASE_URL "https://esp32-cd-default-rtdb.asia-southeast1.firebasedatabase.app/" 

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;

char filePath[40] = "";

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void checkTime(){ // creates filepath based on time for rtdb push
  configTime(gmtOffset_sec, 0, ntpServer);
  struct tm timeinfo;
  strcpy(filePath, ""); 

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
//   Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  char timeYear[6];
  strftime(timeYear, 6, "%Y", &timeinfo);

  char timeMonth[5];
  strftime(timeMonth, 10, "%m", &timeinfo);

  char timeWeekDayNumber[5];
  strftime(timeWeekDayNumber, 10, "%d", &timeinfo);

  strcat(filePath, timeYear);
  strcat(filePath, "/");
  strcat(filePath, timeMonth);
  strcat(filePath, "/");
  strcat(filePath, timeWeekDayNumber);

  // Example filepath: 2024/11/12,Monday/15
}

int firebaseInitWifi(int count) {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  unsigned long wifiStartTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
    if (millis() - wifiStartTime > 15000) {
      Serial.println("\nNo Wi-Fi, using offline mode");
      return 100;
    }
  }
  Serial.println("\nConnected with IP: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase sign-in success");
    signupOK = true;
  } else {
    Serial.printf("Sign-up Error: %s\n", config.signer.signupError.message.c_str());
    return 100;
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  checkTime();

  if (Firebase.ready() && signupOK) {
    if (Firebase.RTDB.getInt(&fbdo, filePath)) {
      count = fbdo.intData();
      Serial.print("Obtained existing count: ");
      Serial.println(count);
    } else if (fbdo.errorReason() != "path not exist") {
      Serial.printf("Firebase Error: %s\n", fbdo.errorReason().c_str());
      return 100; // Treat other errors as critical
    }
  }
  return 0; // Success
}

void sendServer(int count){
  checkTime();
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.setInt(&fbdo, filePath, count)){
      Serial.println("PASSED count");
      Serial.println("PATH: " + fbdo.dataPath());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    delay(500);

    if (Firebase.RTDB.setInt(&fbdo, "vol", distToVol(distance))){
      Serial.println("PASSED volume");
      Serial.println("PATH: " + fbdo.dataPath());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}