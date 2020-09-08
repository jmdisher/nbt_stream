CFLAGS=-Wall -pedantic -Os

all: clean main test

clean:
	rm -f nbt *.o

main: main.o reader.o buffer.o
	$(CC) $(CFLAGS) -o nbt main.o reader.o buffer.o

test:
	cd tests \
		&& ./test.sh \
		&& echo "Testing Complete" \
		&& cd ..
