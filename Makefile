##
# raw2pixflood
CC= gcc
CFLAGS = -Wall
ALLCFLAGS = $(CFLAGS) $(shell echo | gcc -xc -E -v - 2>&1 | grep -E '^\s' | sed '1d;s/^\s/ -I/' | tr '\n' ' ') # Explictly include system libraries for cdb
OBJ= raw2pixflood.o

raw2pixflood: $(OBJ)
	$(CC) $(ALLCFLAGS) $(OBJ) -o raw2pixflood

run: raw2pixflood
	./raw2pixflood $(ARGS)

clean:
	rm $(OBJ) raw2pixflood

%.o: %.c
	$(CC) $(ALLCFLAGS) -c $< -o $@

.PHONY: raw2pixflood run clean

# end
