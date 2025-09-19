int count = 0;

unsigned long sendDataPrevMillis = 0;

// Stepper sealer
int directionSealerPin = 25;
int stepSealerPin = 3;

int proximPrevMillis;
int proximSen = 3;

int milkValve = 27;
int sealerMaster = 14;

int numberOfSteps = 200;
int pulseWidthMicros = 20;  // microsecondo
int millisbetweenSteps = 10; // milliseconds - or try 100 for slower steps

int milkDispenser(){
  Serial.print("Starting milk dispense...");
  digitalWrite(milkValve, HIGH);
  proximPrevMillis = millis();
  while (millis() < proximPrevMillis + 2000){
    if (!digitalRead(proximSen)){
      Serial.println("Plastic not detected!");
      return 0;
    }
  }
  digitalWrite(milkValve, LOW);
  Serial.println("Milk dispense end");
}

void sealerMover(int sealerMoverPin ,int directionPin){
  Serial.print("Starting sealer mover...");
  digitalWrite(directionPin, HIGH);
  for(int n = 0; n < numberOfSteps; n++) {
    digitalWrite(sealerMoverPin, HIGH);
    delayMicroseconds(pulseWidthMicros); 
    digitalWrite(sealerMoverPin, LOW);
    delay(millisbetweenSteps);
  }

  // delay(100);

  digitalWrite(directionPin, LOW);
  for(int n = 0; n < numberOfSteps; n++) {
    digitalWrite(sealerMoverPin, HIGH);
    delayMicroseconds(pulseWidthMicros); 
    digitalWrite(sealerMoverPin, LOW);
    delay(millisbetweenSteps);
  }
  Serial.println("Sealer mover end");
}

void setup() { 
  Serial.begin(9600);
  Serial.println("Starting Test");
}

void loop() { 
  Serial.println(digitalRead(proximSen));
  if (digitalRead(proximSen)){
    milkDispenser();
    sealerMover(stepSealerPin, directionSealerPin);

    count++;
    Serial.println(count);
  }
}
