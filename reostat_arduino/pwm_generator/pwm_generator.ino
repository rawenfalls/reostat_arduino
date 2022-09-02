#include <GyverTimers.h>
volatile byte dutyD13 = 0;

int e = 0;
int i = 0;
void setup() {
  
  Serial.begin(9600);
  pinMode(13, OUTPUT);
//Serial.println(Timer2.setFrequency(1280));
//Serial.println(Timer2.setFrequency(25600));
Serial.println(Timer2.setFrequency(100));
  //Timer2.setFrequency(50UL);
  Timer2.enableISR();
  i = analogRead(A0);
}

void loop() {
  i = analogRead(A0) / 20.48;
  Serial.print(i);
  Serial.print(',');
  Serial.println(e*50);
  dutyD13 = i;
  delay(20);
}

//void pwmTick(){
//static volatile byte counter = 0;
//if (counter == 100){counter = 0;}
//if (counter <= 0){
//  if (dutyD13 > 0){digitalWrite(13, HIGH);e=1;}
//}
////dutyD13 = dutyD13*10/255*25;
//if (counter == dutyD13){
//  if(dutyD13 < 100){
//    digitalWrite(13, LOW);e=0;
//    } 
//  }
//  counter++;
//}

void pwmTick(){
static volatile byte counter = 0;
if (counter == 100){counter = 0;}
if (counter <= 0){
  if (dutyD13 > 0){digitalWrite(13, HIGH);e=1;}
}
//dutyD13 = dutyD13*10/255*25;
if (counter == dutyD13){
  if(dutyD13*2 < 90){
    digitalWrite(13, LOW);e=0;
    } 
  }
  counter++;
}

ISR(TIMER2_A){
  pwmTick();
}
