#define sensor A0 // Sharp IR GP2Y0A41SK0F (4-30cm, analog)

void setup() {
  Serial.begin(9600);

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
    int i = 5; 
    Serial.println("countin down:");
    for(i = 5 ; i > 0 ; i--){
      Serial.println(i);
      float volts = analogRead(sensor)*0.0048828125;  // value from sensor * (5/1024)
      int distance = 13*pow(volts, -1); // worked out from datasheet graph
      if(distance > 7)
        break; 
      delay(1000);
    }
    if(i == 0){
      Serial.println("the alarm is ringing");
      delay(3000);
    }
  }
}
