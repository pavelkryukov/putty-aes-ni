CC:=gcc
CFLAGS:=-O3 -Wall -std=c89 -pedantic -I ../Putty

all: sshaes sshaesni

%: ../Putty/%.c aestest.o puttymem.o
	$(CC) $(CFLAGS) $^ -o $@
	
%.o: %.c
	$(CC) $(CFLAGS) $^ -c -o $@
