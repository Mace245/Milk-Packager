#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "time.h"
#include <Arduino.h> 
#include "mechanism.h"

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define WIFI_SSID "SOV"
#define WIFI_PASSWORD "iiiiiiii"

#define API_KEY "AIzaSyBmx9spwBUIeBbIpfBo3haK8A8SvChhhAw"
#define DATABASE_URL "https://console.firebase.google.com/u/0/project/esp32-cd/database/esp32-cd-default-rtdb/data/~2F" 

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

  char timeMonth[10];
  strftime(timeMonth, 10, "%B", &timeinfo);

  char timeWeekDay[10];
  strftime(timeWeekDay, 10, "%A", &timeinfo);

  char timeWeekDayNumber[10];
  strftime(timeWeekDayNumber, 10, "%d", &timeinfo);

  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);

  strcat(filePath, timeYear);
  strcat(filePath, "/");
  strcat(filePath, timeMonth);
  strcat(filePath, "/");
  strcat(filePath, timeWeekDayNumber);
  strcat(filePath, ",");
  strcat(filePath, timeWeekDay);
  strcat(filePath, "/");
  strcat(filePath, timeHour);

  // Example filepath: 2024/11/12,Monday/15
}

void firebaseInitWifi(int count){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Database
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Firebase sign in success");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectNetwork(true);

  checkTime();
  if (Firebase.ready() && signupOK && Firebase.RTDB.getInt(&fbdo, filePath)) { // Fetch count from rtdb if it's available
    if (fbdo.dataType() == "int") {
      count = fbdo.intData();
      Serial.print("Obtained previous count: ");
      Serial.println(count);
    }
  }
  else {
    Serial.println(fbdo.errorReason());
    if (fbdo.errorReason() == "path not exist"){
      return;
    }
    delay(1000);
    firebaseInitWifi(count);
  }
}

void sendServer(int count){
  checkTime();
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // First write operation
    // fbdo.clear();  // Clear the state of fbdo
    // if (Firebase.RTDB.setInt(&fbdo, filePath, count)) {
    //     Serial.println("PASSED");
    //     Serial.println("PATH: " + fbdo.dataPath());
    // } else {
    //     Serial.println("FAILED");
    //     Serial.println("REASON: " + fbdo.errorReason());
    // }

    delay(100);  // Add a delay between writes

    // Second write operation
    fbdo.clear();  // Clear the state again
    Serial.println(distToVol(distance));
    if (Firebase.RTDB.setFloat(&fbdo, "vol/vol", distToVol(distance))) {
        Serial.println("PASSED volume");
        Serial.println("PATH: " + fbdo.dataPath());
    } else {
        Serial.println("FAILED");
        Serial.println("REASON: " + fbdo.errorReason());
    }
}

delay(500);  // General delay

}