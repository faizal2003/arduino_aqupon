
 /*
 * file DFRobot_ESP_EC_BY_GREENPONIK.ino
 * @ https://github.com/GreenPonik/DFRobot_ESP_EC_BY_GREENPONIK
 *
 * This is the sample code for Gravity: Analog EC Sensor / Meter Kit V2, SKU:DFR300
 *  * In order to guarantee precision, a temperature sensor such as DS18B20 is needed, to execute automatic temperature compensation.
 * You can send commands in the serial monitor to execute the calibration.
 * Serial Commands:
 *   enterec -> enter the calibration mode
 *   calec   -> calibrate with the standard buffer solution, two buffer solutions(1413us/cm and 12.88ms/cm) will be automaticlly recognized
 *   exitec  -> save the calibrated parameters and exit from calibration mode
 * 
 * Based on the @ https://github.com/DFRobot/DFRobot_EC
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * ##################################################
 * ##################################################
 * ########## Fork on github by GreenPonik ##########
 * ############# ONLY ESP COMPATIBLE ################
 * ##################################################
 * ##################################################
 * 
 * version  V1.1.2
 * date  2019-06
 */

#include "Arduino.h"
#include "Adafruit_ADS1X15.h"
#include "DFRobot_ESP_EC.h"
// #include "EEPROM.h"
// #define EC_PIN 15
// #define ESPADC 4096.0   //the esp Analog Digital Convertion value
// #define ESPVOLTAGE 3300 //the esp voltage supply value
#define outec 27
// pin relay 27 35 32
DFRobot_ESP_EC ec;
Adafruit_ADS1115 ads;

float voltage, ecValue, temperature = 25;

void setup()
{
  pinMode(outec, OUTPUT);
	Serial.begin(115200);
	// EEPROM.begin(32);//needed EEPROM.begin to store calibration k in eeprom
	// ec.begin();//by default lib store calibration k since 10 change it by set ec.begin(30); to start from 30
	ads.setGain(GAIN_ONE);
	ads.begin();
}

void loop()
{
	static unsigned long timepoint = millis();
	if (millis() - timepoint > 1000U) //time interval: 1s
	{

		timepoint = millis();
		voltage = ads.readADC_SingleEnded(0) / 10;
		// voltage = analogRead(EC_PIN) / ESPADC * ESPVOLTAGE;
    Serial.print("voltage:");
		Serial.println(voltage, 4);

		//temperature = readTemperature();  // read your temperature sensor to execute temperature compensation
		Serial.print("temperature:");
		Serial.print(temperature, 1);
		Serial.println("^C");

		ecValue = ec.readEC(voltage, temperature); // convert voltage to EC with temperature compensation
		Serial.print("EC:");
		Serial.print(ecValue, 4);
		Serial.println("ms/cm");
    if (ecValue > 5.0){
      digitalWrite(outec, HIGH);
    }else{
      digitalWrite(outec, LOW);
  }
	}
	ec.calibration(voltage, temperature); // calibration process by Serail CMD
}
