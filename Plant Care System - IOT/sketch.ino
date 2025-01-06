#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
// const char* serverName = "http://server-name.com/get-endpoint";

int servoPins[] = {18, 19, 21, 22}; // Servo pins
int minPulseWidth = 1000;   // Minimum pulse width in microseconds
int maxPulseWidth = 2000;   // Maximum pulse width in microseconds
int pulseWidth;             // Current pulse width

float luxValue = 0.0;
float soilMoistureValue = 0.0;
float temperatureValue = 0.0;
float humidityValue = 0.0;
bool increasing = true;

bool device1 = true;
bool device2 = false;

void setup() {
  Serial.begin(115200);

  // Servo pins to output
  for (int i = 0; i < 4; i++) {
    pinMode(servoPins[i], OUTPUT);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Setup completed");
}

void loop() {
  if (device1) {
    if (increasing) {
      luxValue += 5;
      soilMoistureValue += 3;
      temperatureValue += 2;
      humidityValue += 4;
      if (luxValue >= 500) {
        increasing = false;
      }
    } else {
      luxValue -= 5;
      soilMoistureValue -= 3;
      temperatureValue -= 2;
      humidityValue -= 4;
      if (luxValue <= 0) {
        increasing = true;
      }
    }

    Serial.print("Simulated Lux Value: ");
    Serial.println(luxValue);
    Serial.print("Simulated Soil Moisture Value: ");
    Serial.println(soilMoistureValue);
    Serial.print("Simulated Temperature Value: ");
    Serial.println(temperatureValue);
    Serial.print("Simulated Humidity Value: ");
    Serial.println(humidityValue);

    controlServo(servoPins[0], luxValue);         
    controlServo(servoPins[1], soilMoistureValue); 
    controlServo(servoPins[2], temperatureValue);
    controlServo(servoPins[3], humidityValue); 
  }

  delay(1000);
}

void controlServo(int pin, float value) {
  int angle;
  if (value < 100) {
    angle = 0; 
  } else if (value >= 100 && value < 300) {
    angle = 90; 
  } else {
    angle = 180; 
  }

  // PWM
  pulseWidth = map(angle, 0, 180, minPulseWidth, maxPulseWidth);
  for (int i = 0; i < 50; i++) { // 50 Hz PWM
    digitalWrite(pin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(pin, LOW);
    delayMicroseconds(20000 - pulseWidth);
  }
}
