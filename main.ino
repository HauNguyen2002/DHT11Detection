#include <DHT.h>
#include <AccelStepper.h>

#define DHTPIN 2         
#define DHTTYPE DHT11    

#define FULLSTEP 4       
#define motorPin1  8     
#define motorPin2  9     
#define motorPin3  10    
#define motorPin4  11    

#define LED_PIN_H_H 5
#define LED_PIN_H_L 4 
#define LED_PIN_T_L 3 

DHT dht(DHTPIN, DHTTYPE);
AccelStepper stepper(FULLSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

unsigned long previousMillis = 0;
const long interval = 1000;
const long ledTimeout = 5000;
unsigned long belowThresholdTime[3] = {0,0,0};


void setup() {
  Serial.begin(9600);
  dht.begin();
  
  stepper.setMaxSpeed(1000.0);
  stepper.setAcceleration(100.0);
  stepper.setSpeed(1000);
  stepper.setCurrentPosition(0);
  pinMode(LED_PIN_H_H, OUTPUT);
  pinMode(LED_PIN_H_L, OUTPUT);
  pinMode(LED_PIN_T_L, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  float humidity;
  float temperature;

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    humidity = dht.readHumidity();          
    temperature = dht.readTemperature();    

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %\t");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    if (humidity > 80) {
      digitalWrite(LED_PIN_H_H, HIGH);
      belowThresholdTime[0] = 0;
    } else {
      if (belowThresholdTime[0] >= ledTimeout) digitalWrite(LED_PIN_H_H, LOW);
      else belowThresholdTime[0] += interval;
    }
    if (humidity < 50) {
      digitalWrite(LED_PIN_H_L, HIGH);
      belowThresholdTime[1] = 0;
    } else {
      if (belowThresholdTime[1] >= ledTimeout) digitalWrite(LED_PIN_H_L, LOW);
      else belowThresholdTime[1] += interval;
    }
    if (temperature < 30) {
      digitalWrite(LED_PIN_T_L, HIGH);
      belowThresholdTime[2] = 0;
    } else {
      if (belowThresholdTime[2] >= ledTimeout) digitalWrite(LED_PIN_T_L, LOW);
      else belowThresholdTime[2] += interval;
    }
  }

    if (humidity > 80 || temperature > 50) {
      stepper.moveTo(stepper.currentPosition() + 2000);
    }
    else{
      if (stepper.distanceToGo() == 0) stepper.stop();
    }
    if (stepper.distanceToGo() > 0) stepper.run();
}
