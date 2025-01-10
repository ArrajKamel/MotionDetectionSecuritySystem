#include <Keypad.h>
#include <LiquidCrystal.h>
#define sensor A9 // Sharp IR GP2Y0A41SK0F (4-30cm, analog)

unsigned long timerStart = 0; // Variable to track the start of the timer
bool timerRunning = false;    // Flag to indicate if the timer is running

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
const int redLEDPin = 23;
const int yellowLEDPin = 22;

String password = "1234DD";
String userInput = "";

byte row_pins[] = {A0, A1, A2, A3}; //row pins of the keypad (r4 -> pin(A0), r3 -> pin(A1) ..)
byte column_pins[] = {A4, A5, A7, A8}; //column pins of the keypad (c4 -> pin(A4), c3 -> pin(A5) ..)

//Declaration of the keys of the keypad
char hexaKeys[sizeof(row_pins) / sizeof(byte)][sizeof(column_pins) / sizeof(byte)] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'0', 'F', 'E', 'D'}
};


//define object for teh keypad
Keypad kypd = Keypad( makeKeymap(hexaKeys), row_pins,
                      column_pins, sizeof(row_pins) / sizeof(byte),
                      sizeof(column_pins) / sizeof(byte)); 


int counter = 0;  //variable to count keypresses
int column = 0; // Tracks the current column
int row = 0;    // Tracks the current row



void setup(){
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  
  // Initialize LEDs
  pinMode(redLEDPin, OUTPUT);
  pinMode(yellowLEDPin , OUTPUT);
  
  // Turn on the red LED at startup
  digitalWrite(redLEDPin, HIGH);
  digitalWrite(yellowLEDPin, LOW);
}


void loop()
{

  // 5v
  float volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
  int distance = 13*pow(volts, -1); // worked out from datasheet graph
  delay(200); // slow down serial port 

  char current_key = kypd.getKey(); // Get the pressed key

  if (current_key) { // If a key is pressed
    if (current_key == 'F') { // 'F' signals the end of input
      lcd.clear();
      if (userInput == password) {
        lcd.print("Access Granted");
        digitalWrite(redLEDPin, LOW);   // Turn off red LED
        digitalWrite(yellowLEDPin, HIGH); // Turn on yellow LED
      } else {
        lcd.print("Wrong Password");
        delay(2000); // Display error message for 2 seconds
        lcd.clear();
        lcd.print("Enter Password:");
      }
      userInput = ""; // Reset user input
    } else if (current_key == 'C') { // 'C' to clear input
      digitalWrite(redLEDPin, HIGH);   // Turn off red LED
      digitalWrite(yellowLEDPin, LOW); // Turn on yellow LED
      userInput = ""; // Clear the current input
      lcd.clear();
      lcd.print("Enter Password:");
    } else {
      userInput += current_key; // Append the key to the user input
      lcd.setCursor(0, 1); // Move to the second row
      lcd.print(userInput); // Display the user input
    }
  }

}