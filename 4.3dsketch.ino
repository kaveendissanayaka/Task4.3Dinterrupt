#define USING_TIMER_TC3 true

#include <SAMDTimerInterrupt.h>
#include <Wire.h>
#include <BH1750.h>

SAMDTimer ITimer(TIMER_TC3);

#define BUTTON_PIN     2  // External Interrupt
#define PIR_PIN        3  // External Interrupt
#define LED_BUTTON     5
#define LED_PIR        6
#define LED_TIMER      7

BH1750 lightSensor;

volatile bool buttonState = false;
volatile bool pirState = false;
volatile bool timerFlag = false;

// Interrupt Service Routines
void handleButtonInterrupt() {
  buttonState = !buttonState;
  digitalWrite(LED_BUTTON, buttonState);
  Serial.println("Button interrupt: LED1 toggled");
}

void handlePIRInterrupt() {
  pirState = !pirState;
  digitalWrite(LED_PIR, pirState);
  Serial.println("PIR interrupt: LED2 toggled");
}

// Timer ISR - called every 1000ms
void TimerHandler() {
  digitalWrite(LED_TIMER, !digitalRead(LED_TIMER));
  timerFlag = true;  // Used to print sensor data from loop()
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Use internal pull-up for the button pin
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_BUTTON, OUTPUT);
  pinMode(LED_PIR, OUTPUT);
  pinMode(LED_TIMER, OUTPUT);

  // Interrupt on FALLING edge because button pulls pin LOW when pressed
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), handlePIRInterrupt, RISING);

  Wire.begin();
  lightSensor.begin();

  // Timer setup: 1 second (1000 ms)
  if (ITimer.attachInterruptInterval(1000000, TimerHandler)) {
    Serial.println("Timer interrupt started (1s interval)");
  }

  Serial.println("System Initialized");
}

void loop() {
  if (timerFlag) {
    timerFlag = false;
    float lux = lightSensor.readLightLevel();
    Serial.print("Light sensor reading (BH1750): ");
    Serial.print(lux);
    Serial.println(" lx");
  }
}
