#include <GyverBME280.h>                      // Подключение библиотеки bmp280 -датчика давления в шланге
GyverBME280 bme; // Создание обьекта bme

#include <microDS18B20.h> 
MicroDS18B20<5> ds;

#include <GyverPID.h>
GyverPID pid;


int cooler = 12;
int gas = 11;
int nagrew = 10;
int led = 9;

#include <TroykaDHT.h>// библиотека дачика температуры внутри корпуса
DHT dht(4, DHT11);//инициализациядатчика 

#include <NDIRZ16.h>
#include <SoftwareSerial.h>
//Arduino UNO Pin D2 (Software Serial Rx) <===> Adaptor's Green  Wire (Tx)
//Arduino UNO Pin D3 (Software Serial Tx) <===> Adaptor's Yellow Wire (Rx)
SoftwareSerial mySerial(2,3);
NDIRZ16 mySensor = NDIRZ16(&mySerial);

void setup() {
  Serial.begin(9600);                         // Запуск последовательного порта
  Serial.setTimeout(5);
  
  bme.begin();                                // инициализируем датчик bmp280 
  dht.begin();                                // инициализируем датчик DHT11 
  
  pinMode(led, OUTPUT);                       //устанвливаем пины для выхода
  pinMode(cooler, OUTPUT);
  pinMode(gas, OUTPUT);
  pinMode(nagrew, OUTPUT);
  
  digitalWrite(led, LOW);                   //устанавливаем на пинах низкий уровень
  digitalWrite(cooler, HIGH);
  digitalWrite(gas, LOW);
  digitalWrite(nagrew, LOW);
  mySerial.begin(9600);//for co2
  //Serial.println("Wait 10 seconds for the sensor to starup");
   // delay(10000);


   pid.Kp = 0.3;
   pid.Ki = 0.001;//не увеличивать в 10 раз в 5 раз уже не стабильно от 0,001
   pid.Kd = 30;

   //pid.Kp = 0.22;//0.2 много 0.168 мало
   //pid.Ki = 0.00036;//не увеличивать в 10 раз в 5 раз уже не стабильно от 0,001
   //pid.Kd = 30;
   pid.setDt(1000);
   pid.setDirection(NORMAL);
   pid.setpoint = 37;
}

float temperature1;
float temperature2 = 35;
int reg = 1;
int pidr = 0;
void loop() {
  static uint32_t timer = millis();
  uint32_t sec = millis() / 1000ul;
  int timeHours = (sec / 3600ul);        // часы
  int timeMins = (sec % 3600ul) / 60ul;  // минуты
  int timeSecs = (sec % 3600ul) % 60ul;  // секунды
  
  if (millis() - timer >= 1000){
    timer = millis();
    if (ds.readTemp()){
      
      temperature1 = ds.getTemp();
      Serial.print(temperature1);
      Serial.print(" ");
      temperature1 = runMiddleArifmOptim(temperature1);
      //temperature1 = expRunningAverage(temperature1);
      }
  ds.requestTemp();
  
  //gas_pressure();
  //temperature1 = temperatyre();
  
  if (ds.readTemp()){Serial.print(temperature1);Serial.print(" C");}
  Serial.print(" ");

  time_now( timeSecs, timeMins,  timeHours);

//pidControl( temperature1);
  control_signal();
    
  //co2_sensor();
//  Serial.print(millis());
//  Serial.print(" msec ");


  if (pidr == 1)
  {
    pid.input = temperature1;
  pid.getResultTimer();
    Serial.print(" ");
  float t = pid.output;  
  Serial.print(t,5);
  Serial.print(" ");
  if (t >= 1 ){//&& pid.setpoint >= temperature1 && pid.setpoint >= (temperature1-0.2)
  Serial.print("nagrew on");
  digitalWrite(nagrew, 1); 
  }
  else{
  Serial.print("nagrew off");
  digitalWrite(nagrew, 0); 
  }
  }
  Serial.println("");     
  }
}


const int NUM_READ = 10;  // количество усреднений для средних арифм. фильтров
// оптимальное бегущее среднее арифметическое
float runMiddleArifmOptim(float newVal) {
  static int t = 0;
  static float vals[NUM_READ];
  
  static float average = 0;
  if (++t >= NUM_READ) t = 0; // перемотка t
  average -= vals[t];         // вычитаем старое
  average += newVal;          // прибавляем новое
  vals[t] = newVal;           // запоминаем в массив
  return ((float)average / NUM_READ);
}

float k = 0.05;  // коэффициент фильтрации, 0.0-1.0
// бегущее среднее
float expRunningAverage(float newVal) {
  static float filVal = 0;
  filVal += (newVal - filVal) * k;
  return filVal;
}

void pidControl(float temperature1){
//  static uint32_t tmr;
//  if (millis() - tmr >= 1000)
//  {
//    tmr = millis();
    pid.input = temperature1;
  pid.getResultTimer();
  if (pidr == 1)
  {
    Serial.print(" ");
  float t = pid.output;  
  Serial.print(t,5);
  Serial.print(" ");
  if (t >= 1){
  Serial.print("nagrew on");
  digitalWrite(nagrew, 1); 
  }
  else{
  Serial.print("nagrew off");
  digitalWrite(nagrew, 0); 
  }
  }
  //  }
  }
  
void time_now(int timeSecs, int timeMins, int timeHours){
    Serial.print(timeSecs);
  Serial.print(" sec ");
  Serial.print(timeMins);
  Serial.print(" min ");
  Serial.print(timeHours);
  Serial.print(" hours ");
  }

void co2_sensor(){
     if (mySensor.measure()) {
        Serial.print("CO2 Concentration is ");
        Serial.print(mySensor.ppm);
        Serial.print ("ppm ");
        Serial.print(mySensor.ppm/10000.0);
        Serial.println("%");
    }
}
float temperature;
float temperatyre(){
  dht.read(); // считывание данных с датчика
  
  switch(dht.getState()) {// проверяем состояние данных
    case DHT_OK: // всё OK
      Serial.print("Temperature = ");
       temperature = dht.getTemperatureC();
      Serial.print(temperature);
      Serial.println(" C \t");   
      break;
    case DHT_ERROR_CHECKSUM: // ошибка контрольной суммы
      Serial.println("Checksum error");
      break;
    case DHT_ERROR_TIMEOUT:// превышение времени ожидания
      Serial.println("Time out error");
      break;
    case DHT_ERROR_NO_REPLY: // данных нет, датчик не реагирует или отсутствует
      //Serial.println("Sensor not connected");
      Serial.print("Temperature = ");
      Serial.print(temperature);
      Serial.println(" C \t");  
      break;
  }
  return (temperature);
}

void control_signal(){//управление происходит путем передачи в порт первой буквы устройства которым необходимо управлять,
                      //управление происходит путем изменения уровня(high or low)
         if (Serial.available() > 0) {
    char key = Serial.read();//читает первый символ
    int val = Serial.parseInt();
    //int in_data = Serial.read() - '0';//принемает циру

   // int data = Serial.parseInt();// читает число
    //Serial.println(in_data);
    switch (key) {
      case 'l': if (digitalRead(led))
        {digitalWrite(led, LOW); Serial.print(" led off");}
        else
        { digitalWrite(led, HIGH); Serial.print(" led on");}
          break;
        case 'c': if (digitalRead(cooler))
        {digitalWrite(cooler, LOW); Serial.print(" cooler off");}
        else
        { digitalWrite(cooler, HIGH); Serial.print(" cooler on");}
          break;
        case 'g': if (digitalRead(gas))
        {digitalWrite(gas, LOW); Serial.print(" gas off");}
        else
        { digitalWrite(gas, HIGH); Serial.print(" gas on");
          delay(500);
          digitalWrite(gas, LOW); Serial.print(" gas off");
        }
          break;
        case 'n': if (digitalRead(nagrew))
        {digitalWrite(nagrew, LOW); Serial.print(" nagrew off");}
        else
        { digitalWrite(nagrew, HIGH); Serial.print(" nagrew on");}
          break;
        case 'p': if (pidr)
        {pidr = 0; Serial.print(" pid off");digitalWrite(nagrew, LOW);}
        else
        { pidr = 1; Serial.print(" pid on");digitalWrite(nagrew, LOW);}
          break;
         case 't': pid.setpoint = val;
         Serial.print(" new temperature = ");
         Serial.print(val);
          break;
          
          case 'o': pid.Kp = val;
         Serial.print(" p = ");
         Serial.print(val);
          break;
          case 'i': pid.Ki = val;
         Serial.print(" i = ");
         Serial.print(val);
          break;
          case 'd': pid.Kd = val;
         Serial.print(" d = ");
         Serial.print(val);
          break;
        }  
    } 
}

void gas_pressure(){
//  Serial.print("Temperature: ");
//  Serial.print(bme.readTemperature());        // Выводим темперутуру в [*C]
//  Serial.println(" *C");

//  Serial.print("Humidity: ");
//  Serial.print(bme.readHumidity());           // Выводим влажность в [%]
//  Serial.println(" %");

  float pressure = bme.readPressure();        // Читаем давление в [Па]
  Serial.print("Pressure: ");
  Serial.print(pressure / 100.0F);            // Выводим давление в [гПа]
  Serial.print(" hPa , ");
  Serial.print(pressureToMmHg(pressure));     // Выводим давление в [мм рт. столба]
  Serial.println(" mm Hg");
//  Serial.print("Altitide: ");
//  Serial.print(pressureToAltitude(pressure)); // Выводим высоту в [м над ур. моря]
//  Serial.println(" m");
  //Serial.println("");
}
