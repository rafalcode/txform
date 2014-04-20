CC=gcc
CFLAGS=-g -Wall
EXECUTABLES=txform

txform: txform.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
