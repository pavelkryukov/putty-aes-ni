# Makefile for unit tests
#
# @author kryukov@frtk.ru
# @version 2.0
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

CC=gcc
CFLAGS=-Wall -Werror -maes -msse4 -std=c89 -pedantic -Wno-long-long -I ../Putty
OPT=-O3
NOOPT=-O3

TARGETS=sshaes-test sshaesni-test sshaes-perf sshaesni-perf

all: $(TARGETS)

%-perf: ../Putty/%.c aesperf.o puttymem.o
	$(CC) $(OPT) $(CFLAGS) $^ -o $@

%-test: ../Putty/%.c aestest.o puttymem.o
	$(CC) $(OPT) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(OPT) $(CFLAGS) $^ -c -o $@

cpuid: cpuid.c
	$(CC) $(OPT) $(CFLAGS) $^ -o $@ -DSILENT

clean:
	rm $(TARGETS) cpuid *.exe
