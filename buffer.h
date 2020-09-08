#if !defined(__BUFFER_H__)
#define __BUFFER_H__

#include <unistd.h>
#include <inttypes.h>

// 64 KiB - largest single data element is 32 KiB String so this is plenty of space but still pretty small.
#define BUFFER_SIZE (64 * 1024)

// The long-lived state representation for the reader.
typedef struct
{
	// The working buffer which will be used.
	uint8_t buffer[BUFFER_SIZE];
	// The file descriptor from which more data is pulled.
	int fd;
	// The offset into buffer where uninitialized data begins (always >= offset and <= BUFFER_SIZE).
	size_t limit;
	// The offset of the next byte to return (always >= 0 and <= limit).
	size_t offset;
	// A counter for the number of bytes consumed during processing (total number read might be greater).
	size_t bytesConsumed;
} buffer_t;

// Ensures at least the next size bytes are available in the buffer.
// Assert fails if the buffer cannot be filled at least that much.
void ensureAvailable(buffer_t *buffer, size_t size);

// Skips over the next size bytes of the buffer, considering them consumed.
void skipAvailable(buffer_t *buffer, size_t size);

// Reads a single unsigned byte, advancing the buffer, considering it consumed.
uint8_t readByte(buffer_t *buffer);

// Reads a 2-byte signed integer, advancing the buffer, considering it consumed.
int16_t readShort(buffer_t *buffer);

// Reads a 4-byte signed integer, advancing the buffer, considering it consumed.
int32_t readInt(buffer_t *buffer);

// Reads a 8-byte signed integer, advancing the buffer, considering it consumed.
int64_t readLong(buffer_t *buffer);

#endif // __BUFFER_H__
