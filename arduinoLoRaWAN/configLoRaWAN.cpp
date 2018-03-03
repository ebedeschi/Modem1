/*
 * configLoRaWAN.cpp
 *
 *  Created on: 28 feb 2018
 *      Author: Emanuele
 */

#include "configLoRaWAN.h"
#include <Arduino.h>

const uint32_t chFreq[16] = {0, 0, 0, 867100000, 867300000, 867500000, 867700000, 867900000, 0, 0, 0, 0, 0, 0, 0, 0};
const uint8_t minDR = 0;
const uint8_t maxDR = 5;
const uint16_t dcycle = 0;


uint8_t configChDefault(arduinoLoRaWAN LoRaWAN)
{
	uint8_t error1 = 0;
	uint8_t error[5];
	uint8_t i = 0;
	memset(error,0x00,sizeof(error));

	for (i=3;i<=7;i++)
	{
		error1 = 0;
		error1 = LoRaWAN.setChannelDRRange(i, minDR, maxDR);
		arduinoLoRaWAN::printAnswer(error1);
		error[i]+=error1;
		error1 = LoRaWAN.setChannelFreq(i, chFreq[i]);
		arduinoLoRaWAN::printAnswer(error1);
		error[i]+=error1;
		error1 = LoRaWAN.setChannelDutyCycle(i, dcycle);
		arduinoLoRaWAN::printAnswer(error1);
		error[i]+=error1;
		error1 = LoRaWAN.setChannelStatus(i, "on");
		arduinoLoRaWAN::printAnswer(error1);
		error[i]+=error1;
	}

	error1 = LoRaWAN.saveConfig();
	arduinoLoRaWAN::printAnswer(error1);
	Serial.println("Save configuration");

	for (i=3;i<=7;i++)
	{
		error1 = 0;
		error1 = LoRaWAN.getChannelDRRange(i);
		arduinoLoRaWAN::printAnswer(error1);
		Serial.println("LoRaWAN._drrMin: " + LoRaWAN._drrMin[i]);
		Serial.println("LoRaWAN._drrMax: " + LoRaWAN._drrMax[i]);
		error1 = LoRaWAN.getChannelFreq(i);
		arduinoLoRaWAN::printAnswer(error1);
		Serial.println("LoRaWAN._freq: " + LoRaWAN._freq[i]);
		error1 = LoRaWAN.getChannelDutyCycle(i);
		arduinoLoRaWAN::printAnswer(error1);
		Serial.println("LoRaWAN._dCycle: " + LoRaWAN._dCycle[i]);
		error1 = LoRaWAN.getChannelStatus(i);
		arduinoLoRaWAN::printAnswer(error1);
		Serial.println("LoRaWAN._status: " + LoRaWAN._status[i]);
	}

	return LORAWAN_ANSWER_OK;
}


