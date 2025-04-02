 #define pwm1 12
#define pwm2 14

void setup(){
   pinMode(pwm1, OUTPUT);
   pinMode(pwm2, OUTPUT);
}

void loop(){
   analogWrite(pwm1, 0);
   analogWrite(pwm2, 255);
   delay(5000);

  analogWrite(pwm1, 255);
  analogWrite(pwm2, 0);
  delay(5000);
  
  analogWrite(pwm1, 0);
   analogWrite(pwm2, 0);
   delay(5000);
}