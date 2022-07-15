#include <util/atomic.h>

// Pins
#define ENCA 2
#define ENCB 3
#define A1A 5 // capable of PWM
#define A1B 6 // capable of PWM

int speed = 255;

int num = 10;
float cpsAvg = 0;
float rpmAvg;
 
// globals
long prevT = 0;
int posPrev = 0;
// Use the "volatile" directive for variables
// used in an interrupt
volatile int pos_i = 0;

void setup() {
  Serial.begin(9600);

  pinMode(ENCA,INPUT);
  pinMode(ENCB,INPUT);
  pinMode(A1A,OUTPUT);
  pinMode(A1B,OUTPUT);

  attachInterrupt(digitalPinToInterrupt(ENCA),readEncoder,RISING);
  analogWrite(A1A, 0);
  analogWrite(A1B, speed);

}

void loop() {
  // put your main code here, to run repeatedly:

  // read the position in an atomic block
  // to avoid potential misreads  
  int pos = 0;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    pos = pos_i; //pos_i is updated in the ISR
  }

  // Compute counts per second (cps)
  long currT = micros();
  float deltaT = ((float) (currT-prevT))/1.0e6;
  float cps = (pos - posPrev)/deltaT;
  float rpm = cps*60/(34*12);
  posPrev = pos;
  prevT = currT;

  cpsAvg = ((num-1)*cpsAvg + cps)/num;
  rpmAvg = cpsAvg*60/(34*12);

  Serial.print(rpm);
  Serial.print(" ");
  Serial.println(rpmAvg);

  //Serial.println(pos);
  //delay(1);
  
}

void readEncoder(){
  // Read encoder B when ENCA rises
  int b = digitalRead(ENCB);
  int increment = 0;
  if(b>0){
    // If B is high, increment forward
    increment = 1;
  }
  else{
    // Otherwise, increment backward
    increment = -1;
  }
  pos_i += increment;
  //pos_i += 1;
}
