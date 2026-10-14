#include <Adafruit_NeoPixel.h> 

#define LED 38
#define PACKAGE_SENSOR 1
Adafruit_NeoPixel pixels(1, LED, NEO_GRB + NEO_KHZ800);

bool go = false;

void setup() {
  pixels.begin();

  pinMode(PACKAGE_SENSOR, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  pixels.clear();

  int package_sensor_value = analogRead(PACKAGE_SENSOR);

  if (package_sensor_value <= 400) {
    pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    pixels.show();
  } else {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
  }
}
