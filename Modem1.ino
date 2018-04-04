
#include "constant.h"

#include "wiring_private.h"
#include <RTCZero.h>
#include <arduinoUART.h>

#include <Wire.h>
#include "Adafruit_HTU21DF.h"
#include "SparkFunBME280.h"
#include <ClosedCube_OPT3001.h>
#define OPT3001_ADDRESS 0x44
#include <OneWire.h>

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
BME280 bme280;
bool bme280P = false;
ClosedCube_OPT3001 opt3001;
bool opt3001P = false;
OneWire _ds(DS18B20_PIN);  // on pin 11 (a 4.7K resistor is necessary)
float _ds18b20Temp = 0;

uint8_t ds18b20Temp(OneWire ds);

uint32_t cont = 0;

void alarmMatch()
{
	//Serial2.println("Wakeup");
}

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

	//Serial.begin(9600);
	Serial.begin(9600);
//	pinPeripheral(6, PIO_SERCOM);
//	pinPeripheral(7, PIO_SERCOM);
//	Serial1.begin(57600);

	digitalWrite(LED, HIGH);
	delay(10000);
	digitalWrite(LED, LOW);
	Serial.println("START");

//	USBDevice.detach();

	// RTC initialization
	rtc.begin();
	rtc.setTime(hours, minutes, seconds);
	rtc.setDate(day, month, year);

	// RTC alarm setting on every 15 s resulting in 1 minute sleep period
//	rtc.setAlarmSeconds(30);
//	rtc.enableAlarm(rtc.MATCH_SS);
//	rtc.attachInterrupt(alarmMatch);

    //////////////////////////////////////////////
    // 7. Switch off
    //////////////////////////////////////////////
//    error = LoRaWAN.OFF(uart);
//    Serial.print("Switch off: ");
//    printAnswer(error);

//    Serial.println("Start sleep");
//	Serial.flush();
//    Serial.end();
////    Serial1.end();
//
////    pinMode(0, INPUT_PULLUP);
////    pinMode(1, INPUT_PULLUP);
//    // RTC initialization
//    rtc.begin();
//    rtc.setTime(hours, minutes, seconds);
//    rtc.setDate(day, month, year);
//
//    // RTC alarm setting on every 15 s resulting in 1 minute sleep period
//    rtc.setAlarmSeconds(10);
//    rtc.enableAlarm(rtc.MATCH_SS);
//    rtc.attachInterrupt(alarmMatch);
//
//    digitalWrite(LED, LOW);
//    USBDevice.detach();
//    rtc.standbyMode();
//
//    // Initialize USB and attach to host (not required if not in use)
//    USBDevice.init();
//    USBDevice.attach();
//
//	digitalWrite(LED, HIGH);
//	delay(5000);
//	digitalWrite(LED, LOW);
//
//    error = LoRaWAN.wakeUP();
//    printAnswer(error);
//
//	delay(100);
//
//	error = LoRaWAN.check();
//	printAnswer(error);


//	digitalWrite(LED, HIGH);
//	delay(5000);
//	digitalWrite(LED, LOW);

	htuP = htu.begin();
	Serial.print("Sensor HTU21DF: ");
	if (htuP)
	{
		Serial.println("ON");
	}
	else
	{
		Serial.println("OFF");
	}

	bme280.settings.commInterface = I2C_MODE;
	bme280.settings.I2CAddress = 0x77;
	bme280.settings.runMode = 3;
	bme280.settings.tStandby = 0;
	bme280.settings.filter = 0;
	bme280.settings.tempOverSample = 1;
	bme280.settings.pressOverSample = 1;
	bme280.settings.humidOverSample = 1;
	if(bme280.begin() == 0x60)
		bme280P = true;

	if(opt3001.begin(OPT3001_ADDRESS) == NO_ERROR)
	{
		opt3001P = true;
		OPT3001_Config newConfig;
		newConfig.RangeNumber = B1100;
		newConfig.ConvertionTime = B0;
		newConfig.Latch = B1;
		newConfig.ModeOfConversionOperation = B11;
		opt3001.writeConfig(newConfig);
	}

	 error = LoRaWAN.ON(uart);
	 arduinoLoRaWAN::printAnswer(error);

}

// the loop function runs over and over again forever
void loop() {

	switch( _deviceState )
	{

		case DEVICE_STATE_INIT:
		{
		    //////////////////////////////////////////////
		    // 1. Switch on
		    //////////////////////////////////////////////
		    error = LoRaWAN.ON(uart);

		    //Serial2.print("1. Switch on: ");
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


			_deviceState = DEVICE_STATE_JOIN;
			break;
		}

		case DEVICE_STATE_JOIN:
		{
			//////////////////////////////////////////////
			// 6. Join network
			//////////////////////////////////////////////
			error = LoRaWAN.joinOTAA();
			arduinoLoRaWAN::printAnswer(error);

			// Check status
			 if( error == 0 )
			 {
			   //2. Join network OK
			   Serial.println("Join network OK");

			   _deviceState = DEVICE_STATE_SEND;
			 }
			 else
			 {
			   //2. Join network error
				 Serial.println("Join network error");
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
		//		//Serial2.print("hex: ");
		//		//Serial2.println(data);
			}
			if(ds18b20Temp(_ds) == 0)
			{
				Serial.print("DS18B20 temp: ");
				Serial.println(_ds18b20Temp);
				memcpy(&datab[0], &_ds18b20Temp, 4);
				sprintf(datas,"%02X%02X%02X%02X", datab[0] & 0xff, datab[1] & 0xff, datab[2] & 0xff, datab[3] & 0xff);
				strcat(data, datas);
		//		//Serial2.print("hex: ");
		//		//Serial2.println(data);
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
		//			//Serial2.print("hex: ");
		//			//Serial2.println(data);
				}
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
		//		//Serial2.print("hex: ");
		//		//Serial2.println(data);
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
			 Serial.println("Send Confirmed packet OK");
			 if (LoRaWAN._dataReceived == true)
			 {
			   //There's data on
			   //port number: LoRaWAN._port
			   //and Data in: LoRaWAN._data
			   Serial.println("Downlink data");
			   Serial.print("LoRaWAN._port: ");
			   Serial.println(LoRaWAN._port);
			   Serial.print("LoRaWAN._data: ");
			   Serial.println(LoRaWAN._data);
			 }
			}
			else if( error == 6 )
			{
			 //3. Send Confirmed packet error
			   Serial.println("Module hasn't joined a network");

			   _deviceState = DEVICE_STATE_JOIN;
			}
			else
			{
			 //3. Send Confirmed packet error
			   Serial.println("Send Confirmed packet ERROR");
			}
			digitalWrite(LED, HIGH);
			delay(300);
			digitalWrite(LED, LOW);

			error = LoRaWAN.sleep(500000);
			arduinoLoRaWAN::printAnswer(error);

			Serial.print("Start sleep: ");
			Serial.println(++cont);
//			//Serial2.flush();
//			//Serial2.end();

			//rtc.setAlarmSeconds((rtc.getAlarmSeconds() + 30) % 60);
			rtc.setAlarmMinutes((rtc.getAlarmMinutes() + 2) % 60);
			rtc.enableAlarm(rtc.MATCH_MMSS);
			rtc.attachInterrupt(alarmMatch);

			digitalWrite(LED, LOW);
		//	USBDevice.detach();
//			rtc.standbyMode();
			delay(120000);

			// Initialize USB and attach to host (not required if not in use)
		//	USBDevice.init();
		//	USBDevice.attach();
			delay(500);
//			//Serial2.begin(9600);
			Serial.println("Exit sleep");
		//	digitalWrite(LED, HIGH);
		//	delay(3000);
		//	digitalWrite(LED, LOW);

			error = LoRaWAN.wakeUP();
			arduinoLoRaWAN::printAnswer(error);

			error = LoRaWAN.check();
			arduinoLoRaWAN::printAnswer(error);
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

//	error = LoRaWAN.check();
//	arduinoLoRaWAN::printAnswer(error);
//
//  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);              // wait for a second
//  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
//  delay(5000);              // wait for a second
}


uint8_t ds18b20Temp(OneWire ds)
{
	  byte i;
	  byte present = 0;
	  byte type_s;
	  byte data[12];
	  byte addr[8];

	  if ( !ds.search(addr)) {
//	    Serial.println("No more addresses.");
//	    Serial.println();
	    ds.reset_search();
	    return 1;
	  }

//	  Serial.print("ROM =");
//	  for( i = 0; i < 8; i++) {
//	    Serial.write(' ');
//	    Serial.print(addr[i], HEX);
//	  }

	  if (OneWire::crc8(addr, 7) != addr[7]) {
	      //Serial2.println("OneWire CRC is not valid!");
	      return 2;
	  }
//	  Serial.println();

	  // the first ROM byte indicates which chip
	  switch (addr[0]) {
	    case 0x10:
//	      Serial.println("  Chip = DS18S20");  // or old DS1820
	      type_s = 1;
	      break;
	    case 0x28:
//	      Serial.println("  Chip = DS18B20");
	      type_s = 0;
	      break;
	    case 0x22:
//	      Serial.println("  Chip = DS1822");
	      type_s = 0;
	      break;
	    default:
	      //Serial2.println("Device is not a DS18x20 family device.");
	      return 3;
	  }

	  ds.reset();
	  ds.select(addr);
	  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

	  delay(1000);     // maybe 750ms is enough, maybe not
	  // we might do a ds.depower() here, but the reset will take care of it.

	  present = ds.reset();
	  ds.select(addr);
	  ds.write(0xBE);         // Read Scratchpad

//	  Serial.print("  Data = ");
//	  Serial.print(present, HEX);
//	  Serial.print(" ");
	  for ( i = 0; i < 9; i++) {           // we need 9 bytes
	    data[i] = ds.read();
//	    Serial.print(data[i], HEX);
//	    Serial.print(" ");
	  }
//	  Serial.print(" CRC=");
//	  Serial.print(OneWire::crc8(data, 8), HEX);
//	  Serial.println();

	  // Convert the data to actual temperature
	  // because the result is a 16 bit signed integer, it should
	  // be stored to an "int16_t" type, which is always 16 bits
	  // even when compiled on a 32 bit processor.
	  int16_t raw = (data[1] << 8) | data[0];
	  if (type_s) {
	    raw = raw << 3; // 9 bit resolution default
	    if (data[7] == 0x10) {
	      // "count remain" gives full 12 bit resolution
	      raw = (raw & 0xFFF0) + 12 - data[6];
	    }
	  } else {
	    byte cfg = (data[4] & 0x60);
	    // at lower res, the low bits are undefined, so let's zero them
	    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
	    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
	    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
	    //// default is 12 bit resolution, 750 ms conversion time
	  }
	  _ds18b20Temp = (float)raw / 16.0;
//	  fahrenheit = celsius * 1.8 + 32.0;
//	  Serial.print("  Temperature = ");
//	  Serial.print(celsius);
//	  Serial.print(" Celsius, ");
//	  Serial.print(fahrenheit);
//	  Serial.println(" Fahrenheit");
	  return 0;
}

