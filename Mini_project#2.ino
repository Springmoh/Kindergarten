#define BUTTON 2  //assign pin 2 to the button
int flag = 1;
int warmingup = 1;

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
  
}

void loop() {
  if(warmingup){
    Serial.println("LED13 on. Running LEDs - Rightward.");
    warmingup = 0;
  }
  if (!digitalRead(BUTTON)) {
    while (!digitalRead(BUTTON)) {
      delay(20);
    }
    flag = !flag;
    if (flag) {
      Serial.println("LED13 on. Running LEDs - Rightward.");
    } else {
      Serial.println("LED13 off. Running LEDs - Leftward.");
    }
  }

  if (flag) {
    digitalWrite(13, 1);
    for (int i = 4; i <= 7; i++) {
      digitalWrite(i, 1);
      delay(200);
      digitalWrite(i, 0);
      if (!digitalRead(BUTTON)) {
        break;
      }
    }
  } else {
    digitalWrite(13, 0);
    for (int i = 7; i >= 4; i--) {
      digitalWrite(i, 1);
      delay(200);
      digitalWrite(i, 0);
      if (!digitalRead(BUTTON)) {
        break;
      }
    }
  }
}
