CC=gcc
CCFLAGS=-Wall -g
LDFLAGS=
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TARGET=ultra-cp

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CC) -o $@ $^ $(LDFLAGS) 

%.o: %.c %.h
		$(CC) $(CCFLAGS) -c $<

%.o: %.c
		$(CC) $(CCFLAGS) -c $<

clean:
		rm -f *.o $(TARGET)

wc: 
		wc -l *.c *.h

zip:
		zip nicolas_boeckh.sofia_delijaj.tp4.zip *.c *.h Makefile README.md