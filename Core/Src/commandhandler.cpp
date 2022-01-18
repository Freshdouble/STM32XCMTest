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
	auto buffer = smp.GetBuffer();
	auto [command, offset, remainingbytes] = GetMatchedCommand(buffer, length);

	switch(command)
	{
	case Commands::SetLEDStatus:
	{
		SetLEDStatus ledstatus;
		bool valid = std::get<1>(ledstatus.Unserialize<buffer.max_size()>(buffer.begin() + offset, buffer.end(), remainingbytes));
		if(valid)
		{
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, ledstatus.argument.Read_ledstatus() ? GPIO_PIN_SET : GPIO_PIN_RESET);
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
	if(HAL_GetTick() > (lastTimestamp + 1000))
	{
		bool led = HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin) == GPIO_PIN_SET;
		status.argument.Write_ledstatus(led);
		std::array<uint8_t, LEDStatus::packetLength> data;
		auto size = status.SerializeWithID(data);
		smp.SendData<LEDStatus::packetLength, sendOverUART>(data, size);
		lastTimestamp = HAL_GetTick();
	}
}

#ifdef __cplusplus
}
#endif
