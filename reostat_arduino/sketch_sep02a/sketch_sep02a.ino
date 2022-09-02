#include <GyverTimers.h>
volatile byte dutyD13 = 0;

int e = 0;
int i = 0;
void setup() {

  Serial.begin(9600);
  pinMode(13, OUTPUT);
  Serial.println(Timer1.setFrequency(50));
  Timer1.enableISR();
  i = analogRead(A0);
}

void loop() {
  dutyD13 = analogRead(A0) / 20.48;
  Serial.print(dutyD13);
  Serial.print(',');
  Serial.println(e * 50);
  delay(10);
}

void pwmTick() {
  static volatile byte counter = 0;
  if (counter == 50) {
    counter = 0;
  }
  if (dutyD13 > 0) {
    digitalWrite(13, HIGH);
    e = 1;
  }
  if (counter >= dutyD13) {
    if (dutyD13 < 49) {
      digitalWrite(13, LOW);
      e = 0;
    }
  }
  counter++;
}

ISR(TIMER1_A) {
  pwmTick();
}
