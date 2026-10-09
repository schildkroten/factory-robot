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

#define TRIG 18
#define ECHO 17

#define KP 30
#define KD 150

#define WEIGHT_HIGH 8
#define WEIGHT_LOW 3

#define START_SPD 700
#define MIN_SPD 0
#define BASE_SPD 600
#define MAX_SPD 1023

Adafruit_NeoPixel pixels(1, 38, NEO_GRB + NEO_KHZ800);

bool go = true;
bool last_go = false;

float duration = 0;
float distance = 0;

int package_sensor_threshold = 400;

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

  // pinMode(TRIG, OUTPUT);
  // pinMode(ECHO, INPUT);

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
  Serial.println(package_sensor_value);

  if (package_sensor_value <= package_sensor_threshold) {
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();
    
    go = true;

    package_sensor_threshold = 500;
  } else {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();

    go = false;

    package_sensor_threshold = 300;
  }

  // digitalWrite(TRIG, LOW);
  // delayMicroseconds(2);
  // digitalWrite(TRIG, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(TRIG, LOW);

  // duration = pulseIn(ECHO, HIGH);
  // distance = (duration * 0.0343) / 2;

  // Serial.println(distance);

  // go = distance < 15 ? false : true;

  if (go && !last_go) {
    delay(1000);

    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    ledcWrite(MTRSPD_A, START_SPD);
    ledcWrite(MTRSPD_B, START_SPD);

    delay(100);

    last_go = true;
  } else if (!go) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);

    last_go = false;

    return;
  }

  int error = readSensors();

  int derivitive = error - last_error;

  int correction = (KP * error) + (KD * derivitive);

  last_error = error;

  ledcWrite(MTRSPD_A, constrain(BASE_SPD - correction, MIN_SPD, MAX_SPD));
  ledcWrite(MTRSPD_B, 0.8 * constrain(BASE_SPD + correction, MIN_SPD, MAX_SPD));

  if (error == -WEIGHT_HIGH - WEIGHT_LOW) {
    ledcWrite(MTRSPD_B, 0);
  } else if (error == WEIGHT_HIGH + WEIGHT_LOW) {
    ledcWrite(MTRSPD_A, 0);
  }
}
