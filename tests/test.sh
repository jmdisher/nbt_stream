#!/bin/sh

COUNT=$(cat join_game.dat | ../nbt 0x47 | grep '^Total NBT structure 0x770e bytes' | wc -l)
if [ $COUNT -eq 1 ]; then
	echo "Matched first NBT"
else
	echo "Failed to find first NBT"
	exit 1
fi

COUNT=$(cat join_game.dat | ../nbt 0x7755 | grep "^Total NBT structure 0x10c bytes" | wc -l)
if [ $COUNT -eq 1 ]; then
	echo "Matched second NBT"
else
	echo "Failed to find second NBT"
	exit 1
fi

