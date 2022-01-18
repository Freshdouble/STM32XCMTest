#include "dmaRingBuffer.h"
#include "main.h"
#include <string.h>

 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

 #define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

void dma_Init(dmaRingBuffer_t* dma, DMA_HandleTypeDef* dmaInstance, uint8_t* buffer, uint32_t bufferLength)
{
	dma->dma = dmaInstance;
	dma->readPtr = dma->buffer = buffer;
	dma->bufferLen = bufferLength;
	dma->bufferEnd = buffer + (bufferLength - 1);
}

uint32_t dma_ReadBytes(dmaRingBuffer_t* dma, uint8_t* buffer, uint32_t length)
{
	uint32_t bytesToRead = min(length,dma_GetBytesToRead(dma));
	uint32_t ret = bytesToRead;
	//Read until the end of the buffer
	uint32_t blockLength = min(dma->bufferLen - dma_GetReadIndex(dma), bytesToRead);
	memcpy(buffer, dma->readPtr, blockLength);
	bytesToRead -= blockLength;
	//Read remaining bytes
	memcpy(&buffer[blockLength], dma->buffer, bytesToRead);
	dma->readPtr = dma->readPtr + ret;
	while(dma->readPtr > dma->bufferEnd)
	{
		dma->readPtr = dma->readPtr - dma->bufferLen;
	}
	return ret;
}
