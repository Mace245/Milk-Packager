#include <Arduino.h> 
#include <ESP32Servo.h>

int servoPin = 25;

Servo servo;

int proximPrevMillis;
int proximMilk = 26;
int pedal = 27; //P2

int milkValve = 13;

int echo = 14;
int trig = 12;


void pinsInit(){ // Initialisasi pin microcontroller
  pinMode(servoPin, OUTPUT);

  pinMode(milkValve, OUTPUT);

  pinMode(proximMilk, INPUT);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);


	servo.attach(servoPin); // attaches the servo on pin 18 to the servo object
}


bool milkDispenser(){ // Mulai mengalirkan susu
  Serial.print("Starting milk dispense...");
  digitalWrite(milkValve, HIGH);
  proximPrevMillis = millis();
  while (millis() < proximPrevMillis + 3800){
    // if (digitalRead(proximMilk) == 1){
    //   Serial.println("Plastic not detected!");
    //   return;
    // }
  }
  digitalWrite(milkValve, LOW);
  Serial.println("Milk dispense end");
  return true;
}


void sealerMoverServo(){ // Membuka dan menutupi sealer
  servo.write(0);
  Serial.print("Starting sealer mover...");

  while(digitalRead(pedal) == 0){
    servo.write(180);
      delay(800);
  }
  Serial.print("Sealer mover end");
  servo.write(0);
}

// void sealerMoverStepper(bool flag, int RPM){
//   stepper.setMaxSpeed(RPM);
  
//   Serial.print("Starting sealer mover...");

//   // Use IF sealer relay is available
//   // do{
//   //   digitalWrite(stepSealerPin, HIGH);
//   //   delayMicroseconds(pulseWidthMicros); 
//   //   digitalWrite(stepSealerPin, LOW);
//   //   delay(millisbetweenSteps);
//   // } while(digtalRead(sealerRelay) == LOW);
//   if (flag == true){
//     stepper.moveTo(-400);
//     while (stepper.distanceToGo() != 0) {
//       stepper.run();
//     }
//   }

//   if(flag == false){
//     stepper.moveTo(0);
//     while (stepper.distanceToGo() != 0) {
//       stepper.run();
//     }
//     Serial.println("Sealer mover end");
//   }
// }

// void sealerMoverLinear(){
//   analogWrite(, 0);
//    analogWrite(directionSealerPin, 255);
//    delay(5000);

//   analogWrite(stepSealerPin, 255);
//   analogWrite(directionSealerPin, 0);
//   delay(5000);
  
//   analogWrite(stepSealerPin, 0);
//    analogWrite(directionSealerPin, 0);
//    delay(5000);
// }

float duration, distance;

int milkLevel(){ // Mendapatkan volume susu pada wadah dari sensor ultrasonic
  // 0 is critically low; 1 is low; 2 is not low
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  // Serial.print("Detected distance: ");
  // Serial.println(distance);
  
  if (distance > 38){
    return 0;
  }else if(distance >= 30){
    return 1;
  }else if(distance < 30){
    return 2;
  }
}

float distToVol(float distance){
  int x;
  x = (41 - distance) * 27.5 * 29 / 1000;
  return x;
}