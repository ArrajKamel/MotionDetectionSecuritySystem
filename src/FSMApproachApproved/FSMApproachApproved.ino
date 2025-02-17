#include <Wire.h>
#include <LiquidCrystal.h>
#include <Keypad.h>


// Pin Definitions
#define SHARP_SENSOR_PIN A15
#define BUZZER_PIN 25
#define RED_LED_PIN 26
#define ACTIVATION_BUTTON 33

// Constants
#define TIMER_DURATION 10000 // 10 seconds (asstimated time for the owneer to input the password)
#define ALARM_AUTO_STOP_TIME 300000 // 5 minutes (the alarm will keep ringing for five minutes maximum in case no password introduced)
#define REACTIVATION_DELAY 300000 // 5 minuts (the systme will be ready after five minutes of user turning it on which is the ustimated time for the user to empty the place) 

// FSM States
enum State {
  IDLES,
  TIMER_ON,
  ALARM_ACTIV,
  SLEEP
};

State currentState = IDLES;


unsigned long timerStartTime = 0;
unsigned long alarmStartTime = 0;
bool motionDetected = false;
String correctPassword = "1111"; // Set your password here
String enteredPassword = "";

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const byte ROWS = 4; // 4x4 keypad
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'0', 'F', 'E', 'D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3}; //row pins of the keypad (r4 -> pin(A0), r3 -> pin(A1) ..)
byte colPins[COLS] = {A4, A5, A7, A8}; //column pins of the keypad (c4 -> pin(A4), c3 -> pin(A5) ..)
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  pinMode(SHARP_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(ACTIVATION_BUTTON, INPUT_PULLUP);

  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("System Ready");
  delay(1000);
}

void loop() {
  switch (currentState) {
    case IDLES:
      handleIDLESState();
      break;

    case TIMER_ON:
      handleTimerOnState();
      break;

    case ALARM_ACTIV:
      handleAlarmActivState();
      break;

    case SLEEP:
      handleSleepState();
      break;
  }
}

void handleIDLESState() {
  // int sharpValue = analogRead(SHARP_SENSOR_PIN);
  // motionDetected = (sharpValue > 300 && sharpValue < 700); // Adjust range based on sensor

  float volts = analogRead(SHARP_SENSOR_PIN)*0.0048828125;  // value from sensor * (5/1024)
  int distance = 13 * pow(volts, -1); // worked out from datasheet graph
  Serial.print("Current distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(200);
  motionDetected = (distance < 7);


  if (motionDetected) {
    currentState = TIMER_ON;
    timerStartTime = millis();
    lcd.clear();
    lcd.print("Motion Detected");
  }
}

void handleTimerOnState() {
  int sharpValue = analogRead(SHARP_SENSOR_PIN);
  motionDetected = (sharpValue > 300 && sharpValue < 700);

  if (!motionDetected) {
    currentState = IDLES;
    enteredPassword ="";
    lcd.clear();
    lcd.print("System Ready");
    return;
  }

  char key = keypad.getKey();
  if (key) {
    enteredPassword += key;
    lcd.setCursor(0, 1);
    lcd.print(enteredPassword);

    if (enteredPassword.length() == correctPassword.length()) {
      delay(500);
      if (enteredPassword == correctPassword) {
        currentState = SLEEP;
        enterSleepMode();
        return;
      } else {
        lcd.clear();
        lcd.print("Wrong Password");
        delay(2000);
        enteredPassword = "";
      }
    }
  }

  if (millis() - timerStartTime > TIMER_DURATION) {
    currentState = ALARM_ACTIV;
    enterAlarmMode();
  }
}

void handleAlarmActivState() {
  char key = keypad.getKey();
  if (key) {
    enteredPassword += key;
    lcd.setCursor(0, 1);
    lcd.print(enteredPassword);

    if (enteredPassword.length() == correctPassword.length()) {
      if (enteredPassword == correctPassword) {
        currentState = SLEEP;
        stopAlarm();
        enterSleepMode();
        return;
      } else {
        lcd.clear();
        lcd.print("Wrong Password");
        delay(2000);
        enteredPassword = "";
      }
    }
  }

  if (millis() - alarmStartTime > ALARM_AUTO_STOP_TIME) {
    currentState = IDLES;
    stopAlarm();
    lcd.print("System Ready");
  }
}

void handleSleepState() {
  lcd.setCursor(0, 0);
  lcd.print("System in Sleep");
  if (digitalRead(ACTIVATION_BUTTON) == LOW) {
    lcd.clear();
    lcd.print("Reactivating...");
    delay(REACTIVATION_DELAY); // reactivate the system after five minits 
    currentState = IDLES;
    lcd.clear();
    lcd.print("System Ready");
  }
}
////////////////////////////////////

void enterSleepMode() {
  lcd.clear();
  lcd.print("Entering Sleep");
  delay(2000);
  lcd.clear();
  lcd.print("System in Sleep");
}

void enterAlarmMode() {
  alarmStartTime = millis();
  digitalWrite(RED_LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000);
  lcd.clear();
  lcd.print("Alarm Activated!");
}

void stopAlarm() {
  digitalWrite(RED_LED_PIN, LOW);
  noTone(BUZZER_PIN);
  lcd.clear();
  lcd.print("Alarm Stopped");
  delay(2000);
}
