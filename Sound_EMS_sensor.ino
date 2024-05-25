#include <SoftwareSerial.h>

const int WINDOW_DURATION_MS = 50; // Sample window width in milliseconds (50 ms = 20Hz)
const int SOUND_PIN = A0;
const int HC12_RX = 8; // Rx
const int HC12_TX = 9; // Tx

SoftwareSerial hc12(HC12_RX, HC12_TX); // HC-12 connected to pins 8 (RX) and 9 (TX)

void setup() {
  pinMode(SOUND_PIN, INPUT); // Set the microphone pin as input

  Serial.begin(19200);
  hc12.begin(19200); // Set the baud rate for HC-12
}

void loop() {
  unsigned long windowStartTime = millis(); // Start of the sample window
  unsigned int maxValue = 0; // Initialize Maximum signal value
  unsigned int minValue = 1024; // Initialize Minimum signal value

  // Collect data for 50 ms
  while (millis() - windowStartTime < WINDOW_DURATION_MS) {
    unsigned int sensorValue = analogRead(SOUND_PIN); // Get reading from the microphone
    if (sensorValue < 1024) { // Check if the reading is within 10-bit ADC range, 0-1023
      if (sensorValue > maxValue) {
        maxValue = sensorValue; // Save the max levels
      }
      if (sensorValue < minValue) {
        minValue = sensorValue; // Save the min levels
      }
    }
  }

  unsigned int peakToPeakAmplitude = maxValue - minValue; // max - min = peak-to-peak amplitude

  // Map the peak-to-peak amplitude to the desired decibel range (37 to 130)
  int db = map(peakToPeakAmplitude, 20, 900, 37, 130); // Calibrate for decibels

  // Transmit sound data in decibels via HC-12
  hc12.print("Sound: ");
  hc12.println(db);

  delay(2000); // Delay before the next reading
}
