# Makefile for unit tests
#
# @author kryukov@frtk.ru
# @version 4.1
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

CC=gcc
CFLAGS=-O3 -Wall -Werror -maes -msse4 -std=c89 -pedantic -Wno-long-long
LDFLAGS=
TARGETS=perf demo demo-decode test
PUTTY=../Putty
OBJ_DIR=obj
BIN_DIR=bin
TXT_DIR=txt
SRC_DIR=src

ifeq ($(SDE), yes)
    SDERUN=sde --
else
    SDERUN=
endif

.SECONDARY:
	

$(BIN_DIR)/sshaes-%: $(OBJ_DIR)/sshaes.o $(OBJ_DIR)/aes%.o $(OBJ_DIR)/puttymem.o
	$(CC) $(LDFLAGS) $^ -o $@

$(BIN_DIR)/sshaesni-%: $(OBJ_DIR)/sshaesni.o $(OBJ_DIR)/aes%.o $(OBJ_DIR)/puttymem.o
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/sshaes.o: $(PUTTY)/sshaes.c
	$(CC) $(CFLAGS) $^ -c -o $@ -I $(PUTTY) -D_FORCE_SOFTWARE_AES
    
$(OBJ_DIR)/sshaesni.o: $(PUTTY)/sshaes.c
	$(CC) $(CFLAGS) $^ -c -o $@ -I $(PUTTY)
    
$(OBJ_DIR)/aesdemo-decode.o: $(SRC_DIR)/aesdemo.c
	$(CC) $(CFLAGS) $^ -c -o $@ -DDECODE

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $^ -c -o $@

$(BIN_DIR)/cpuid: $(SRC_DIR)/cpuid.c
	$(CC) $(CFLAGS) $^ -o $@ -DSILENT

$(TXT_DIR)/test-original-%.txt: $(BIN_DIR)/sshaes-test
	$^ $*
	mv test-output.txt $@

$(TXT_DIR)/test-output-%.txt: $(BIN_DIR)/sshaesni-test
	$(SDERUN) $^ $*
	mv test-output.txt $@

$(TXT_DIR)/perf-original.txt: $(BIN_DIR)/sshaes-perf
	$^
	mv perf-output.txt $@

$(TXT_DIR)/perf-output.txt: $(BIN_DIR)/sshaesni-perf
	$(SDERUN) $^
	mv perf-output.txt $@

$(TXT_DIR)/%.sorted.txt: $(TXT_DIR)/%.txt
	sort $^ > $@
  
clean:
	rm $(BIN_DIR) $(OBJ_DIR) $(TXT_DIR) -rf
