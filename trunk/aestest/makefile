# Makefile for unit tests
#
# @author kryukov@frtk.ru
# @version 1.0
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

CC:=gcc
CFLAGS:=-O3 -Wall -Werror -maes -std=c89 -pedantic -I ../Putty

all: sshaes sshaesni

%: ../Putty/%.c aestest.o puttymem.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -c -o $@

clean:
	rm sshaes
	rm sshaesni

