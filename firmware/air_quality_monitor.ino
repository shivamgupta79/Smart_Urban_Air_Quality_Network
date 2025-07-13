#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

#define MQ135_PIN 34  // ADC pin for MQ135

// PMS7003 Serial
HardwareSerial pmsSerial(2);
#define PMS_RX 16
#define PMS_TX 17


const char* ssid = "Tute";
const char* password = "dknx1909";

// n8n webhook URL
const char* webhookUrl = "https://n8n-webhook-url.com/webhook/air-quality";

// BME680
Adafruit_BME680 bme;

// Buffer for PMS7003
uint8_t pmsBuffer[32];


int calculateAQI_PM25(int pm25) {
  if (pm25 <= 50) return 50;
  else if (pm25 <= 100) return 100;
  else if (pm25 <= 150) return 150;
  else if (pm25 <= 200) return 200;
  else return 300;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  
  pmsSerial.begin(9600, SERIAL_8N1, PMS_RX, PMS_TX);

  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected ✅");

  
  if (!bme.begin()) {
    Serial.println("❌ BME680 not found!");
    while (1);
  }
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setGasHeater(320, 150); 
}

void loop() {
  int pm25 = 0, pm10 = 0;

  // Read PMS7003
  if (pmsSerial.available() >= 32) {
    pmsSerial.readBytes(pmsBuffer, 32);
    if (pmsBuffer[0] == 0x42 && pmsBuffer[1] == 0x4D) {
      pm25 = pmsBuffer[12] << 8 | pmsBuffer[13];
      pm10 = pmsBuffer[14] << 8 | pmsBuffer[15];
    }
  }

  // Read MQ135
  int mq135Value = analogRead(MQ135_PIN);

  // Read BME680
  float temperature = 0, humidity = 0, gas = 0;
  if (bme.performReading()) {
    temperature = bme.temperature;
    humidity = bme.humidity;
    gas = bme.gas_resistance / 1000.0; 
  }


  int aqi = calculateAQI_PM25(pm25);

  Serial.println("📡 Sensor Readings:");
  Serial.printf("PM2.5: %d µg/m³, PM10: %d µg/m³\n", pm25, pm10);
  Serial.printf("Gas (MQ135): %d, Temp: %.2f°C, Humidity: %.2f%%, VOC: %.2f kΩ\n", mq135Value, temperature, humidity, gas);
  Serial.printf("AQI: %d\n", aqi);


  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(webhookUrl);
    http.addHeader("Content-Type", "application/json");

    String payload = "{";
    payload += "\"device_id\":\"node01\",";
    payload += "\"location\":\"Sector-62\",";
    payload += "\"pm25\":" + String(pm25) + ",";
    payload += "\"pm10\":" + String(pm10) + ",";
    payload += "\"gas\":" + String(mq135Value) + ",";
    payload += "\"temperature\":" + String(temperature) + ",";
    payload += "\"humidity\":" + String(humidity) + ",";
    payload += "\"voc\":" + String(gas) + ",";
    payload += "\"aqi\":" + String(aqi);
    payload += "}";

    int responseCode = http.POST(payload);
    if (responseCode > 0) {
      Serial.println("✅ Data sent to n8n");
    } else {
      Serial.printf("❌ Error sending data: %d\n", responseCode);
    }
    http.end();
  }

  delay(60000);
}
