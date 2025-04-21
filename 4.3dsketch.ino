#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>

// Pin definitions
#define BUTTON_PIN 2
#define LED1_PIN 3
#define LED2_PIN 4
#define LED3_PIN 5
#define DHT_PIN 6

// Sensor objects
BH1750 lightMeter;
DHT dht(DHT_PIN, DHT11);

// Variables for interrupt flags
volatile bool buttonPressed = false;
volatile bool lightThresholdExceeded = false;
volatile bool tempThresholdExceeded = false;

// Variables for timer functionality
unsigned long previousMillis = 0;
const long interval = 1000; // 1 second interval

// Threshold values
const float LIGHT_THRESHOLD = 200.0; // Lux
const float TEMP_THRESHOLD = 28.0;   // °C

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial to initialize
  
  // Initialize pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  

  
  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  
  // Initialize sensors
  Wire.begin();
  lightMeter.begin();
  dht.begin();
  
  Serial.println("System initialized");
}


void loop() {
  unsigned long currentMillis = millis();
  
  // Timer-based actions (every 'interval' milliseconds)
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read sensors
    float lux = lightMeter.readLightLevel();
    float temp = dht.readTemperature();
    
    // Check light threshold
    if (lux > LIGHT_THRESHOLD) {
      lightThresholdExceeded = true;
    }
    
    // Check temperature threshold
    if (!isnan(temp) && temp > TEMP_THRESHOLD) {
      tempThresholdExceeded = true;
    }
    
    // Toggle LED3 (timer-controlled)
    digitalWrite(LED3_PIN, !digitalRead(LED3_PIN));
    
    // Log sensor readings
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.print(" lux | Temp: ");
    Serial.print(temp);
    Serial.println(" °C");
  }

  
  // Handle button interrupt
  if (buttonPressed) {
    buttonPressed = false;
    digitalWrite(LED1_PIN, !digitalRead(LED1_PIN));
    Serial.println("Button pressed - LED1 toggled");
  }
  
  // Handle light sensor threshold
  if (lightThresholdExceeded) {
    lightThresholdExceeded = false;
    digitalWrite(LED2_PIN, !digitalRead(LED2_PIN));
    Serial.println("Light threshold exceeded - LED2 toggled");
  }
  
  // Handle temperature threshold
  if (tempThresholdExceeded) {
    tempThresholdExceeded = false;
    Serial.println("Temperature threshold exceeded");
  }
}

// Button interrupt service routine
void buttonISR() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  
  // Debounce - ignore interrupts faster than 200ms
  if (interruptTime - lastInterruptTime > 200) {
    buttonPressed = true;
  }
  lastInterruptTime = interruptTime;
}