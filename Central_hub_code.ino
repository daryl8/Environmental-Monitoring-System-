#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <WiFi.h>         // WiFi library
#include <HTTPClient.h>  // HTTPClient library

SoftwareSerial hc12Serial(16, 17);
LiquidCrystal_I2C lcd(0x27, 20, 4);

const char *ssid = "Galaxy A21sAC3A";                // WiFi SSID
const char *password = "mmwb9669";                        // WiFi password
const char *ENDPOINT = "http://192.168.108.112:8000/data"; // API endpoint


const int tempHumidityRelayPin = 2; // Pin connected to temperature/humidity relay
const int gasRelayPin = 4;          // Pin connected to gas relay
const int soundRelayPin = 5;        // Pin connected to sound relay

void setup() {
  Serial.begin(9600);
  hc12Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  // Display startup message
  lcd.setCursor(0, 1);
  lcd.print("    Environmental");
  lcd.setCursor(0, 2);
  lcd.print("  Monitoring System");
  delay(5000); // Wait for 5 seconds

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Clear the LCD screen after displaying startup message
  lcd.clear();

  // Set relay pins as output
  pinMode(tempHumidityRelayPin, OUTPUT);
  pinMode(gasRelayPin, OUTPUT);
  pinMode(soundRelayPin, OUTPUT);
}

void loop() {
  if (hc12Serial.available()) {
    String receivedData = hc12Serial.readStringUntil('\n');
    Serial.println("Received: " + receivedData);

    for (int i = 0; i < receivedData.length(); i++) {
      if (!isPrintable(receivedData.charAt(i))) {
        receivedData.setCharAt(i, ' ');
      }
    }

    // Extract sensor data and print on LCD
    printSensorData(receivedData);

    // Make HTTP POST request with received data
    sendHttpPost(receivedData);
  }
}

bool isPrintable(char c) {
  return c >= 32 && c <= 126;
}

void printSensorData(String data) {
  // Temperature/Humidity
  int tempHumidityIndex = data.indexOf("Temp:");
  if (tempHumidityIndex != -1) {
    String tempHumidityValue = data.substring(tempHumidityIndex + 5, data.indexOf(",", tempHumidityIndex));
    lcd.setCursor(0, 0);
    lcd.print("Temp: " + tempHumidityValue + (char)223 + "C");

    // Check temperature/humidity threshold and control relay
    int tempHumidityThreshold = 30; // Example threshold, replace with your desired value
    int tempHumidity = tempHumidityValue.toInt();
    if (tempHumidity > tempHumidityThreshold) {
      digitalWrite(tempHumidityRelayPin, HIGH); // Activate relay
    } else {
      digitalWrite(tempHumidityRelayPin, LOW); // Deactivate relay
    }
  }

  // Humidity
  int humidityIndex = data.indexOf("Humidity:");
  if (humidityIndex != -1) {
    String humidityValue = data.substring(humidityIndex + 9, data.indexOf(",", humidityIndex));
    lcd.setCursor(0, 1);
    lcd.print("Humidity: " + humidityValue + "%");

    // Check humidity threshold and control relay
    int humidityThreshold = 60; // Example threshold, replace with your desired value
    int humidity = humidityValue.toInt();
    if (humidity > humidityThreshold) {
      digitalWrite(tempHumidityRelayPin, HIGH); // Activate relay
    } else {
      digitalWrite(tempHumidityRelayPin, LOW); // Deactivate relay
    }
  }

  // Gas
  int gasIndex = data.indexOf("Gas:");
  if (gasIndex != -1) {
    String gasValue = data.substring(gasIndex + 4, data.indexOf(",", gasIndex));
    lcd.setCursor(0, 2);
    lcd.print("Gas: " + gasValue + " ppm");

    // Check gas threshold and control relay
    int gasThreshold = 50; // Example threshold, replace with your desired value
    int gas = gasValue.toInt();
    if (gas > gasThreshold) {
      digitalWrite(gasRelayPin, HIGH); // Activate relay
    } else {
      digitalWrite(gasRelayPin, LOW); // Deactivate relay
    }
  }

  // Sound
  int soundIndex = data.indexOf("Sound:");
  if (soundIndex != -1) {
    String soundValue = data.substring(soundIndex + 6);
    lcd.setCursor(0, 3);
    lcd.print("Sound: " + soundValue + " dB");

    // Check sound threshold and control relay
    int soundThreshold = 125; // Example threshold, replace with your desired value
    int sound = soundValue.toInt();
    if (sound > soundThreshold) {
      digitalWrite(soundRelayPin, HIGH); // Activate relay
    } else {
      digitalWrite(soundRelayPin, LOW); // Deactivate relay
    }
  }
}


void sendHttpPost(String data) {
  HTTPClient http;

  // Construct JSON payload
  String payload = "{\"data\":\"" + data + "\"}";

  // Make HTTP POST request
  http.begin(ENDPOINT);
  http.addHeader("Content-Type", "application/json");
  

  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.println("Response from server: " + response);
  } else {
    Serial.print("Error Code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
