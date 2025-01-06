#define BLYNK_TEMPLATE_ID  "TMPL6kESKT1_f"
#define BLYNK_TEMPLATE_NAME "Esp32_Environment_recorder"
#define BLYNK_AUTH_TOKEN   "xRt4wOSffGfpuPf3x43kpxHPUvcv05_r"

#include "DHTesp.h"
#include <RTClib.h>
#include <Wire.h>
#include <WiFi.h> 
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

const char* ssid = "Wokwi-GUEST"; 
const char* password = ""; 
//const char* serverName = "http://server-name.com/post-endpoint";


char auth[] = BLYNK_AUTH_TOKEN;

#define DHT_PIN 15
#define SOIL_PIN 34
#define PHOTORESISTOR_PIN 35

const float GAMMA = 0.7;
const float RL10 = 50;

DHTesp dhtSensor;
RTC_DS1307 rtc;

void setup()
{
  Serial.begin(9600);
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  delay(2000); // Wait for stabilization
  pinMode(PHOTORESISTOR_PIN, INPUT);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: "); //x
  Serial.println(WiFi.localIP()); //x

  Wire.begin(4, 22);
  if (!rtc.begin()) { 
    Serial.println("Couldn't find RTC"); 
    while (1);
  }
  if (!rtc.isrunning()) { 
    Serial.println("RTC is NOT running!");
  }

  // Blynk
  Blynk.begin(auth, ssid, password);

  Serial.println("Setup completed");
}

void loop()
{
  // Date and Time
  DateTime now = rtc.now();
  String date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()); 
  String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  Serial.print("Date: "); 
  Serial.print(date); 
  Serial.print(" Time: "); 
  Serial.println(time);
  // Soil Moisture
  int16_t soil_moisture = analogRead(SOIL_PIN)+1;
  Serial.print("Soil Value: ");
  Serial.println(soil_moisture);

  // Temperature & Humidity:
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  String temperature = String(data.temperature, 2);
  String humidity = String(data.humidity, 1);
  Serial.println("Temp: " + temperature + "°C");
  Serial.println("Humidity: " + humidity + "%");

  // Lux
  int luxValue = analogRead(PHOTORESISTOR_PIN);
  float voltage = luxValue / 4096. * 5;
  float resistance = 2000 * voltage / (1 - voltage / 5);
  float lux_a = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
  Serial.print("Analog: Lux ");
  Serial.println(lux_a);
  delay(500);

  //Wifi
  //if (WiFi.status() == WL_CONNECTED) { 
  //  HTTPClient http; http.begin(serverName); 
  //  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  //  
  //  String postData = "date=" + date + "&time=" + time + "&lux=" + String(lux_a); 
  //  int httpResponseCode = http.POST(postData); 
  //  
  //  if (httpResponseCode > 0) { 
  //    String response = http.getString(); 
  //    Serial.println(httpResponseCode); 
  //    Serial.println(response); 
  //  }
  //  else {
  //    Serial.print("Error on sending POST: "); 
  //    Serial.println(http.errorToString(httpResponseCode).c_str());
  //} 
  //  http.end(); 
  //} else { 
  //  Serial.println("WiFi Disconnected"); 
  //}

  Blynk.virtualWrite(V1, date);
  Blynk.virtualWrite(V2, time);
  Blynk.virtualWrite(V3, soil_moisture);
  Blynk.virtualWrite(V4, temperature);
  Blynk.virtualWrite(V5, humidity);
  Blynk.virtualWrite(V6, lux_a);

  delay(10000);
}
