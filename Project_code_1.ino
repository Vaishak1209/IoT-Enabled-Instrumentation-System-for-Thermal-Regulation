#define BLYNK_TEMPLATE_ID "TMPL3Vd5jKble"
#define BLYNK_TEMPLATE_NAME "IOT 2"
#define BLYNK_AUTH_TOKEN "P3rgiV4ARYqMvyhFakd5E9ckKpAW-Pj0"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// Blynk and Wi-Fi configuration
#define BLYNK_PRINT Serial
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Vaishak's Galaxy A52s 5G";
char pass[] = "jyky7928";

// LCD and sensor setup
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define ONE_WIRE_BUS 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;  // Create a timer object

// Variables and settings
int fanPin = D0;  // Connect the MOSFET gate to D0 (GPIO16) on the ESP8266
float temp = 0;
int threshold = 30;
bool fanState = false;
WidgetLED FAN(V0);

void setup() {
  Serial.begin(115200);
  
  // Blynk setup
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 8080);
  
  // Sensor and LCD setup
  sensors.begin();
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, LOW);  // Ensure fan is off initially
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  Temperature ");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring System");
  delay(4000);
  lcd.clear();

  // Set up a timer to call sendSensorData every 2 seconds (2000 ms)
  timer.setInterval(2000L, sendSensorData);
}

BLYNK_WRITE(V7) {
  threshold = param.asInt();
  Serial.print("Threshold value is: ");
  Serial.println(threshold);
}

void controlFan(bool state) {
  fanState = state;
  digitalWrite(fanPin, state ? HIGH : LOW);  // Turn the fan on or off

  Serial.print("Fan State: ");
  Serial.println(state ? "ON" : "OFF");
  
  lcd.setCursor(0, 1);
  lcd.print("Fan: ");
  lcd.print(state ? "ON " : "OFF");
}

void sendSensorData() {
  // Get temperature reading
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("*C");
  
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print("*C  ");
  
  // Send temperature to Blynk
  Blynk.virtualWrite(V3, temp);

  if (temp >= threshold) {
    FAN.on();
    controlFan(true);  // Turn fan on if temperature exceeds threshold
  } else {
    FAN.off();
    controlFan(false);  // Turn fan off if temperature is below threshold
  }
  
  Blynk.virtualWrite(V4, fanState ? 100 : 0);  // Send fan state to Blynk
}

void loop() {
  Blynk.run();
  timer.run();  // Run the timer to keep checking intervals
}
