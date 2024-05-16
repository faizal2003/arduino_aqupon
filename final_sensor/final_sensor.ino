/*
 * file read_ph.ino
 * @ https://github.com/GreenPonik/DFRobot_ESP_PH_WITH_ADC_BY_GREENPONIK
 *
 * IMPORTANT : to make it work, you will need the help of an additionnal ADC converter because the one on the ESP32 isn't accurate enough. Here we used an ADS1115 from Adafruit
 * You can find it here : https://www.adafruit.com/product/1085
 * And here is the library you'll need to add to your sketch : https://github.com/adafruit/Adafruit_ADS1X15
 * 
 * This is the sample code for Gravity: Analog pH Sensor / Meter Kit V2, SKU:SEN0161-V2
 * In order to guarantee precision, a temperature sensor such as DS18B20 is needed, to execute automatic temperature compensation.
 * You can send commands in the serial monitor to execute the calibration.
 * Serial Commands:
 *   enterph -> enter the calibration mode
 *   calph   -> calibrate with the standard buffer solution, two buffer solutions(4.0 and 7.0) will be automaticlly recognized
 *   exitph  -> save the calibrated parameters and exit from calibration mode
 * 
 * Based on the @ https://github.com/DFRobot/DFRobot_PH
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * ##################################################
 * ##################################################
 * ######## Forked on github by GreenPonik ##########
 * ############# ONLY ESP COMPATIBLE ################
 * ##################################################
 * ##################################################
 * 
 * version  V1.2.2
 * date  2019-06
 */

#include "DFRobot_ESP_PH_WITH_ADC.h"
#include "DFRobot_ESP_EC.h"
// #include "OneWire.h"
//#include "DallasTemperature.h"
#include "Adafruit_ADS1X15.h"
//#include "EEPROM.h"
#define outec 27
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance, tinggi;
//#define ONE_WIRE_BUS 15
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
DFRobot_ESP_EC ec;
DFRobot_ESP_PH_WITH_ADC ph;
Adafruit_ADS1115 ads;

float voltageec, voltageph, phValue, ecValue, temperature = 25;

float readTemperature()
{
	
}

void setup()
{
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
	Serial.begin(115200);
	//EEPROM.begin(32);//needed EEPROM.begin to store calibration k in eeprom
	//ph.begin();
	//sensors.begin();
	ads.setGain(GAIN_ONE);
	ads.begin();
}

void waterlevel(){
   // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  tinggi = 60 - distance;
  Serial.print("tinggi: ");
  Serial.println(tinggi);
}

void loop()
{
	static unsigned long timepoint = millis();
	if (millis() - timepoint > 1000U) //time interval: 1s
	{
		timepoint = millis();
		/**
		 * index 0 for adc's pin A0
 		 * index 1 for adc's pin A1
		 * index 2 for adc's pin A2
		 * index 3 for adc's pin A3
		*/
    voltageec = ads.readADC_SingleEnded(0) / 10;
		voltageph = ads.readADC_SingleEnded(1) / 10; // read the voltage
		Serial.print("adc voltageph:");
		Serial.println(voltageph, 4);
		Serial.print("adc voltageec:");
		Serial.println(voltageec, 4);

		// temperature = readTemperature(); // read your temperature sensor to execute temperature compensation
		// Serial.print("temperature:");
		// Serial.print(temperature, 1);
		// Serial.println("^C");

    ecValue = ec.readEC(voltageec, temperature); // convert voltage to EC with temperature compensation
		Serial.print("EC:");
		Serial.print(ecValue, 4);
		Serial.println("ms/cm");

    
		phValue = ph.readPH(voltageph, temperature); // convert voltage to pH with temperature compensation
		Serial.print("pH:");
		Serial.println(phValue, 4);

    waterlevel();

    if (ecValue < 1.2){
      digitalWrite(outec, HIGH);
    }else{
      digitalWrite(outec, LOW);
    }

    if (phValue > 7) {
      // ph down

    }

    if (phValue < 5){
      // ph up

    }

	}
	// ph.calibration(voltage, temperature); // calibration process by Serail CMD
}
