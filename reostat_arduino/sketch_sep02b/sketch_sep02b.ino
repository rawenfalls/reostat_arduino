void setup() {
  pinMode(13, OUTPUT);
}
void loop() {
//  analogWriteResolution(16);
//  analogWriteFreq(100);
//  
//  // ШИМ на 3 пин, 1023/4 = 255 - перевели диапазон
//  analogWrite(3, analogRead(0) / 4);

//  digitalWrite(13, HIGH);
//  delayMicroseconds(2000); // Approximately 10% duty cycle @ 1KHz
//  digitalWrite(13, LOW);
//  delayMicroseconds(10000 - 2000);
pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);
  TCCR2A = _BV(COM2A0) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS22);
  OCR2A = 180;
  OCR2B = 50;
}
