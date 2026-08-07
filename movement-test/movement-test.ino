/*
* Created by Angus Bonney 2026 for ESP32 Dev Kit C
* This script simply moves the robot forward, backward,
* left, and right.
*/

/* IN1 and IN2 control motor A direction. */
#define IN1 20
#define IN2 10

/* IN3 and IN4 control motor B direction. */
#define IN3 7
#define IN4 6

/* These two pins control the speed through PWM. */
#define MTRSPD_A 21
#define MTRSPD_B 5

/* The base speed for the motors. */
#define BASE_SPD 172

void setup() {
  /* Set the direction control pins to outputs. */
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  /* Make the motors stop. */
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);

  /* Set the speed control pins to outputs. */
  pinMode(MTRSPD_A, OUTPUT);
  pinMode(MTRSPD_B, OUTPUT);

  /*
  * Attach the speed control pins to the pwm generator
  * and set them to the base speed.
  */
  ledcAttach(MTRSPD_A, 5000, 8);
  ledcAttach(MTRSPD_B, 5000, 8);

  ledcWrite(MTRSPD_A, BASE_SPD);
  ledcWrite(MTRSPD_B, BASE_SPD);
}

void loop() {
  /* Go forward */
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(1000);

  /* Go backward */
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  delay(1000);

  /* Turn left */
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  delay(1000);

  /* Turn right */
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  delay(1000);

  /* Stop */
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);

  /*
  * Wait forever so the I can set it up
  * before starting the script by reseting.
  */
  while (1) {}
}
