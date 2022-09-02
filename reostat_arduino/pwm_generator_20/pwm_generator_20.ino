#include <GyverTimers.h>
volatile byte dutyD13 = 0;

//#define PWM_DEPTH 16;
int e = 0;
int i = 0;
void setup() {
  
  Serial.begin(9600);
  pinMode(13, OUTPUT);
//Serial.println(Timer2.setFrequency(1280));
//Serial.println(Timer2.setFrequency(25600));
Serial.println(Timer2.setFrequency(1000));
  //Timer2.setFrequency(50UL);
  Timer2.enableISR();
  i = analogRead(A0);
}

void loop() {
  i = analogRead(A0) / 51;
  Serial.print(i);
  Serial.print(',');
  Serial.println(e*20);
  dutyD13 = i;
  delay(20);
}

void pwmTick(){
static volatile byte counter = 0;
if (counter == 20){counter = 0;}
if (counter <= 0){
  if (dutyD13 > 0){digitalWrite(13, HIGH);e=1;}
}
//dutyD13 = dutyD13*10/255*25;
if (counter == dutyD13){
  if(dutyD13 < 20){
    digitalWrite(13, LOW);e=0;
    } 
  }
  counter++;
}

ISR(TIMER2_A){
  pwmTick();
}
