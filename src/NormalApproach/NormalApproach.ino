#include <Wire.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

// Pin Definitions
#define SHARP_SENSOR_PIN A0
#define BUZZER_PIN 25
#define RED_LED_PIN 26
#define SENSOR_SLEEP_BUTTON 33

// Constants
#define ALARM_TIME_LIMIT 10000 // 10 seconds
#define ALARM_AUTO_STOP_TIME 300000 // 5 minutes

// Variables
bool sharpActive = true;
unsigned long motionTimer = 0;
unsigned long alarmStartTime = 0;
bool alarmActive = false;
bool keypadActive = false;
String correctPassword = "1234"; // Change this to set a custom password
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
  pinMode(SHARP_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(SENSOR_SLEEP_BUTTON, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.print("System Ready");
}

void loop() {
  if (sharpActive) {
    checkMotion();
  }

  handleKeypadInput();
  handleAlarm();
  checkSleepButton();
}

void checkMotion() {
  int sharpValue = analogRead(SHARP_SENSOR_PIN);

  // Check if motion is detected within range
  if (sharpValue > 300 && sharpValue < 700) { // Adjust range based on your sensor's characteristics
    if (motionTimer == 0) {
      motionTimer = millis(); // Start the timer
    }
    keypadActive = true;

    lcd.setCursor(0, 1);
    lcd.print("Motion detected!");

    if (millis() - motionTimer > ALARM_TIME_LIMIT && !alarmActive) {
      startAlarm();
    }
  } else {
    motionTimer = 0;
    keypadActive = false;
    lcd.setCursor(0, 1);
    lcd.print("                "); // Clear line
  }
}

void handleKeypadInput() {
  if (keypadActive || alarmActive) {
    char key = keypad.getKey();
    if (key) {
      enteredPassword += key;
      lcd.setCursor(0, 1);
      lcd.print(enteredPassword);

      if (enteredPassword.length() == correctPassword.length()) {
        if (enteredPassword == correctPassword) {
          stopAlarm();
          resetSystem();
        } else {
          lcd.setCursor(0, 1);
          lcd.print("Wrong password!");
          delay(2000);
          enteredPassword = "";
        }
      }
    }
  }
}

void startAlarm() {
  alarmActive = true;
  alarmStartTime = millis();
  digitalWrite(RED_LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000); // Start buzzer
  lcd.setCursor(0, 1);
  lcd.print("Alarm Activated!");
}

void stopAlarm() {
  alarmActive = false;
  digitalWrite(RED_LED_PIN, LOW);
  noTone(BUZZER_PIN);
  lcd.setCursor(0, 1);
  lcd.print("Alarm Stopped!  ");
}

void checkSleepButton() {
  if (digitalRead(SENSOR_SLEEP_BUTTON) == LOW) {
    sharpActive = !sharpActive;
    lcd.clear();
    lcd.print(sharpActive ? "Sensor Active" : "Sensor Sleep");
    delay(500); // Debounce
  }
}

void handleAlarm() {
  if (alarmActive && (millis() - alarmStartTime > ALARM_AUTO_STOP_TIME)) {
    stopAlarm();
  }
}

void resetSystem() {
  sharpActive = false;
  motionTimer = 0;
  alarmActive = false;
  enteredPassword = "";
  lcd.clear();
  lcd.print("System Reset");
  delay(2000);
  lcd.clear();
}
