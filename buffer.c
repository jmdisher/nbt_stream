#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "buffer.h"


// Prototypes for internal functions (uses the "_" prefix so it is clear what is being called).
static void _ensureAvailable(buffer_t *buffer, size_t size);
static int _fillBuffer(buffer_t *buffer);


void ensureAvailable(buffer_t *buffer, size_t size)
{
	_ensureAvailable(buffer, size);
}

void skipAvailable(buffer_t *buffer, size_t size)
{
	_ensureAvailable(buffer, size);
	buffer->offset += size;
	buffer->bytesConsumed += size;
}

uint8_t readByte(buffer_t *buffer)
{
	_ensureAvailable(buffer, 1);
	uint8_t b = buffer->buffer[buffer->offset];
	buffer->offset += 1;
	buffer->bytesConsumed += 1;
	return b;
}

int16_t readShort(buffer_t *buffer)
{
	_ensureAvailable(buffer, 2);
	uint8_t b1 = buffer->buffer[buffer->offset];
	uint8_t b0 = buffer->buffer[buffer->offset + 1];
	buffer->offset += 2;
	buffer->bytesConsumed += 2;
	int16_t s = (b1 << 8)
		| (b0)
	;
	return s;
}

int32_t readInt(buffer_t *buffer)
{
	_ensureAvailable(buffer, 4);
	uint8_t b3 = buffer->buffer[buffer->offset];
	uint8_t b2 = buffer->buffer[buffer->offset + 1];
	uint8_t b1 = buffer->buffer[buffer->offset + 2];
	uint8_t b0 = buffer->buffer[buffer->offset + 3];
	buffer->offset += 4;
	buffer->bytesConsumed += 4;
	int32_t i = (b3 << 24)
		| (b2 << 16)
		| (b1 << 8)
		| (b0)
	;
	return i;
}

int64_t readLong(buffer_t *buffer)
{
	_ensureAvailable(buffer, 8);
	uint8_t b7 = buffer->buffer[buffer->offset];
	uint8_t b6 = buffer->buffer[buffer->offset + 1];
	uint8_t b5 = buffer->buffer[buffer->offset + 2];
	uint8_t b4 = buffer->buffer[buffer->offset + 3];
	uint8_t b3 = buffer->buffer[buffer->offset + 4];
	uint8_t b2 = buffer->buffer[buffer->offset + 5];
	uint8_t b1 = buffer->buffer[buffer->offset + 6];
	uint8_t b0 = buffer->buffer[buffer->offset + 7];
	buffer->offset += 8;
	buffer->bytesConsumed += 8;
	int64_t i = ((uint64_t)b7 << 56)
		| ((uint64_t)b6 << 48)
		| ((uint64_t)b5 << 40)
		| ((uint64_t)b4 << 32)
		| (b3 << 24)
		| (b2 << 16)
		| (b1 << 8)
		| (b0)
	;
	return i;
}


void _ensureAvailable(buffer_t *buffer, size_t size)
{
	if ((buffer->limit - buffer->offset) < size)
	{
		_fillBuffer(buffer);
		assert ((buffer->limit - buffer->offset) >= size);
	}
}

int _fillBuffer(buffer_t *buffer)
{
	int bytesToMove = buffer->limit - buffer->offset;
	assert (bytesToMove >= 0);
	if (bytesToMove > 0)
	{
		memmove(buffer->buffer, &buffer->buffer[buffer->offset], bytesToMove);
		buffer->limit -= bytesToMove;
		buffer->offset = 0;
	}
	int spaceInBuffer = BUFFER_SIZE - buffer->limit;
	assert (spaceInBuffer > 0);
	int readSize = (int) read(buffer->fd, &buffer->buffer[buffer->offset], spaceInBuffer);
	if (readSize > 0)
	{
		buffer->limit += readSize;
	}
	return readSize;
}

