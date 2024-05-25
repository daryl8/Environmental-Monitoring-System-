#include <SoftwareSerial.h>

#define MQ7_PIN A0  // Analog pin to which MQ-7 sensor is connected
SoftwareSerial hc12Serial(8, 9);  // HC-12 connected to pin 8 (RX) and pin 9 (TX)

void setup() {
  Serial.begin(19200);
  hc12Serial.begin(19200);
}

void loop() {
  // Read gas data from MQ-7 sensor
  int gasValue = analogRead(MQ7_PIN);
  
  // Map the gas value from the range of the sensor to the desired range (10-80 ppm)
  int ppmValue = map(gasValue, 0, 1023, 10, 80);

  // Transmit gas data via HC-12
  hc12Serial.print("Gas: ");
  hc12Serial.println(ppmValue);

  delay(2000);  // Adjust delay as needed
}
