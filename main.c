#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>

#include "buffer.h"
#include "reader.h"


// Only 1 command-line option, which is optional:  number of bytes to skip (will be interpreted as decimal, octal, or hex, depending on how it is formatted).
int main(int argc, char **argv)
{
	size_t bytesToSkip = (1 == argc)
		? 0
		: strtol(argv[1], NULL, 0)
	;
	if (0 != errno)
	{
		printf("Invalid option given: \"%s\"\n", argv[1]);
		exit(1);
	}
	else
	{
		printf("Skipping 0x%lx bytes\n", bytesToSkip);
		// We will avoid doing any sort of optimal chunking and just read 1 byte at a time - slow, but simple.
		for (int i = 0; i < bytesToSkip; ++i)
		{
			unsigned char f;
			int check = read(STDIN_FILENO, &f, 1);
			if (1 != check)
			{
				printf("Premature end of file\n");
				exit(2);
			}
		}

		buffer_t buffer;
		buffer.fd = STDIN_FILENO;
		buffer.limit = 0;
		buffer.offset = 0;

		parseOneTag(&buffer);
		printf("Total NBT structure 0x%lx bytes\n", buffer.bytesConsumed);
	}
}
