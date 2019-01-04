# Makefile for unit tests
#
# @author kryukov@frtk.ru
# @version 4.3
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

CC?=gcc
CFLAGS=-O2 -Wall -Werror -std=c99 -Wno-long-long # -pedantic
LDFLAGS=
PUTTY=./Putty
OBJ_DIR=obj
BIN_DIR=bin
TXT_DIR=txt
SRC_DIR=src
HEADERS=$(SRC_DIR)/coverage.h $(SRC_DIR)/defines.h
MAKEFILE=Makefile

ifeq ($(SDE), yes)
	SDERUN=$(SDE_BIN) -mix --
else
	SDERUN=
endif

.SECONDARY:

# BINARIES
$(BIN_DIR)/sshsha-%: $(OBJ_DIR)/sshshasw.o $(OBJ_DIR)/sshsh256sw.o $(OBJ_DIR)/sha%.o $(OBJ_DIR)/memory.o $(OBJ_DIR)/outof.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/marshal.o
	@echo "[$(LD)] $@"
	@$(CC) $(LDFLAGS) $^ -o $@

$(BIN_DIR)/sshshani-%: $(OBJ_DIR)/sshsha.o $(OBJ_DIR)/sshsh256.o $(OBJ_DIR)/sha%.o $(OBJ_DIR)/memory.o $(OBJ_DIR)/outof.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/marshal.o
	@echo "[$(LD)] $@"
	@$(CC) $(LDFLAGS) $^ -o $@

$(BIN_DIR)/sshaes-%: $(OBJ_DIR)/sshaessw.o $(OBJ_DIR)/aes%.o $(OBJ_DIR)/memory.o $(OBJ_DIR)/outof.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/marshal.o
	@echo "[$(LD)] $@"
	@$(CC) $(LDFLAGS) $^ -o $@

$(BIN_DIR)/sshaesni-%: $(OBJ_DIR)/sshaes.o $(OBJ_DIR)/aes%.o $(OBJ_DIR)/memory.o $(OBJ_DIR)/outof.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/marshal.o
	@echo "[$(LD)] $@"
	@$(CC) $(LDFLAGS) $^ -o $@

# Generic rule
$(OBJ_DIR)/outof.o: $(SRC_DIR)/outof.c $(HEADERS) $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY)

$(OBJ_DIR)/sha%.o: $(SRC_DIR)/sha%.c $(HEADERS) $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY)

$(OBJ_DIR)/aes%.o: $(SRC_DIR)/aes%.c $(HEADERS) $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY)

$(OBJ_DIR)/%.o: $(PUTTY)/%.c $(HEADERS) $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY)

$(OBJ_DIR)/sshsh256sw.o: $(PUTTY)/sshsh256.c $(HEADERS) $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY) -D_FORCE_SOFTWARE_SHA

$(OBJ_DIR)/sshshasw.o: $(PUTTY)/sshsha.c $(HEADERS) $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY) -D_FORCE_SOFTWARE_SHA
       
$(OBJ_DIR)/sshaessw.o: $(PUTTY)/sshaes.c $(HEADERS) $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY) -D_FORCE_SOFTWARE_AES

$(OBJ_DIR)/aesdemo-decode.o: $(SRC_DIR)/aesdemo.c $(HEADERS) $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -DDECODE

# CPUID
$(BIN_DIR)/aescpuid: $(SRC_DIR)/aescpuid.c $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -o $@ -DSILENT
    
$(BIN_DIR)/shacpuid: $(SRC_DIR)/shacpuid.c $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -o $@ -DSILENT

# Outputs
$(TXT_DIR)/test-original-aes-%.txt: $(BIN_DIR)/sshaes-test
	@echo "[$^] $@"
	@$^ $*
	@mv test-output.txt $@

$(TXT_DIR)/test-output-aes-%.txt: $(BIN_DIR)/sshaesni-test
	@rm sde-mix-out.txt -f
	@echo "[$(SDERUN) $^] $@"
	@$(SDERUN) $^ $*
	@if [ -r sde-mix-out.txt ]; then echo "Dump SDE AES stats:"; cat sde-mix-out.txt | grep "^AES" | cat; fi
	@mv test-output.txt $@

$(TXT_DIR)/test-original-sha-%.txt: $(BIN_DIR)/sshsha-test
	@echo "[$^] $@"
	@$^ $*
	@mv test-output.txt $@

$(TXT_DIR)/test-output-sha-%.txt: $(BIN_DIR)/sshshani-test
	@rm sde-mix-out.txt -f
	@echo "[$(SDERUN) $^] $@"
	@$(SDERUN) $^ $*
	@if [ -r sde-mix-out.txt ]; then echo "Dump SDE SHA stats:"; cat sde-mix-out.txt | grep "^SHA" | cat; fi
	@mv test-output.txt $@
    
# Performance outputs
$(TXT_DIR)/perf-original-aes.txt: $(BIN_DIR)/sshaes-perf
	@echo "[run] $@"
	@$^
	@mv perf-output.txt $@

$(TXT_DIR)/perf-output-aes.txt: $(BIN_DIR)/sshaesni-perf
	@echo "[$(SDERUN) run] $@"
	@$(SDERUN) $^
	@mv perf-output.txt $@

$(TXT_DIR)/%.sorted.txt: $(TXT_DIR)/%.txt
	@echo "[sort] $@"
	@sort $^ > $@
    
$(TXT_DIR)/perf-geomean-%.txt: $(TXT_DIR)/perf-original-%.sorted.txt $(TXT_DIR)/perf-output-%.sorted.txt
	@echo "[geomean] $@"
	@python ./geomean.py $^ > $@

clean:
	rm $(BIN_DIR) $(OBJ_DIR) $(TXT_DIR) -rf
