#include <Adafruit_NeoPixel.h> 

#define IN1 36
#define IN2 35
#define IN3 48
#define IN4 47

#define MTRSPD_A 37
#define MTRSPD_B 21

#define BASE_SPD 172

#define PACKAGE_SENSOR 1
Adafruit_NeoPixel pixels(1, 38, NEO_GRB + NEO_KHZ800);

bool go = false;

void setup() {
  pixels.begin();

  pinMode(PACKAGE_SENSOR, INPUT);

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
}

void loop() {
  pixels.clear();

  int package_sensor_value = analogRead(PACKAGE_SENSOR);

  if (package_sensor_value <= 400) {
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();

    go = true;
  } else {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();

    go = false;
  }

  if (!go) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, HIGH);

    return;
  }

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
}
