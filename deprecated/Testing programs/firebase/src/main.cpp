#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "time.h"
#include <Arduino.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define WIFI_SSID "SOV"
#define WIFI_PASSWORD "iiiiiiii"

#define API_KEY "AIzaSyBmx9spwBUIeBbIpfBo3haK8A8SvChhhAw"
#define DATABASE_URL "https://esp32-cd-default-rtdb.asia-southeast1.firebasedatabase.app/" 

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;

char filePath[40] = "";

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

int count = 0;
int  dispenser_flag = 0;
int debugMillis=0;
bool firebase_flag = true;


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

int firebaseInitWifi(int count){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  unsigned long wifiStartTime = millis(); // Track the time when the connection attempt started

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);

    // Check if more than 8 seconds have passed since the connection attempt started
    if (millis() - wifiStartTime > 8000) {
      Serial.println("\nNo Wi-Fi, using offline mode");
      return 100;
    }
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
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  checkTime();
  if (Firebase.ready() && signupOK && Firebase.RTDB.getInt(&fbdo, filePath)) { // Fetch count from rtdb if it's available
    count = fbdo.intData();
    Serial.print("Obtained existing count: ");
    Serial.println(count);
  } else {
    Serial.println(fbdo.errorReason());
    if (fbdo.errorReason() == "path not exist"){
      return 0;
    }
    delay(1000);
    firebaseInitWifi(count);
  }
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

    if (Firebase.RTDB.setInt(&fbdo, "vol", 1000)){
      Serial.println("PASSED volume");
      Serial.println("PATH: " + fbdo.dataPath());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Test");

  lcd.init();
	lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Menyari WiFi...");
  
  if (firebaseInitWifi(count) == 100){
    firebase_flag = false;
    lcd.setCursor(0,0);
    lcd.print("Tidak ada WiFi");
    lcd.setCursor(0,1);
    lcd.print("Fitur Online off");
    delay(2000);
  }
}

void loop() {
  if (firebase_flag != false){
    sendServer(count);
  }

  lcd.setCursor(0,0);
  lcd.print("Jumlah kemas:");

  lcd.setCursor(14, 0);
  lcd.print(count);

  lcd.setCursor(0,1);

  if (dispenser_flag == false){
    lcd.print("Injak untuk pompa");
  } else {
    lcd.print("Injak untuk seal");
  }
}