#include <Arduino.h>
#include "mechanism.h"
#include "pFirebase.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int count = 0, dispenser_flag = 0, debugMillis=0;
bool firebase_flag = true;

void setup() { 
  Serial.begin(115200);
  Serial.println("Starting Test");

  pinsInit();
  lcd.init();
	lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("Mencari WiFi...");
  
  if (firebaseInitWifi(count) == 100){
    firebase_flag = false;
    lcd.setCursor(0,0);
    lcd.print("Tidak ada WiFi");
    lcd.setCursor(0,1);
    lcd.print("Fitur Online off");
    delay(2000);
  } else {
    lcd.setCursor(0,0);
    lcd.print("WiFi Tersambung");
    lcd.setCursor(0,1);
    lcd.print("Fitur Online on");
    delay(2000);
  }
  lcd.clear();
}


void loop() { 
  digitalWrite(milkValve, LOW); // make sure pump is off 

  // milkLevel() != 0 
  if (digitalRead(proximMilk) == 0 && digitalRead(pedal) == 0 && dispenser_flag == 0){
    delay(500);
    while(!milkDispenser()){
      //Wait for pump to finish
    }
    dispenser_flag = 1; 
    count++;
  }

  if (digitalRead(pedal) == 0 && dispenser_flag == 1){
    sealerMoverServo();
    dispenser_flag = 0;
  }

  if (firebase_flag != false){
    sendServer(count);
  }

  // Kendali LCD
  lcd.setCursor(0,0);
  lcd.print("Jumlah kemas:");

  lcd.setCursor(13, 0);
  lcd.print(count);

  lcd.setCursor(0,1);

  if (dispenser_flag == false){
    lcd.print("Injak untuk pompa");
  } else {
    lcd.print("Injak untuk seal");
  }

  delay(400);

  // Debugging
  if (millis() - debugMillis > 800 || debugMillis == 0){
    debugMillis = millis();

    Serial.print("count: ");
    Serial.println(count);
    
    Serial.print("milk proxim status: ");
    Serial.println(digitalRead(proximMilk));

    Serial.print("pedal status: ");
    Serial.println(digitalRead(pedal));

    Serial.print("milk volume: ");
    Serial.println(distToVol(distance));

    Serial.print("milk Distance: ");
    Serial.println(distance);
  }
}