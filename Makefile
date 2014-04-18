CC=gcc
CFLAGS=-g -Wall
EXECUTABLES=txforn

txform: txform.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
