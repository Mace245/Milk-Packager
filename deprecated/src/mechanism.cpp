#include <Arduino.h> 

int numberOfSteps = 200;
int pulseWidthMicros = 2000;  // microsecondo

// Stepper sealer
int directionSealerPin = 33;
int stepSealerPin = 25;

int proximPrevMillis;
int proximMilk = 26;
int proximSealer = 27; //P2

int milkValve = 13;
// int sealerMaster = 23;

int echo = 14;
int trig = 12;

void pinsInit(){
  pinMode(directionSealerPin, OUTPUT);
  pinMode(stepSealerPin, OUTPUT);

  pinMode(milkValve, OUTPUT);
//   pinMode(sealerMaster, OUTPUT);

  pinMode(proximMilk, INPUT);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
}

void milkDispenser(){
  Serial.print("Starting milk dispense...");
  digitalWrite(milkValve, HIGH);
  proximPrevMillis = millis();
  while (millis() < proximPrevMillis + 3800){
    if (digitalRead(proximMilk) == 1){
      Serial.println("Plastic not detected!");
      return;
    }
  }
  digitalWrite(milkValve, LOW);
  Serial.println("Milk dispense end");
}

void sealerMover(){
  Serial.print("Starting sealer mover...");

  // Use IF sealer relay is available
  // do{
  //   digitalWrite(stepSealerPin, HIGH);
  //   delayMicroseconds(pulseWidthMicros); 
  //   digitalWrite(stepSealerPin, LOW);
  //   delay(millisbetweenSteps);
  // } while(digtalRead(sealerRelay) == LOW);

  digitalWrite(directionSealerPin, LOW); // Closing sealer
  for(int n = 0; n < 110; n++) { 
    digitalWrite(stepSealerPin, HIGH);
    delayMicroseconds(pulseWidthMicros); 
    digitalWrite(stepSealerPin, LOW);
    delayMicroseconds(pulseWidthMicros); 
  }

  delay(1500);

  digitalWrite(directionSealerPin, HIGH); // Opening sealer
  for(int n = 0; n < 110; n++) {
    digitalWrite(stepSealerPin, HIGH);
    delayMicroseconds(pulseWidthMicros); 
    digitalWrite(stepSealerPin, LOW);
    delayMicroseconds(pulseWidthMicros); 
  }
  Serial.println("Sealer mover end");
}

float duration,   distance;

int milkLevel(){ // 0 is critically low; 1 is low; 2 is not low
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Detected distance: ");
  Serial.println(distance);
  
  if (distance > 38){
    return 0;
  }else if(distance >= 30){
    return 1;
  }else if(distance < 30){
    return 2;
  }
}