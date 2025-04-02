#include <Arduino.h> 

int proximPrevMillis;
int proximMilk = 4;
int milkValve = 13;


void milkDispenser(){
  Serial.print("Starting milk dispense...");
  digitalWrite(milkValve, LOW);
  proximPrevMillis = millis();
  while (millis() < proximPrevMillis + 4000){
    // if (digitalRead(proximMilk) == 1){
    //   Serial.println("Plastic not detected!");
    //   return;
    // }
  }
  digitalWrite(milkValve, HIGH);
  Serial.println("Milk dispense end");
}

void setup() { 
  Serial.begin(9600);
  Serial.println("Starting Test");

  pinMode(milkValve, OUTPUT);
  pinMode(proximMilk, INPUT);
}


void loop() { 
  digitalWrite(milkValve, HIGH);
}
