/*
 * commandhandler.cpp
 *
 *  Created on: Jan 17, 2022
 *      Author: peter
 */

#include "main.h"
#include "commandhandler.hpp"
#include "libsmp.hpp"
#include "xcode.hpp"

extern UART_HandleTypeDef huart3;

#ifdef __cplusplus
extern "C" {
#endif
signed char SMPReceiveFunc(uint8_t *data, uint32_t length);
#ifdef __cplusplus
}
#endif
static SMP<100> smp(SMPReceiveFunc);

size_t sendOverUART(const uint8_t* data, size_t length)
{
	return HAL_UART_Transmit(&huart3, data, static_cast<uint16_t>(length), 100) == HAL_OK ? length : 0;
}

#ifdef __cplusplus
extern "C" {
#endif

signed char SMPReceiveFunc(uint8_t *data, uint32_t length)
{
	auto buffer = smp.GetBuffer(); //Get the smp receive buffer. We can use this buffer directly because the smp receiver starts writing at the base address.
	//TODO: Evaluate iterator based command matching to be independendt of the actual write position of libsmp
	auto [command, offset, remainingbytes] = GetMatchedCommand(buffer, length); //Get the command that matches the data header

	switch(command)
	{
	case Commands::SetLEDStatus:
	{
		SetLEDStatus ledstatus;
		bool valid = std::get<1>(ledstatus.Unserialize<buffer.max_size()>(buffer.begin() + offset, buffer.end(), remainingbytes)); //Check if the data is valid. If this is false it is possible that the generated xcode file is older than the latest transmission.xml definition file
		if(valid)
		{
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, ledstatus.argument.Read_ledstatus() ? GPIO_PIN_SET : GPIO_PIN_RESET); //Set the output according to the command
		}
		break;
	}
	case Commands::Unkown:
		break;
	}

	return 0;
}

void parseInput(uint8_t* buffer, size_t length)
{
	smp.ReceiveData(buffer, length);
}

void loop()
{
	static uint32_t lastTimestamp = 0;
	LEDStatus status;
	if(HAL_GetTick() > (lastTimestamp + 1000)) //Check if 1s passed in a non blocking way so we don't stall the uart reception
	{
		bool led = HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin) == GPIO_PIN_SET;
		status.argument.Write_ledstatus(led);
		std::array<uint8_t, LEDStatus::packetLength> data;
		auto size = status.SerializeWithID(data); //Serialize the data to the packet
		smp.SendData<LEDStatus::packetLength, sendOverUART>(data, size); //Transmit the packet using the sendOverUART function.
		lastTimestamp = HAL_GetTick();
	}
}

#ifdef __cplusplus
}
#endif
