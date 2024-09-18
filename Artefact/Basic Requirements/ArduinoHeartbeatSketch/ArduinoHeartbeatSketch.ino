#include "Wire.h"
#include "DFRobot_BloodOxygen_S.h"
#include <LiquidCrystal.h>    //include relevant libraries

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);    //create instance of LiquidCrystal library for controlling LCD

#define UPDATE_TIME   500
byte heart[8] = {0b00000, 0b01010, 0b11111, 0b11111, 0b11111, 0b01110, 0b00100, 0b00000};    //This is for the Heart symbol on LCD display
uint32_t previous_update_time = 0;
#define I2C_COMMUNICATION 

#ifdef  I2C_COMMUNICATION                             //Built-in part of the initialization of max30201 sensor, allows choice between I2C and UART communication
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

void setup() { //Arduino setup function
  Serial.begin(9600);   //Initialize serial communication
  delay(1000); 
  lcd.createChar(2, heart); //Create character for the heart symbol on the LCD
  lcd.begin(16, 2);   //Initialize LCD display with 16 columns and 2 rows
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  lcd.setCursor(0, 1);
  lcd.print("Pulse Oximeter");    //Initialization messages

  delay(3000);

  while (false == MAX30102.begin()) {     //Attempt to initialize MAX30102 sensor
    Serial.println("init fail!");
    delay(1000);
  }

  Serial.println("init success!");    //success message if initialization works
  Serial.println("start measuring...");
  MAX30102.sensorStartCollect();
}

void loop() {    //main loop function
  MAX30102.getHeartbeatSPO2();

  if (millis() - previous_update_time > UPDATE_TIME) {    //check if it's time to update display
    lcd.clear();
    lcd.setCursor(0, 0);
    
    //Read and print the heartrate
    int heartbeatValue = MAX30102._sHeartbeatSPO2.Heartbeat < 0 ? 0 : MAX30102._sHeartbeatSPO2.Heartbeat;
    lcd.write((uint8_t)2);   //heart symbol
    lcd.print("Rate: ");
    lcd.print(heartbeatValue);
    lcd.print("bpm");

    lcd.setCursor(0, 1);

//read and print Sp02 value
    int spo2Value = MAX30102._sHeartbeatSPO2.SPO2 < 0 ? 0 : MAX30102._sHeartbeatSPO2.SPO2;
    lcd.print(" SpO2 : ");
    lcd.print(spo2Value);
    lcd.print("%");

    Serial.print(heartbeatValue);    //print to serial monitor
    Serial.print(", ");
    Serial.println(spo2Value);

    previous_update_time = millis();    //update previous update time
  }
}