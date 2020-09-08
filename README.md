# NBT Stream

This is a really simple utility for converting streams of [Minecraft NBT (Named Binary Tag) data](https://minecraft.gamepedia.com/NBT_format) into something human-readable.

This tool is not very versatile as it is only something I needed in order to inspect what the Minecraft 1.16.2 server sends over the wire so that I could provide the same information shape in another tool I am writing.

It is posted as open source in case anyone else needed to look into a very minimal interpretation of the encoding, since this project is designed to do little other than just read and display a single tag.

## Why?

I was extracting data with [netcat](https://en.wikipedia.org/wiki/Netcat) and reading it with [bvi](http://bvi.sourceforge.net/), which is fine for examining structure of most messages but NBT data was too large to follow in this way.  I looked into some of the other tools but they either had large dependencies or couldn't handle reading from a file which may have other data after the NBT data (since I didn't know how long it was - I was just looking at the binary stream from the server).

Hence, I figured it would be a fun way to spend a day as I missed working in C or writing zero-allocation algorithms.

## Building

Running the default target will clean, compile, link, and test the binary (called `nbt`):

```
make
```

The tests are just invocations of the binary on the captured output of a `Join Game` packet from a 1.16.2 server.  They just show that it seeks correctly and finds a correctly-sized NBT.

## Usage

The tool operates only on STDIN and STDOUT, with one optional parameter to specify a number of bytes it should skip from input before it begins decoding the stream.

As an illustration, consider this example from `tests/test.sh`:  `cat join_game.dat | ../nbt 0x47`.  This means it will decode the stream in `join_game.dat` after skipping the first 0x47 bytes of the stream (since that is the address where the first NBT starts).

The human-readable interpretation is written to STDOUT.

Since this just operates on streams, this also means the compressed examples can be read by piping them through something like `gunzip`, instead of the tool duplicating this functionality.

## About the code

The code is pretty basic pure C99 and I verified it builds on GCC version 9.3.0 on Linux x86-64.

There are 3 components:

* `main.c` - Includes command-line processing, buffer allocation, and advancing the stream before starting.
* `buffer.c` - Includes the buffer which sits on top of STDIN and provides various high-level reading facilities.
* `reader.c` - The actual NBT reader state machine and logic, as well as where all output is produced.

Unexpected/unhandled cases `assert` and the only command-line option (skip offset) is checked before start-up and should fail out correctly.

Note that much of the string reading assumes the buffer isn't changing out from under it, so be careful with any reordering of function calls around there.

Much of the implementation was based on documentation here:  https://wiki.vg/NBT

## Maintenance

I will update this if I need to adapt some part of it but it should largely be considered "done".
