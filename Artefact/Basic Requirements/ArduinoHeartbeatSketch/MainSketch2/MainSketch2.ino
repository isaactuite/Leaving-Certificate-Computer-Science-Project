#include "Wire.h"
#include "DFRobot_BloodOxygen_S.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define UPDATE_TIME   4000
byte heart[8] = {0b00000, 0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000};  // Heart symbol for LCD display
uint32_t previous_update_time = 0;
#define I2C_COMMUNICATION 

#ifdef  I2C_COMMUNICATION
#define I2C_ADDRESS    0x57
DFRobot_BloodOxygen_S_I2C MAX30102(&Wire, I2C_ADDRESS);
#else
#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
SoftwareSerial mySerial(4, 5);
DFRobot_BloodOxygen_S_SoftWareUart MAX30102(&mySerial, 9600);
#else
DFRobot_BloodOxygen_S_HardWareUart MAX30102(&Serial1, 9600); 
#endif
#endif

void on_pulse_detected() {
  Serial.println("Pulse Detected!");
}

void displayHeartbeatSpO2(int heartbeat, int spo2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("❤ Rate: ");
  lcd.print(heartbeat);
  lcd.print("bpm");

  lcd.setCursor(0, 1);
  lcd.print(" SpO2 : ");
  lcd.print(spo2);
  lcd.print("%");
}

void setup() {
  Serial.begin(9600);
  delay(1000); 
  lcd.createChar(2, heart);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  lcd.setCursor(0, 1);
  lcd.print("Pulse Oximeter");

  delay(3000);

  while (false == MAX30102.begin()) {
    Serial.println("init fail!");
    delay(1000);
  }

  Serial.println("init success!");
  Serial.println("start measuring...");
  MAX30102.sensorStartCollect();
}

void loop() {
  MAX30102.getHeartbeatSPO2();

  if (Serial.available() > 0) {
    // If data is available from Python, read and display it
    String receivedData = Serial.readStringUntil('\n');

    // Parse received data from Python
    // Example: Assuming data format "123,456,AnalyzedResult"
    int heartbeat = receivedData.substring(0, 3).toInt();
    int spo2 = receivedData.substring(4, 7).toInt();
    String analyzedResult = receivedData.substring(8);

    // Display the analyzed result on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(analyzedResult);

    delay(4000); // Display the analyzed result for 4 seconds

    // Resume displaying heartbeat and SpO2 data
    displayHeartbeatSpO2(heartbeat, spo2);
  }

  if (millis() - previous_update_time > UPDATE_TIME) {
    // Display heartbeat and SpO2 values
    displayHeartbeatSpO2(MAX30102._sHeartbeatSPO2.Heartbeat, MAX30102._sHeartbeatSPO2.SPO2);

    previous_update_time = millis();
  }
}