
#define echoPin 4
#define trigPin 5

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(2);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);



//Limits
float phLlimit = 4, phHlimit = 7;
float moistureLimit = 30;
float waterLimit = 300;



int phval = 0;
unsigned long int avgval;
int buffer_arr[10], temp;
float temperature = 0.00,distance = 0.00,pH = 0.00,moisture = 0.00,calibration_value= 22.36;



void setup() {
  Serial.begin(9600);
  sensors.begin();
  lcd.begin();
  lcd.backlight();

 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(A3, INPUT);

  pinMode(7, OUTPUT); //Pump 1
  pinMode(9, OUTPUT); //Pump 2
  pinMode(11, OUTPUT); //Pump 3 (Water)

}

void loop() {
  gettemperature();
  getpH();
  getMoisture();
  getDistance();
  Serial.print("Celsius temperature: ");
  Serial.print(temp);
  Serial.print(" | Distance: ");
  Serial.print(distance);
  Serial.print(" | pH: ");
  Serial.print(pH);
  Serial.print(" | Moisture: ");
  Serial.println(moisture);

  delay(200);
  getDistance();
  dis("D:" + String((int)distance) + "mm  M:" + String((int)moisture) + "%", "pH:" + String(pH) + " T:" + String(temperature));


  if (pH < phLlimit)
    digitalWrite(7, 1);
  else
    digitalWrite(7, 0);

  if (pH > phHlimit)
    digitalWrite(9, 1);
  else
    digitalWrite(9, 0);

  if (moisture<moistureLimit)
    digitalWrite(11, 1);
  else if(waterLimit > distance)
    digitalWrite(11, 0);
  //else digitalWrite(11, 0);

}









void getMoisture() {
  moisture = ((1023.00 - analogRead(A3)) / 1024.00) * 100.00;
}


void gettemperature() {
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
}


void getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  distance =  pulseIn(echoPin, HIGH) * (0.034 / 2) * 10;

}

void getpH() {
  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(A7);
    delay(30);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 6;
  pH = -5.70 * volt + calibration_value;
}

void dis(String a, String b)
{
  lcd.clear();

  lcd.setCursor(0, 0); // set cursor to 1 symbol of 1 line
  lcd.print(a);

  lcd.setCursor(0, 1); // set cursor to 1 symbol of 2 line
  lcd.print(b);
}
