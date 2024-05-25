#include <SoftwareSerial.h>
#include <DHT.h>

#define DHT_PIN 4  // Define the pin to which the DHT22 is connected
#define DHT_TYPE DHT22

SoftwareSerial hc12Serial(8, 9);  // HC-12 connected to pin 8 (RX) and pin 9 (TX)
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(19200);
  hc12Serial.begin(19200);
  dht.begin();
}

void loop() {
  delay(2000);  // Wait for DHT22 sensor to stabilize

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Transmit formatted data via HC-12
  hc12Serial.print("Temp: ");
  hc12Serial.print(temperature);
  hc12Serial.println(); 
  hc12Serial.print(" Humidity: ");
  hc12Serial.print(humidity);
  hc12Serial.println();  // Add a newline character to separate transmissions
}
