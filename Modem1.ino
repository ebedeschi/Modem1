
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
#define SLEEP_TIME 10
#define DEFAULT_DUTY_CYCLE_TIME 180

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
uint8_t pre_tx_ok = 0;
uint32_t cont = 0;

uint32_t sleep_cont = 0;

uint32_t appWatchdog = 0;

int err_tx = 0;

uint8_t duty_cycle_time = DEFAULT_DUTY_CYCLE_TIME;

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

	Serial.begin(9600);
	Serial5.begin(9600);
//	pinPeripheral(6, PIO_SERCOM);
//	pinPeripheral(7, PIO_SERCOM);
//	Serial1.begin(57600);

	digitalWrite(LED, HIGH);
	delay(10000);
	digitalWrite(LED, LOW);
	Serial5.println("START");


	// RTC initialization
	rtc.begin();
	rtc.setTime(hours, minutes, seconds);
	rtc.setDate(day, month, year);

	htuP = htu.begin();
	Serial5.print("Sensor HTU21DF: ");
	if (htuP)
	{
		Serial5.println("htu ON");
	}
	else
	{
		Serial5.println("htu OFF");
	}

	sensors.begin();
	memcpy(&_ds.addr, 0x00, 8);
	_ds.t_16 = 0;
	_ds.check = false;
	if (oneWire.search(_ds.addr))
	{
		Serial5.print("ds18b20 ON: ");
		printAddress(_ds.addr);
		Serial5.println("");
		_ds.check = true;
		sensors.setResolution(_ds.addr, 12);
	}
	else
		Serial5.println("ds18b20 OFF");

	if(opt3001.begin(OPT3001_ADDRESS) == NO_ERROR)
	{
		Serial5.println("opt3001P ON");
		opt3001P = true;
		OPT3001_Config newConfig;
		newConfig.RangeNumber = B1100;
		newConfig.ConvertionTime = B0;
		newConfig.Latch = B1;
		newConfig.ModeOfConversionOperation = B11;
		opt3001.writeConfig(newConfig);
	}
	else
		Serial5.println("opt3001P OFF");

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
		Serial5.println("bme280 ON");
		bme280P = true;
	}
	else
		Serial5.println("bme280 OFF");

	Serial5.println("Fine setup");

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

			error = LoRaWAN.OFF(uart);
			digitalWrite(RN_RESET, LOW);
			delay(500);
			digitalWrite(RN_RESET, HIGH);
			error = LoRaWAN.ON(uart);

			Serial5.print("1. Switch on: ");
			arduinoLoRaWAN::printAnswer(error);

			error = LoRaWAN.setDataRate(0);
			arduinoLoRaWAN::printAnswer(error);
			Serial5.print("LoRaWAN._dataRate: ");
			Serial5.println(LoRaWAN._dataRate);

			error = LoRaWAN.setRetries(0);
			arduinoLoRaWAN::printAnswer(error);
			Serial5.print("LoRaWAN._retries: ");
			Serial5.println(LoRaWAN._retries);

			error = LoRaWAN.setADR("on");
			arduinoLoRaWAN::printAnswer(error);
			Serial5.print("LoRaWAN._adr: ");
			Serial5.println(LoRaWAN._adr);

			_deviceState = DEVICE_STATE_JOIN;
			break;
		}

		case DEVICE_STATE_JOIN:
		{
			Watchdog.disable();
			Watchdog.enable(16000);
			//////////////////////////////////////////////
			// 6. Join network
			//////////////////////////////////////////////
			error = LoRaWAN.joinOTAA();
			arduinoLoRaWAN::printAnswer(error);

			// Check status
			 if( error == 0 )
			 {
			   //2. Join network OK
			   Serial5.println("Join network OK");

			   _deviceState = DEVICE_STATE_SEND;
			 }
			 else
			 {
			   //2. Join network error
				 Serial5.println("Join network error");
				 if(appWatchdog++ >= 10)
				 {
					 Serial5.println("Auto reset appWatchdog");
					 appWatchdog = 0;
					 Watchdog.reset();
					 Watchdog.enable(1000);
					 delay(2000);
				 }
				 Watchdog.reset();
				 delay(10000);
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

			//tx_ok trasmissione precedente
			memcpy(&datab[0], &pre_tx_ok, 1);
			sprintf(datas,"%02X", datab[0] & 0xff);
			strcat(data, datas);

			if (htuP)
			{
				float t1 = htu.readTemperature();
				Serial5.print("htu temp: ");
				Serial5.println(t1);
				float h1 = htu.readHumidity();
				Serial5.print("htu hum: ");
				Serial5.println(h1);
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
				Serial5.print("ds18b20Temp: ");
				Serial5.println(_ds18b20Temp);
				memcpy(&datab[0], &_ds18b20Temp, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
			}
			else
			{
				Serial5.println("ds18b20Temp error: 0");
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
					Serial5.print("OPT3001: ");
					Serial5.println(result.lux);
					memcpy(&datab[0], &result.lux, 4);
					sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
					strcat(data, datas);
				}
				else
				{
					Serial5.println("OPT3001 error: 0");
					float zero = 0;
					memcpy(&datab[0], &zero, 4);
					sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
					strcat(data, datas);
				}
			}
			else
			{
				Serial5.println("OPT3001 error: 1");
				float zero = 0;
				memcpy(&datab[0], &zero, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
			}
			if (bme280P)
			{
				Serial5.print("bme280 temp: ");
				float t2 = bme280.readTempC();
				Serial5.println(t2, 2);
				Serial5.print("bme280 hum: ");
				float h2 = bme280.readFloatHumidity();
				Serial5.println(h2, 2);
				Serial5.print("bme280 press: ");
				float p = bme280.readFloatPressure();
				Serial5.println(bme280.readFloatPressure(), 2);
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

			int tx_cont = 0;
			do
			{
				Watchdog.reset();
				if(tx_cont>0)
					delay(5000);
				Serial5.print("Send unconfirmed packet: ");
				Serial5.println(tx_cont);
				error = LoRaWAN.sendConfirmed(port, data);
				arduinoLoRaWAN::printAnswer(error);
			}while(error!=0 && tx_cont++<3);

			_deviceState = DEVICE_STATE_SLEEP;

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
				Serial5.println("Send Confirmed packet OK");
				err_tx=0;
				pre_tx_ok = 1;
				if (LoRaWAN._dataReceived == true)
				{
					//There's data on
					//port number: LoRaWAN._port
					//and Data in: LoRaWAN._data
					Serial5.println("Downlink data");
					Serial5.print("LoRaWAN._port: ");
					Serial5.println(LoRaWAN._port);
					Serial5.print("LoRaWAN._data: ");
					Serial5.println(LoRaWAN._data);

					pre_tx_ok = 2;

					if(LoRaWAN._port == 1)
					{

						int number = (int)strtol(LoRaWAN._data, NULL, 16);
						Serial5.println(number);

						// auto-reset
						if(number == 97)
						{
						//auto-reset
						Serial5.println("Auto-reset");
						Watchdog.enable(1000);
						delay(2000);
						}
					}
					else if(LoRaWAN._port == 2)
					{
						long number = strtol(LoRaWAN._data, NULL, 16);
						Serial5.println(number);

						uint8_t command = (number >> 16) && 0xFF;
						uint16_t dct_next = (number >> 8 && 0xFF) + (number >> 0 && 0xFF);

						if(number == 100) // d
						{
						//auto-reset
						Serial5.println("Auto-reset");
						Watchdog.enable(1000);
						delay(2000);
						}
					}

				}
			}
//			else if( error == 2 )
//			{
//				_deviceState = DEVICE_STATE_INIT;
//			}
			else if( error == 6 )
			{
				_deviceState = DEVICE_STATE_JOIN;
			}
			else
			{
				Serial5.println("Send Confirmed packet ERROR");
				if(err_tx++ >= 5)
				{
					Serial5.println("Auto-reset");
					Watchdog.enable(1000);
					delay(2000);
				}
			}
			break;
		}
		case DEVICE_STATE_SLEEP:
		{
			//////////////////////////////////////////////
			// 7. Sleep
			//////////////////////////////////////////////

			Serial5.println("SLEEP");
			Serial5.print("sleep_cont: ");
			Serial5.println(sleep_cont);

			if(sleep_cont == 0)
			{
				sleep_cont++;

				Serial5.println("LoRaWAN.sleep");
				error = LoRaWAN.sleep(2000000);
				arduinoLoRaWAN::printAnswer(error);

			}else if( ( sleep_cont > 0 ) && ( sleep_cont < ( duty_cycle_time / SLEEP_TIME ) ) )
			{
				sleep_cont++;

				char buf[20];
				sprintf(buf, "%02d:%02d:%02d - %2d/%2d/%4d", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds(), rtc.getDay(), rtc.getMonth(), rtc.getYear());
				Serial5.println(buf);

				rtc.disableAlarm();
				rtc.setAlarmSeconds((rtc.getSeconds() + SLEEP_TIME) % 60);
				rtc.enableAlarm(rtc.MATCH_SS);
				rtc.attachInterrupt(alarmMatch);

				sprintf(buf, "%02d:%02d:%02d - %2d/%2d/%4d", rtc.getAlarmHours(), rtc.getAlarmMinutes(), rtc.getAlarmSeconds(), rtc.getAlarmDay(), rtc.getAlarmMonth(), rtc.getAlarmYear());
				Serial5.println(buf);

				Watchdog.disable();
				Watchdog.enable(16000);
				Serial5.println("standbyMode");
				delay(100);
				Serial5.end();

				rtc.standbyMode();

				Serial5.begin(9600);
				Serial5.println("exit sleep");
				sprintf(buf, "%02d:%02d:%02d - %2d/%2d/%4d", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds(), rtc.getDay(), rtc.getMonth(), rtc.getYear());
				Serial5.println(buf);

				Watchdog.disable();
				Watchdog.enable(16000);

				_deviceState = DEVICE_STATE_SLEEP;

			}else if( sleep_cont >= ( duty_cycle_time / SLEEP_TIME ) )
			{
				Serial5.println("WAKEUP radio");
				sleep_cont = 0;
				_deviceState = DEVICE_STATE_SEND;

				error = LoRaWAN.wakeUP();
				arduinoLoRaWAN::printAnswer(error);
				delay(100);
				error = LoRaWAN.check();
				arduinoLoRaWAN::printAnswer(error);
				delay(100);
				error = LoRaWAN.check();
				arduinoLoRaWAN::printAnswer(error);
				delay(100);
				error = LoRaWAN.check();
				arduinoLoRaWAN::printAnswer(error);
				if(error > 0)
					_deviceState = DEVICE_STATE_INIT;
			}

			break;
		}
	}

}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial5.print("0");
    Serial5.print(deviceAddress[i], HEX);
  }
}
