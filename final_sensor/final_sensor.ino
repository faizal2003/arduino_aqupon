
#include "DFRobot_ESP_PH_WITH_ADC.h"
#include "DFRobot_ESP_EC.h"
// #include "OneWire.h"
//#include "DallasTemperature.h"
#include "Adafruit_ADS1X15.h"
//#include "EEPROM.h"
#define outec 27
#include <WiFiClientSecure.h>

const char* ssid = "ZTE_2.4G_rMTtse";
const char* password = "12345678";

const char*  server = "puh.web.id";  // Server URL

WiFiClientSecure client;
const int trigPin = 9;
const int echoPin = 10;
long duration;
int distance, tinggi, ketinggian;
//#define ONE_WIRE_BUS 15
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);
DFRobot_ESP_EC ec;
DFRobot_ESP_PH_WITH_ADC ph;
Adafruit_ADS1115 ads;
String ecString, phString;
String sensrec = "/sensor";
String sensrph = "/sensor/ph";
float voltageec, voltageph, phValue, ecValue, temperature = 25;

void postTo(String sensr, String value)
{
	  client.print("POST " + sensr);
	  client.println(" HTTP/1.1");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Host: puh.web.id");
    client.println("Content-Length: 6");
    client.println();
    client.println("val="+ value);
    // client.println();

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    // if there are incoming bytes available
    // from the server, read them and print them:
    while (client.available()) {
      char c = client.read();
      Serial.write(c);
    }

    client.stop();
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
  
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  client.setInsecure();
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
}

int waterlevel(){
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
  tinggi = 30 - distance;
  Serial.print("tinggi: ");
  Serial.println(tinggi);
  return tinggi;
}

void loop()
{
	static unsigned long timepoint = millis();
	if (millis() - timepoint > 7000U) //time interval: 7s
	{
		timepoint = millis();

    voltageec = ads.readADC_SingleEnded(0) / 10;
		voltageph = ads.readADC_SingleEnded(1) / 10; // read the voltage
		Serial.print("adc voltageph:");
		Serial.println(voltageph, 4);
		Serial.print("adc voltageec:");
		Serial.println(voltageec, 4);


    ecValue = ec.readEC(voltageec, temperature); // convert voltage to EC with temperature compensation
		Serial.print("EC:");
		Serial.print(ecValue, 4);
		Serial.println("ms/cm");

    ecString = String(ecValue);
    postTo(sensrec, ecString);

		phValue = ph.readPH(voltageph, temperature); // convert voltage to pH with temperature compensation
		Serial.print("pH:");
		Serial.println(phValue, 4);

    phString = String(phValue);
    postTo(sensrph, phString);

    ketinggian = waterlevel();


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

    if (ketinggian < 20) {
    // pump on
    }

    if (ketinggian == 30) {
    // pump off
    }

	}
	// ph.calibration(voltage, temperature); // calibration process by Serail CMD
}
