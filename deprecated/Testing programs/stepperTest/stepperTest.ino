
// Stepper sealer
int directionSealerPin = 3;
int stepSealerPin = 2;

void setup() {
  // put your setup code here, to run once:
  pinMode(directionSealerPin, OUTPUT);
  pinMode(stepSealerPin, OUTPUT);

  Serial.begin(115200);

}

int numberOfSteps = 200;
int pulseWidthMicros = 1000;  // microsecondo

void loop() {
  // put your main code here, to run repeatedly:
    Serial.print("Starting sealer mover...");
  digitalWrite(directionSealerPin, HIGH);

  // Use IF sealer relay is available
  // do{
  //   digitalWrite(stepSealerPin, HIGH);
  //   delayMicroseconds(pulseWidthMicros); 
  //   digitalWrite(stepSealerPin, LOW);
  //   delay(millisbetweenSteps);
  // } while(digtalRead(sealerRelay) == LOW);

  for(int n = 0; n < numberOfSteps; n++) {
    digitalWrite(stepSealerPin, HIGH);
    delayMicroseconds(pulseWidthMicros); 
    digitalWrite(stepSealerPin, LOW);
    delayMicroseconds(pulseWidthMicros); 

  }

  delay(100);

  digitalWrite(directionSealerPin, LOW);
  for(int n = 0; n < numberOfSteps; n++) {
    digitalWrite(stepSealerPin, HIGH);
    delayMicroseconds(pulseWidthMicros); 
    digitalWrite(stepSealerPin, LOW);
    delayMicroseconds(pulseWidthMicros); 
  }
  Serial.println("Sealer mover end");
  delay(2000);

}
