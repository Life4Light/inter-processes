CC = gcc
CFLAGS = -Wall -g


all: reader xor

reader: src/reader.c
	$(CC) $(CFLAGS) -o reader src/reader.c

xor: src/xor.c
	$(CC) $(CFLAGS) -o xor src/xor.c

clean:
	rm -f reader xor reader.sock

test:
	./xor file1.txt file2.txt output.txt
	./xor output.txt file2.txt result.txt