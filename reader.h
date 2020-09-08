#if !defined(__READER_H__)
#define __READER_H__

#include "buffer.h"

// Parses and fully processes a single tag starting where the given buffer is currently pointing.
// Returns once the tag is fully processed (fails if something unexpected happens).
void parseOneTag(buffer_t *buffer);

#endif // __READER_H__
