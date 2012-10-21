# Makefile for unit tests
#
# @author kryukov@frtk.ru
# @version 2.0
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

CC=gcc
CFLAGS=-Wall -Werror -maes -msse4 -std=c89 -pedantic -Wno-long-long
OPT=-O3
NOOPT=-O0

TARGETS=sshaes-test sshaesni-test sshaes-perf sshaesni-perf

all: $(TARGETS)

%-perf: ../Putty/%.c aesperf.o puttymem.o
	$(CC) $(OPT) $(CFLAGS) $^ -o $@ -I ../Putty
    
%-demo-encode: ../Putty/%.c aesdemo.o puttymem.o
	$(CC) $(OPT) $(CFLAGS) $^ -o $@ -I ../Putty
    
%-demo-decode: ../Putty/%.c aesdemo.o puttymem.o -DDECODE
	$(CC) $(OPT) $(CFLAGS) $^ -o $@ -I ../Putty

%-test: ../Putty/%.c aestest.o puttymem.o
	$(CC) $(OPT) $(CFLAGS) $^ -o $@ -I ../Putty

%.o: %.c
	$(CC) $(OPT) $(CFLAGS) $^ -c -o $@

cpuid: cpuid.c
	$(CC) $(OPT) $(CFLAGS) $^ -o $@ -DSILENT

clean:
	rm $(TARGETS) cpuid *.exe
