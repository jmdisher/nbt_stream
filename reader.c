#include "reader.h"

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "buffer.h"


// The high-level tag type used internally.
typedef enum
{
	TAG_End = 0,
	TAG_Byte = 1,
	TAG_Short = 2,
	TAG_Int = 3,
	TAG_Long = 4,
	TAG_Float = 5,
	TAG_Double = 6,
	TAG_Byte_Array = 7,
	TAG_String = 8,
	TAG_List = 9,
	TAG_Compound = 10,
	TAG_Int_Array = 11,
	TAG_Long_Array = 12,
	TAG_LENGTH,
} tag_t;


// Prototypes for internal functions (uses the "_" prefix so it is clear what is being called).
static tag_t _readTag(buffer_t *buffer);
static tag_t _parseOneTag(buffer_t *buffer, int indentation, bool hasName);
static void _processOneTag(buffer_t *buffer, int indentation, tag_t tag, bool hasName);

static void _parseTag_Byte(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_Short(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_Int(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_Long(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_Float(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_Double(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_Byte_Array(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_String(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_List(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_Compound(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_Int_Array(buffer_t *buffer, int indentation, bool hasName);
static void _parseTag_Long_Array(buffer_t *buffer, int indentation, bool hasName);

static void _printName(int indentation, const char *tagType, buffer_t *buffer, bool hasName);
static void _printIndentation(int indentation);
static void _printString(buffer_t *buffer, int16_t length);


// These unions are used for type conversions between int/long and float/double.
typedef union
{
	int32_t i;
	float f;
} int_to_float_t;

typedef union
{
	int64_t i;
	double f;
} long_to_double_t;


void parseOneTag(buffer_t *buffer)
{
	// This is the root entry-point of the state machine.  We always start at a depth of 0, looking for a tag to process.
	_parseOneTag(buffer, 0, true);
}


tag_t _readTag(buffer_t *buffer)
{
	uint8_t b = readByte(buffer);
	assert (b < TAG_LENGTH);
	return (tag_t) b;
}

tag_t _parseOneTag(buffer_t *buffer, int indentation, bool hasName)
{
	tag_t tag = _readTag(buffer);
	_processOneTag(buffer, indentation, tag, hasName);
	return tag;
}

void _processOneTag(buffer_t *buffer, int indentation, tag_t tag, bool hasName)
{
	switch (tag)
	{
		case TAG_End:
			// This has no data so we just return after writing something to show we ended the structure.
			// (we back up one indentation so that the output looks balanced)
			_printIndentation(indentation - 1);
			printf("TAG_End\n");
			break;
		case TAG_Byte:
			_parseTag_Byte(buffer, indentation, hasName);
			break;
		case TAG_Short:
			_parseTag_Short(buffer, indentation, hasName);
			break;
		case TAG_Int:
			_parseTag_Int(buffer, indentation, hasName);
			break;
		case TAG_Long:
			_parseTag_Long(buffer, indentation, hasName);
			break;
		case TAG_Float:
			_parseTag_Float(buffer, indentation, hasName);
			break;
		case TAG_Double:
			_parseTag_Double(buffer, indentation, hasName);
			break;
		case TAG_Byte_Array:
			_parseTag_Byte_Array(buffer, indentation, hasName);
			break;
		case TAG_String:
			_parseTag_String(buffer, indentation, hasName);
			break;
		case TAG_List:
			_parseTag_List(buffer, indentation, hasName);
			break;
		case TAG_Compound:
			_parseTag_Compound(buffer, indentation, hasName);
			break;
		case TAG_Int_Array:
			_parseTag_Int_Array(buffer, indentation, hasName);
			break;
		case TAG_Long_Array:
			_parseTag_Long_Array(buffer, indentation, hasName);
			break;
		default:
			assert(false);
	}
}

void _parseTag_Byte(buffer_t *buffer, int indentation, bool hasName)
{
	_printName(indentation, "TAG_Byte", buffer, hasName);
	uint8_t b = readByte(buffer);
	printf(": %d (0x%02x)\n", (int)b, (int)b);
}

void _parseTag_Short(buffer_t *buffer, int indentation, bool hasName)
{
	_printName(indentation, "TAG_Short", buffer, hasName);
	int16_t s = readShort(buffer);
	printf(": %d (0x%04x)\n", (int)s, (int)s);
}

void _parseTag_Int(buffer_t *buffer, int indentation, bool hasName)
{
	_printName(indentation, "TAG_Int", buffer, hasName);
	int32_t i = readInt(buffer);
	printf(": %d (0x%08x)\n", i, i);
}

void _parseTag_Long(buffer_t *buffer, int indentation, bool hasName)
{
	_printName(indentation, "TAG_Long", buffer, hasName);
	int64_t i = readLong(buffer);
	printf(": %ld (0x%016lx)\n", i, i);
}

void _parseTag_Float(buffer_t *buffer, int indentation, bool hasName)
{
	_printName(indentation, "TAG_Float", buffer, hasName);
	int_to_float_t converter;
	converter.i = readInt(buffer);
	printf(": %0.02f (0x%08x)\n", converter.f, converter.i);
}

void _parseTag_Double(buffer_t *buffer, int indentation, bool hasName)
{
	_printName(indentation, "TAG_Double", buffer, hasName);
	long_to_double_t converter;
	converter.i = readLong(buffer);
	printf(": %0.02f (0x%016lx)\n", converter.f, converter.i);
}

void _parseTag_Byte_Array(buffer_t *buffer, int indentation, bool hasName)
{
	_printName(indentation, "TAG_Byte_Array", buffer, hasName);
	int32_t count = readInt(buffer);
	printf(": [");
	bool comma = false;
	for (int32_t i = 0; i < count; ++i)
	{
		if (comma)
		{
			printf(", ");
		}
		uint8_t b = readByte(buffer);
		printf("0x%02x", (uint32_t) b);
		comma = true;
	}
	printf("]\n");
}

void _parseTag_String(buffer_t *buffer, int indentation, bool hasName)
{
	_printName(indentation, "TAG_String", buffer, hasName);
	uint16_t length = readShort(buffer);
	printf(": \"");
	_printString(buffer, length);
	printf("\"\n");
}

void _parseTag_List(buffer_t *buffer, int indentation, bool hasName)
{
	_printName(indentation, "TAG_List", buffer, hasName);
	tag_t tag = _readTag(buffer);
	int32_t length = readInt(buffer);
	printf(" %d entries\n", length);
	for (int32_t i = 0; i < length; ++i)
	{
		_processOneTag(buffer, indentation + 1, tag, false);
	}
}

void _parseTag_Compound(buffer_t *buffer, int indentation, bool hasName)
{
	_printName(indentation, "TAG_Compound", buffer, hasName);
	printf("\n");
	tag_t tag = TAG_LENGTH;
	while (TAG_End != tag)
	{
		tag = _parseOneTag(buffer, indentation + 1, true);
	}
}

void _parseTag_Int_Array(buffer_t *buffer, int indentation, bool hasName)
{
	//WARNING:  Not tested!
	_printName(indentation, "TAG_Int_Array", buffer, hasName);
	int32_t count = readInt(buffer);
	printf(": [");
	bool comma = false;
	for (int32_t i = 0; i < count; ++i)
	{
		if (comma)
		{
			printf(", ");
		}
		int32_t b = readInt(buffer);
		printf("0x%08x", b);
		comma = true;
	}
	printf("]\n");
}

void _parseTag_Long_Array(buffer_t *buffer, int indentation, bool hasName)
{
	//WARNING:  Not tested!
	_printName(indentation, "TAG_Long_Array", buffer, hasName);
	int32_t count = readInt(buffer);
	printf(": [");
	bool comma = false;
	for (int32_t i = 0; i < count; ++i)
	{
		if (comma)
		{
			printf(", ");
		}
		int64_t b = readLong(buffer);
		printf("0x%016lx", b);
		comma = true;
	}
	printf("]\n");
}

void _printName(int indentation, const char *tagType, buffer_t *buffer, bool hasName)
{
	_printIndentation(indentation);
	if (hasName)
	{
		int16_t nameLength = readShort(buffer);
		printf("%s(\"", tagType);
		_printString(buffer, nameLength);
		printf("\")");
	}
	else
	{
		printf("%s", tagType);
	}
}

void _printIndentation(int indentation)
{
	for (int i =0; i < indentation; ++i)
	{
		printf("\t");
	}
}

void _printString(buffer_t *buffer, int16_t length)
{
	ensureAvailable(buffer, length);
	printf("%.*s", length, &buffer->buffer[buffer->offset]);
	skipAvailable(buffer, length);
}


