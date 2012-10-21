# Makefile for unit tests
#
# @author kryukov@frtk.ru
# @version 3.1
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

CC=gcc
CFLAGS=-O3 -Wall -Werror -maes -msse4 -std=c89 -pedantic -Wno-long-long
LDFLAGS=
TARGETS=perf demo demo-decode test
PUTTY=../Putty

sshaes-%: sshaes.o aes%.o puttymem.o
	$(CC) $(LDFLAGS) $^ -o $@

sshaesni-%: sshaesni.o aes%.o puttymem.o
	$(CC) $(LDFLAGS) $^ -o $@

sshae%.o: $(PUTTY)/sshae%.c
	$(CC) $(CFLAGS) $^ -c -o $@ -I $(PUTTY)
    
aesdemo-decode.o: aesdemo.c
	$(CC) $(CFLAGS) $^ -c -o $@ -DDECODE

%.o: %.c
	$(CC) $(CFLAGS) $^ -c -o $@

cpuid: cpuid.c
	$(CC) $(CFLAGS) $^ -o $@ -DSILENT

clean:
	rm ${TARGETS:%=sshaes-%} ${TARGETS:%=sshaesni-%} cpuid *.exe *.o -f
