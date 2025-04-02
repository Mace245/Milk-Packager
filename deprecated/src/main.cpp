#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "mechanism.h"
#include "pFirebase.h"

int count = 0;

bool milk_flag, sealer_flag;

int I2C_SDA = 23;
int I2C_SCL = 22;

bool re_use_milk, re_use_sealer;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void lcdInit(){
  lcd.init(I2C_SDA, I2C_SCL); // initialize the lcd to use user defined I2C pins
	lcd.backlight();
}

void setup() { 
  Serial.begin(115200);
  Serial.println("Starting Test");

  pinsInit();
  lcdInit();
  firebaseInitWifi(count);
}

int debugMillis=0;

void loop() { 
  digitalWrite(milkValve, LOW); // make sure pump is off 
  
  if (re_use_milk == false && proximMilk) {
    re_use_milk = true;
  }

  if (re_use_sealer == false && proximSealer) {
    re_use_sealer = true;
  }

  /* Pump turns on if:
      1. Milk proximity sensor is triggered
      2. Milk level in container is not low
      3. Milk proximity sensor has a period where its not triggered from last dispensing (to prevent continuous triggering)
  */

  if (digitalRead(proximMilk) == 0 && milkLevel() != 0 && re_use_milk == true){ 
    milkDispenser();
    milk_flag = true;
    re_use_milk = false;
  }

  /* Sealer is on if
      1. Sealer proximity sensor is triggered
      2. Sealer proximity sensor has a period where its not triggered from last sealing (to prevent continuous triggering)
  */

  if (digitalRead(proximSealer) == 0 && re_use_sealer == true) {
    sealerMover();
    sealer_flag = true;
    re_use_sealer = false;
  }

  // Milk package count increases only if both milk dispenser and sealer has been triggered at least once

  if (milk_flag == true && sealer_flag == true){
    count++;
    milk_flag = false;
    sealer_flag = false; 
  }

  sendServer(count);
  delay(100);

  if (millis() - debugMillis > 500 || debugMillis == 0){
    debugMillis = millis();
    Serial.print("count: ");
    Serial.println(count);
    
    Serial.print("milk proxim status: ");
    Serial.println(digitalRead(proximMilk));

    Serial.print("milk reuse status: ");
    Serial.println(digitalRead(re_use_milk));
  }
}
