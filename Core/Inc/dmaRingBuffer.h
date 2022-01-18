#pragma once

#include "main.h"

typedef struct
{
	DMA_HandleTypeDef* dma;
	uint8_t* readPtr;
	uint8_t* buffer;
	uint32_t bufferLen;
	uint8_t* bufferEnd;
}dmaRingBuffer_t;

static inline uint8_t* dma_getWritePtr(dmaRingBuffer_t* dmaInstance)
{
	return &dmaInstance->buffer[dmaInstance->bufferLen - dmaInstance->dma->Instance->NDTR];
}

static inline uint32_t dma_GetBytesToRead(dmaRingBuffer_t* dmaInstance)
{
	int32_t rawBytesToRead = (uint32_t)dma_getWritePtr(dmaInstance) - (uint32_t)dmaInstance->readPtr;
	if(rawBytesToRead < 0)
	{
		return rawBytesToRead + dmaInstance->bufferLen;
	}
	else
	{
		return rawBytesToRead;
	}
}

static inline uint32_t dma_GetReadIndex(dmaRingBuffer_t* dmaInstance)
{
	return (uint32_t)dmaInstance->readPtr - (uint32_t)dmaInstance->buffer;
}

static inline uint32_t dma_GetWriteIndex(dmaRingBuffer_t* dmaInstance)
{
	return (uint32_t)dma_getWritePtr(dmaInstance) - (uint32_t)dmaInstance->buffer;
}

void dma_Init(dmaRingBuffer_t* dma, DMA_HandleTypeDef* dmaInstance, uint8_t* buffer, uint32_t bufferLength);
uint32_t dma_ReadBytes(dmaRingBuffer_t* dma, uint8_t* buffer, uint32_t length);