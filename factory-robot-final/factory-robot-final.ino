#include <Adafruit_NeoPixel.h> 

#define IN1 36
#define IN2 35
#define IN3 48
#define IN4 47
#define MTRSPD_A 37
#define MTRSPD_B 21

#define IR1 14
#define IR2 13
#define IR3 12
#define IR4 11
#define IR5 10

#define PACKAGE_SENSOR 1

#define TRIG 7
#define ECHO 6

#define KP 30
#define KD 150

#define WEIGHT_HIGH 8
#define WEIGHT_LOW 3

#define START_SPD 700
#define MIN_SPD 0
#define BASE_SPD 600
#define MAX_SPD 1023

typedef enum {
  ROBOT_START,
  ROBOT_STOP,
  ROBOT_FOLLOW_LINE,
  ROBOT_OBSTACLE_DETECTED
} RobotState;

Adafruit_NeoPixel pixels(1, 38, NEO_GRB + NEO_KHZ800);

RobotState robot_state = ROBOT_STOP;
RobotState last_robot_state = ROBOT_STOP;

bool obstacle_detected = false;
float duration = 0;
float distance = 0;
int last_millis = 0;

bool package_detected = false;
int package_sensor_threshold = 400;

int error = 0;
int derivitive = 0;
int correction = 0;

int last_error = 0;

int readSensors() {
  int ir1 = !digitalRead(IR1);
  int ir2 = !digitalRead(IR2);
  int ir4 = !digitalRead(IR4);
  int ir5 = !digitalRead(IR5);

  return (-WEIGHT_HIGH * ir1) + (-WEIGHT_LOW * ir2) + (WEIGHT_LOW * ir4) + (WEIGHT_HIGH * ir5);
}

void setup() {
  Serial.begin(115200);
  pixels.begin();

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);

  pinMode(PACKAGE_SENSOR, INPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  pinMode(MTRSPD_A, OUTPUT);
  pinMode(MTRSPD_B, OUTPUT);

  ledcAttach(MTRSPD_A, 20000, 10);
  ledcAttach(MTRSPD_B, 20000, 10);
}

void loop() {
  pixels.clear();

  int package_sensor_value = analogRead(PACKAGE_SENSOR);

  if (package_sensor_value <= package_sensor_threshold) {
    package_detected = true;
    package_sensor_threshold = 500;
  } else {
    package_detected = false;
    package_sensor_threshold = 300;
  }

  if (millis() - last_millis >= 100) {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    duration = pulseIn(ECHO, HIGH);
    distance = (duration * 0.0343) / 2;

    obstacle_detected = distance <= 15;

    last_millis = millis();
  }

  if (obstacle_detected) {
    robot_state = ROBOT_OBSTACLE_DETECTED;
  } else if (package_detected && !obstacle_detected) {
    robot_state = ROBOT_START;
  } else if (robot_state == ROBOT_START && last_robot_state != ROBOT_STOP) {
    robot_state = ROBOT_FOLLOW_LINE;
  } else {
    robot_state = ROBOT_STOP;
  }

  switch (robot_state) {
    case ROBOT_STOP:
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      pixels.show();

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);

      break;

    case ROBOT_START:
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
      pixels.show();

      delay(1000);

      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);

      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);

      ledcWrite(MTRSPD_A, START_SPD);
      ledcWrite(MTRSPD_B, START_SPD);

      delay(100);

      break;

    case ROBOT_FOLLOW_LINE:
      error = readSensors();

      derivitive = error - last_error;

      correction = (KP * error) + (KD * derivitive);

      last_error = error;

      ledcWrite(MTRSPD_A, constrain(BASE_SPD - correction, MIN_SPD, MAX_SPD));
      ledcWrite(MTRSPD_B, 0.8 * constrain(BASE_SPD + correction, MIN_SPD, MAX_SPD));

      if (error == -WEIGHT_HIGH - WEIGHT_LOW) {
        ledcWrite(MTRSPD_B, 0);
      } else if (error == WEIGHT_HIGH + WEIGHT_LOW) {
        ledcWrite(MTRSPD_A, 0);
      }

      break;

    case ROBOT_OBSTACLE_DETECTED:
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      pixels.show();

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);

      while (distance <= 15) {
        delay(100);

        digitalWrite(TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);

        duration = pulseIn(ECHO, HIGH);
        distance = (duration * 0.0343) / 2;
      }

      break;
  }

  last_robot_state = robot_state;
}