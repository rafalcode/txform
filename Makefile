CC=gcc
CFLAGS=-g -Wall
EXECUTABLES=txform phaseform

txform: txform.c
	$(CC) $(CFLAGS) -o $@ $^

phaseform: phaseform.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}
