
#include "constant.h"

#include "wiring_private.h"
#include <RTCZero.h>
#include <Adafruit_SleepyDog.h>
#include <arduinoUART.h>

#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include "SparkFunBME280.h"
#include <ClosedCube_OPT3001.h>
#define OPT3001_ADDRESS 0x44
#include <OneWire.h>
#include <DallasTemperature.h>

// LoRaWAN library
#include <arduinoLoRaWAN.h>
#include <configLoRaWAN.h>

RTCZero rtc;

/* Change these values to set the current initial time */
const uint8_t seconds = 0;
const uint8_t minutes = 00;
const uint8_t hours = 10;

/* Change these values to set the current initial date */
const uint8_t day = 20;
const uint8_t month = 2;
const uint8_t year = 17;

//////////////////////////////////////////////
uint8_t uart = 1;
//////////////////////////////////////////////

// Device parameters for Back-End registration
////////////////////////////////////////////////////////////
char DEVICE_EUI[]  = "0004A30B001BBB91";
char APP_EUI[] = "0102030405060708";
char APP_KEY[] = "27C6363C4B58C2FAA796DA60F762841B";
////////////////////////////////////////////////////////////

// Define port to use in Back-End: from 1 to 223
uint8_t port = 1;

// Define data payload to send (maximum is up to data rate)
char data[] = "010203";

// variable
uint8_t error = 0;
uint8_t _deviceState = DEVICE_STATE_INIT;

Adafruit_HTU21DF htu = Adafruit_HTU21DF();
bool htuP = false;
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);
typedef struct ds
  {
	DeviceAddress addr;
	int16_t t_16;
	bool check;
  };
ds _ds;
BME280 bme280;
bool bme280P = false;
ClosedCube_OPT3001 opt3001;
bool opt3001P = false;

uint32_t cont = 0;

int errtx = 0;

void alarmMatch()
{
	//Serial2.println("Wakeup");
}

void printAddress(DeviceAddress deviceAddress);

// the setup function runs once when you press reset or power the board
void setup() {

	 // Switch unused pins as input and enabled built-in pullup
	 for (unsigned char pinNumber = 0; pinNumber < 23; pinNumber++)
	 {
		 pinMode(pinNumber, INPUT_PULLUP);
	 }

	 for (unsigned char pinNumber = 32; pinNumber < 42; pinNumber++)
	 {
		 pinMode(pinNumber, INPUT_PULLUP);
	 }

	 pinMode(25, INPUT_PULLUP);
	 pinMode(26, INPUT_PULLUP);

	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);
	pinMode(RN_RESET, INPUT);
	pinMode(RN_RESET, OUTPUT);
	digitalWrite(RN_RESET, HIGH);
	pinMode(BAT_ADC_EN, OUTPUT);
	digitalWrite(BAT_ADC_EN, LOW);
	pinMode(BAT_ADC, INPUT);
	analogReadResolution(12);

	//Serial.begin(9600);
	Serial.begin(9600);
//	pinPeripheral(6, PIO_SERCOM);
//	pinPeripheral(7, PIO_SERCOM);
//	Serial1.begin(57600);

	digitalWrite(LED, HIGH);
	delay(10000);
	digitalWrite(LED, LOW);
	Serial.println("START");


	// RTC initialization
	rtc.begin();
	rtc.setTime(hours, minutes, seconds);
	rtc.setDate(day, month, year);

	htuP = htu.begin();
	Serial.print("Sensor HTU21DF: ");
	if (htuP)
	{
		Serial.println("htu ON");
	}
	else
	{
		Serial.println("htu OFF");
	}

	sensors.begin();
	memcpy(&_ds.addr, 0x00, 8);
	_ds.t_16 = 0;
	_ds.check = false;
	if (oneWire.search(_ds.addr))
	{
		Serial.print("ds18b20 ON: ");
		printAddress(_ds.addr);
		Serial.println("");
		_ds.check = true;
		sensors.setResolution(_ds.addr, 12);
	}
	else
		Serial.println("ds18b20 OFF");

	if(opt3001.begin(OPT3001_ADDRESS) == NO_ERROR)
	{
		Serial.println("opt3001P ON");
		opt3001P = true;
		OPT3001_Config newConfig;
		newConfig.RangeNumber = B1100;
		newConfig.ConvertionTime = B0;
		newConfig.Latch = B1;
		newConfig.ModeOfConversionOperation = B11;
		opt3001.writeConfig(newConfig);
	}
	else
		Serial.println("opt3001P OFF");

	bme280.settings.commInterface = I2C_MODE;
	bme280.settings.I2CAddress = 0x77;
	bme280.settings.runMode = 3;
	bme280.settings.tStandby = 0;
	bme280.settings.filter = 0;
	bme280.settings.tempOverSample = 1;
	bme280.settings.pressOverSample = 1;
	bme280.settings.humidOverSample = 1;
	if(bme280.begin() == 0x60)
	{
		Serial.println("bme280 ON");
		bme280P = true;
	}
	else
		Serial.println("bme280 OFF");

	Serial.end();
	USBDevice.detach();

//	 error = LoRaWAN.ON(uart);
//	 arduinoLoRaWAN::printAnswer(error);

}

// the loop function runs over and over again forever
void loop() {

	switch( _deviceState )
	{

		case DEVICE_STATE_INIT:
		{
			Watchdog.disable();
			Watchdog.enable(16000);
		    //////////////////////////////////////////////
		    // 1. Switch on
		    //////////////////////////////////////////////
		    error = LoRaWAN.ON(uart);
		    arduinoLoRaWAN::printAnswer(error);

		//    //////////////////////////////////////////////
		//    // 2. Set Device EUI
		//    //////////////////////////////////////////////
		//    error = LoRaWAN.setDeviceEUI(DEVICE_EUI);
		//
		//    Serial.print("2. Set Device EUI: ");
		//    Serial.println(error);
		//
		//    //////////////////////////////////////////////
		//    // 3. Set Application EUI
		//    //////////////////////////////////////////////
		//    error = LoRaWAN.setAppEUI(APP_EUI);
		//
		//    Serial.print("3. Set Application EUI: ");
		//    Serial.println(error);
		//
		//    //////////////////////////////////////////////
		//    // 4. Set Application Session Key
		//    //////////////////////////////////////////////
		//    error = LoRaWAN.setAppKey(APP_KEY);
		//
		//    Serial.print("4. Set Application Session Key: ");
		//    Serial.println(error);
		//
		//    //////////////////////////////////////////////
		//    // 5. Save configuration
		//    //////////////////////////////////////////////
		//    error = LoRaWAN.saveConfig();
		//
		//    Serial.print("5. Save configuration: ");
		//    Serial.println(error);

		    //------------------------------------
		    //Module configured
		    //------------------------------------

		//    error = LoRaWAN.getDeviceEUI();
		//    printAnswer(error);
		//    //Device EUI in LoRaWAN._devEUI
		//    Serial.print("LoRaWAN._devEUI: ");
		//    Serial.println(LoRaWAN._devEUI);

		//    error = LoRaWAN.getDeviceAddr();
		//    printAnswer(error);
		//    //Device Addres in LoRaWAN._devAddr
		//    Serial.print("LoRaWAN._devAddr: ");
		//    Serial.println(LoRaWAN._devAddr);

		    error = LoRaWAN.setDataRate(5);
		    arduinoLoRaWAN::printAnswer(error);
		    //Serial2.print("LoRaWAN._dataRate: ");
		    //Serial2.println(LoRaWAN._dataRate);

		    error = LoRaWAN.setRetries(0);
		    arduinoLoRaWAN::printAnswer(error);
		    //Serial2.print("LoRaWAN._dataRate: ");
		    //Serial2.println(LoRaWAN._dataRate);

		    error = LoRaWAN.setADR("on");
		    arduinoLoRaWAN::printAnswer(error);

		//    error = configChDefault(LoRaWAN);
		//    arduinoLoRaWAN::printAnswer(error);

		//    error = LoRaWAN.sleep(60000);
		//    printAnswer(error);

		    Watchdog.disable();
			_deviceState = DEVICE_STATE_JOIN;
			break;
		}

		case DEVICE_STATE_JOIN:
		{
			Watchdog.disable();
			//////////////////////////////////////////////
			// 6. Join network
			//////////////////////////////////////////////
			error = LoRaWAN.joinOTAA();
			arduinoLoRaWAN::printAnswer(error);

			// Check status
			 if( error == 0 )
			 {
			   //2. Join network OK

				 Watchdog.enable(16000);
			   _deviceState = DEVICE_STATE_SEND;
			 }
			 else
			 {
			   //2. Join network error
				 delay(30000);
				 _deviceState = DEVICE_STATE_JOIN;
			 }

			break;
		}

		case DEVICE_STATE_SEND:
		{
			uint8_t datab[4];
			char datas[9];
			data[0]='\0';

			int sensorValue = 0;
			digitalWrite(BAT_ADC_EN, HIGH);
			delay(500);
			sensorValue = analogRead(BAT_ADC);
			digitalWrite(BAT_ADC_EN, LOW);
			float v = ((float)sensorValue)*(0.0013427734375);
			float a = ( ( (v-3) / 1.2) * 254 ) + 1;
			uint8_t level = 0;
			if(a<=0)
				level = 1;
			else if(a>=254)
				level = 254;
			else
				level = (uint8_t) a;

			error = LoRaWAN.setBatteryLevel(level);
			arduinoLoRaWAN::printAnswer(error);

			float a_10 = ( ( (v) / 10) * 255 );
			uint8_t level_10 = 0;
			if(a_10<=0)
				level_10 = 0;
			else if(a_10>=255)
				level_10 = 255;
			else
				level_10 = (uint8_t) a_10;

			memcpy(&datab[0], &level_10, 1);
			sprintf(datas,"%02X", datab[0] & 0xff);
			strcat(data, datas);

			if (htuP)
			{
				float t1 = htu.readTemperature();
				Serial.print("htu temp: ");
				Serial.println(t1);
				float h1 = htu.readHumidity();
				Serial.print("htu hum: ");
				Serial.println(h1);
				memcpy(&datab[0], &t1, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
				memcpy(&datab[0], &h1, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
			}
			else
			{
				float zero = 0;
				memcpy(&datab[0], &zero, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
				memcpy(&datab[0], &zero, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
			}
			_ds.check = sensors.isConnected(_ds.addr);
			if(_ds.check == true)
			{
				sensors.requestTemperatures();
				delay(800);
				float _ds18b20Temp = sensors.getTempC(_ds.addr);
				memcpy(&datab[0], &_ds18b20Temp, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
			}
			else
			{
				float zero = 0;
				memcpy(&datab[0], &zero, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
			}
			if(opt3001P)
			{
				OPT3001 result = opt3001.readResult();
				if(result.error == NO_ERROR)
				{
					Serial.print("OPT3001: ");
					Serial.println(result.lux);
					memcpy(&datab[0], &result.lux, 4);
					sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
					strcat(data, datas);
				}
				else
				{
					float zero = 0;
					memcpy(&datab[0], &zero, 4);
					sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
					strcat(data, datas);
				}
			}
			else
			{
				float zero = 0;
				memcpy(&datab[0], &zero, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
			}
			if (bme280P)
			{
				Serial.print("bme280 temp: ");
				float t2 = bme280.readTempC();
				Serial.println(t2, 2);
				Serial.print("bme280 hum: ");
				float h2 = bme280.readFloatHumidity();
				Serial.println(h2, 2);
				Serial.print("bme280 press: ");
				float p = bme280.readFloatPressure();
				Serial.println(bme280.readFloatPressure(), 2);
				memcpy(&datab[0], &t2, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
				memcpy(&datab[0], &h2, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
				memcpy(&datab[0], &p, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
			}

			//////////////////////////////////////////////
			// 3. Send unconfirmed packet
			//////////////////////////////////////////////

			error = LoRaWAN.sendConfirmed(port, data);
			arduinoLoRaWAN::printAnswer(error);

			// Error messages:
			/*
			* '6' : Module hasn't joined a network
			* '5' : Sending error
			* '4' : Error with data length
			* '2' : Module didn't response
			* '1' : Module communication error
			*/
			// Check status
			if( error == 0 )
			{
			 //3. Send Confirmed packet OK
//			 Serial.println("Send Confirmed packet OK");
			 if (LoRaWAN._dataReceived == true)
			 {
			   //There's data on
			   //port number: LoRaWAN._port
			   //and Data in: LoRaWAN._data
//			   Serial.println("Downlink data");
//			   Serial.print("LoRaWAN._port: ");
//			   Serial.println(LoRaWAN._port);
//			   Serial.print("LoRaWAN._data: ");
//			   Serial.println(LoRaWAN._data);

			   int number = (int)strtol(LoRaWAN._data, NULL, 16);
			   // auto-reset
			   if(number == 97)
			   {
				   //auto-reset
				   Watchdog.enable(1000);
				   delay(2000);
			   }
			 }
			}
			else if( error == 2 )
			{
			   _deviceState = DEVICE_STATE_INIT;
			}
			else if( error == 6 )
			{
			   _deviceState = DEVICE_STATE_JOIN;
			}
			else
			{
			 //3. Send Confirmed packet error
				if(errtx++ >= 5)
				{
					//auto-reset
					Watchdog.enable(1000);
					delay(2000);
				}
			}
			digitalWrite(LED, HIGH);
			delay(300);
			digitalWrite(LED, LOW);

			error = LoRaWAN.sleep(500000);
			arduinoLoRaWAN::printAnswer(error);

			//rtc.setAlarmSeconds((rtc.getAlarmSeconds() + 30) % 60);
			rtc.setAlarmMinutes((rtc.getAlarmMinutes() + 3) % 60);
			rtc.enableAlarm(rtc.MATCH_MMSS);
			rtc.attachInterrupt(alarmMatch);

			digitalWrite(LED, LOW);
			Watchdog.disable();
			rtc.standbyMode();
//			delay(120000);
			Watchdog.enable(16000);

			delay(500);

			error = LoRaWAN.wakeUP();
			arduinoLoRaWAN::printAnswer(error);

			int err_ch = 0;
			do{
				delay(500);
				error = LoRaWAN.check();
				arduinoLoRaWAN::printAnswer(error);
				if(err_ch++>=5)
					_deviceState = DEVICE_STATE_INIT;
			}while((error!=0) && (_deviceState!=DEVICE_STATE_INIT));
		}
	}


//	uint8_t datab[4];
//	char datas[9];
//	data[0]='\0';
//	if (htuP)
//	{
//		float t1 = htu.readTemperature();
//		Serial.print("htu temp: "); Serial.println(t1);
//		float h1 = htu.readHumidity();
//		Serial.print("htu hum: "); Serial.println(h1);
//		memcpy(&datab[0], &t1, 4);
//		sprintf(datas,"%X%X%X%X", datab[0], datab[1], datab[2], datab[3]);
//		strcat(data, datas);
//		memcpy(&datab[0], &h1, 4);
//		sprintf(datas,"%X%X%X%X", datab[0], datab[1], datab[2], datab[3]);
//		strcat(data, datas);
////		Serial.print("hex: ");
////		Serial.println(data);
//	}
//	if(ds18b20Temp(_ds) == 0)
//	{
//		Serial.print("DS18B20 temp: ");
//		Serial.println(_ds18b20Temp);
//		memcpy(&datab[0], &_ds18b20Temp, 4);
//		sprintf(datas,"%X%X%X%X", datab[0], datab[1], datab[2], datab[3]);
//		strcat(data, datas);
////		//Serial.print("hex: ");
////		//Serial.println(data);
//	}
//	if(opt3001P)
//	{
//		OPT3001 result = opt3001.readResult();
//		if(result.error == NO_ERROR)
//		{
//			Serial.print("OPT3001: ");
//			Serial.println(result.lux);
//			memcpy(&datab[0], &result.lux, 4);
//			sprintf(datas,"%X%X%X%X", datab[0], datab[1], datab[2], datab[3]);
//			strcat(data, datas);
////			//Serial.print("hex: ");
////			//Serial.println(data);
//		}
//	}
//	if (bme280P)
//	{
//		Serial.print("bme280 temp: ");
//		float t2 = bme280.readTempC();
//		Serial.println(t2, 2);
//		Serial.print("bme280 hum: ");
//		float h2 = bme280.readFloatHumidity();
//		Serial.println(h2, 2);
//		Serial.print("bme280 press: ");
//		float p = bme280.readFloatPressure();
//		Serial.println(bme280.readFloatPressure(), 2);
//		memcpy(&datab[0], &t2, 4);
//		sprintf(datas,"%X%X%X%X", datab[0], datab[1], datab[2], datab[3]);
//		strcat(data, datas);
//		memcpy(&datab[0], &h2, 4);
//		sprintf(datas,"%X%X%X%X", datab[0], datab[1], datab[2], datab[3]);
//		strcat(data, datas);
//		memcpy(&datab[0], &p, 4);
//		sprintf(datas,"%X%X%X%X", datab[0], datab[1], datab[2], datab[3]);
//		strcat(data, datas);
////		//Serial2.print("hex: ");
////		//Serial2.println(data);
//	}
//
//	error = LoRaWAN.check();
//	arduinoLoRaWAN::printAnswer(error);
//
//  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);              // wait for a second
//  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
//  delay(5000);              // wait for a second
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
