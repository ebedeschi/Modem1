
#include "constant.h"

#include "wiring_private.h"
#include <RTCZero.h>
#include <arduinoUART.h>

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
uint8_t error;

uint32_t cont = 0;

void alarmMatch()
{
	Serial2.println("Wakeup");
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
//	pinMode(RN_RESET, OUTPUT);
//	digitalWrite(RN_RESET, HIGH);
	pinMode(BAT_ADC_EN, OUTPUT);
	digitalWrite(BAT_ADC_EN, LOW);
	pinMode(BAT_ADC, INPUT);

	Serial2.begin(9600);
	pinPeripheral(6, PIO_SERCOM);
	pinPeripheral(7, PIO_SERCOM);
//	Serial1.begin(57600);

	digitalWrite(LED, HIGH);
	delay(10000);
	digitalWrite(LED, LOW);

	// RTC initialization
	rtc.begin();
	rtc.setTime(hours, minutes, seconds);
	rtc.setDate(day, month, year);

	// RTC alarm setting on every 15 s resulting in 1 minute sleep period
//	rtc.setAlarmSeconds(30);
//	rtc.enableAlarm(rtc.MATCH_SS);
//	rtc.attachInterrupt(alarmMatch);

    //------------------------------------
    //Module configuration
    //------------------------------------

    //////////////////////////////////////////////
    // 1. Switch on
    //////////////////////////////////////////////
    error = LoRaWAN.ON(uart);

    Serial2.print("1. Switch on: ");
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
    Serial2.print("LoRaWAN._dataRate: ");
    Serial2.println(LoRaWAN._dataRate);

    error = LoRaWAN.setRetries(0);
    arduinoLoRaWAN::printAnswer(error);
    Serial2.print("LoRaWAN._dataRate: ");
    Serial2.println(LoRaWAN._dataRate);

//    error = configChDefault(LoRaWAN);
//    arduinoLoRaWAN::printAnswer(error);

//    error = LoRaWAN.sleep(60000);
//    printAnswer(error);

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


     }
     else
     {
       //2. Join network error
    	 Serial.println("Join network error");
     }


}

// the loop function runs over and over again forever
void loop() {

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
	else
	{
	 //3. Send Confirmed packet error
	   Serial.println("Send Confirmed packet ERROR");
	}
	digitalWrite(LED, HIGH);
	delay(300);
	digitalWrite(LED, LOW);

	error = LoRaWAN.sleep(300000);
	arduinoLoRaWAN::printAnswer(error);

	Serial2.print("Start sleep: ");
	Serial2.println(++cont);
//	Serial2.flush();
//	Serial2.end();

	rtc.setAlarmSeconds((rtc.getAlarmSeconds() + 30) % 60);
	rtc.enableAlarm(rtc.MATCH_SS);
	rtc.attachInterrupt(alarmMatch);

	digitalWrite(LED, LOW);
//	USBDevice.detach();
	rtc.standbyMode();

	// Initialize USB and attach to host (not required if not in use)
//	USBDevice.init();
//	USBDevice.attach();
	delay(500);
//	Serial2.begin(9600);
	Serial2.println("Exit sleep");
//	digitalWrite(LED, HIGH);
//	delay(3000);
//	digitalWrite(LED, LOW);

	error = LoRaWAN.wakeUP();
	arduinoLoRaWAN::printAnswer(error);

	error = LoRaWAN.check();
	arduinoLoRaWAN::printAnswer(error);


//  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(1000);              // wait for a second
//  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
//  delay(1000);              // wait for a second
}



