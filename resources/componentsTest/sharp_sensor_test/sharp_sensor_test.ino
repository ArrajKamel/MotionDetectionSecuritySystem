#define sensor A9 // Sharp IR GP2Y0A41SK0F (4-30cm, analog)

unsigned long timerStart = 0; // Variable to track the start of the timer
bool timerRunning = false;    // Flag to indicate if the timer is running

const int redLEDPin = 23;
const int yellowLEDPin = 22;

void setup() {
  Serial.begin(9600);
    // Initialize LEDs
  pinMode(redLEDPin, OUTPUT);
  pinMode(yellowLEDPin , OUTPUT);
  
  // Turn on the red LED at startup
  digitalWrite(redLEDPin, LOW);
  digitalWrite(yellowLEDPin, LOW);

}

void loop() {
  // 5v
  float volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
  int distance = 13*pow(volts, -1); // worked out from datasheet graph
  delay(200); // slow down serial port 
  
  if (distance <= 80){
    Serial.println(distance);   // print the distance
  }

  if(distance <= 7){
    if (!timerRunning) {
      timerStart = millis(); // Start the timer
      timerRunning = true;
      Serial.println("Timer started");
    }
    delay(1000);

    if (millis() - timerStart >= 5000) { // Check if 5 seconds have passed
      Serial.println("5 seconds elapsed, triggering interrupt action");
      Serial.println("the alarm is ringing");
      digitalWrite(yellowLEDPin, HIGH);
      enterThePassword();
      timerRunning = false; 
    }

  }else { // Reset the timer if the condition is not met
    timerRunning = false; 
    timerStart = 0;
  }
  
}

void enterThePassword(){
  // TO_DO next lab (20/12/.24)
}
