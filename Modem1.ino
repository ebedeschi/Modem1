
# include "constant.h"

#include <RTCZero.h>
#include <arduinoUART.h>

// LoRaWAN library
#include <arduinoLoRaWAN.h>

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
uint8_t port = 3;

// Define data payload to send (maximum is up to data rate)
char data[] = "010203";

// variable
uint8_t error;

void printAnswer(uint8_t ans)
{
	char cret[30];
	cret[0]='\0';

	switch(ans)
	{
		case LORAWAN_ANSWER_OK:
			sprintf(cret,"LORAWAN_ANSWER_OK\r\n");
		break;
		case LORAWAN_ANSWER_ERROR:
			sprintf(cret,"LORAWAN_ANSWER_ERROR\r\n");
		break;
		case LORAWAN_NO_ANSWER:
			sprintf(cret,"LORAWAN_NO_ANSWER\r\n");
		break;
		case LORAWAN_INIT_ERROR:
			sprintf(cret,"LORAWAN_INIT_ERROR\r\n");
		break;
		case LORAWAN_LENGTH_ERROR:
			sprintf(cret,"LORAWAN_LENGTH_ERROR\r\n");
		break;
		case LORAWAN_SENDING_ERROR:
			sprintf(cret,"LORAWAN_SENDING_ERROR\r\n");
		break;
		case LORAWAN_NOT_JOINED:
			sprintf(cret,"LORAWAN_NOT_JOINED\r\n");
		break;
		case LORAWAN_INPUT_ERROR:
			sprintf(cret,"LORAWAN_INPUT_ERROR\r\n");
		break;
		case LORAWAN_VERSION_ERROR:
			sprintf(cret,"LORAWAN_VERSION_ERROR\r\n");
		break;
	}

	Serial.print(cret);
}

void alarmMatch()
{

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

	Serial.begin(9600);
//	Serial1.begin(57600);

	digitalWrite(LED, HIGH);
	delay(10000);
	digitalWrite(LED, LOW);

    //------------------------------------
    //Module configuration
    //------------------------------------

    //////////////////////////////////////////////
    // 1. Switch on
    //////////////////////////////////////////////
    error = LoRaWAN.ON(uart);

    Serial.print("1. Switch on: ");
    printAnswer(error);

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
//
//    error = LoRaWAN.setDataRate(5);
//    printAnswer(error);
//    Serial.print("LoRaWAN._dataRate: ");
//    Serial.println(LoRaWAN._dataRate);
//
//    error = LoRaWAN.setRetries(0);
//    printAnswer(error);
//    Serial.print("LoRaWAN._dataRate: ");
//    Serial.println(LoRaWAN._dataRate);

    //////////////////////////////////////////////
    // 6. Join network
    //////////////////////////////////////////////
//    error = LoRaWAN.joinOTAA();
//    printAnswer(error);

    error = LoRaWAN.sleep(60000);
    printAnswer(error);

//    Serial1.print("sys sleep 60000\r\n");

    //////////////////////////////////////////////
    // 7. Switch off
    //////////////////////////////////////////////
//    error = LoRaWAN.OFF(uart);
//    Serial.print("Switch off: ");
//    printAnswer(error);

    Serial.println("Start sleep");
	Serial.flush();
    Serial.end();
//    Serial1.end();

//    pinMode(0, INPUT_PULLUP);
//    pinMode(1, INPUT_PULLUP);


    // RTC initialization
    rtc.begin();
    rtc.setTime(hours, minutes, seconds);
    rtc.setDate(day, month, year);

    // RTC alarm setting on every 15 s resulting in 1 minute sleep period
    rtc.setAlarmSeconds(10);
    rtc.enableAlarm(rtc.MATCH_SS);
    rtc.attachInterrupt(alarmMatch);

    digitalWrite(LED, LOW);
    USBDevice.detach();
    rtc.standbyMode();

    // Initialize USB and attach to host (not required if not in use)
    USBDevice.init();
    USBDevice.attach();

	digitalWrite(LED, HIGH);
	delay(5000);
	digitalWrite(LED, LOW);

    error = LoRaWAN.wakeUP();
    printAnswer(error);

//	error = LoRaWAN.check();
//	printAnswer(error);

//	Serial1.write((uint8_t)0x00);
//	Serial1.write((uint8_t)0x55);

	delay(100);

	error = LoRaWAN.check();
	printAnswer(error);

}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
}



