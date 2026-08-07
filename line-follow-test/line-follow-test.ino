#define IN1 20
#define IN2 10
#define IN3 7
#define IN4 6
#define MTRSPD_A 21
#define MTRSPD_B 5

#define IR1 4
#define IR2 3
#define IR3 2
#define IR4 1
#define IR5 0

#define BASE_SPD 96
#define TURN_SPD 70

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);

  pinMode(MTRSPD_A, OUTPUT);
  pinMode(MTRSPD_B, OUTPUT);

  ledcAttach(MTRSPD_A, 5000, 8);
  ledcAttach(MTRSPD_B, 5000, 8);

  ledcWrite(MTRSPD_A, BASE_SPD);
  ledcWrite(MTRSPD_B, BASE_SPD);

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);
}

void stop() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
}

void forward(uint time) {
  ledcWrite(MTRSPD_A, BASE_SPD);
  ledcWrite(MTRSPD_B, BASE_SPD);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(time);

  stop();
}

void backward(uint time) {
  ledcWrite(MTRSPD_A, BASE_SPD);
  ledcWrite(MTRSPD_B, BASE_SPD);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  delay(time);

  stop();
}

void leftTurn(uint time) {
  ledcWrite(MTRSPD_A, BASE_SPD);
  ledcWrite(MTRSPD_B, TURN_SPD);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(time);

  stop();
}

void rightTurn(uint time) {
  ledcWrite(MTRSPD_A, TURN_SPD);
  ledcWrite(MTRSPD_B, BASE_SPD);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(time);

  stop();
}

void loop() {
  if (digitalRead(IR4) == LOW) {
    rightTurn(100);
  } else if (digitalRead(IR2) == LOW) {
    leftTurn(100);
  }

  forward(25);
}
