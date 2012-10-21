# Makefile for unit tests
#
# @author kryukov@frtk.ru
# @version 3.0
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

CC=gcc
CFLAGS=-Wall -Werror -maes -msse4 -std=c89 -pedantic -Wno-long-long
OPT=-O3
NOOPT=-O0

TARGETS=sshaes-test sshaesni-test sshaes-perf sshaesni-perf

all: $(TARGETS)

sshaes-%: sshaes.o aes%.o puttymem.o
	$(CC) $^ -o $@

sshaesni-%: sshaesni.o aes%.o puttymem.o
	$(CC) $^ -o $@

sshae%.o: ../Putty/sshae%.c
	$(CC) $(OPT) $(CFLAGS) $^ -c -o $@ -I ../Putty
    
aesdemo-decode.o: aesdemo.c
	$(CC) $(OPT) $(CFLAGS) $^ -c -o $@ -DDECODE

%.o: %.c
	$(CC) $(OPT) $(CFLAGS) $^ -c -o $@

cpuid: cpuid.c
	$(CC) $(OPT) $(CFLAGS) $^ -o $@ -DSILENT

clean:
	rm $(TARGETS) cpuid *.exe
