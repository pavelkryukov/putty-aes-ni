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
MAKEFILE=Makefile

.SECONDARY:

# BINARIES
sshaes-perf: sshaes.o aesperf.o memory.o utils.o marshal.o
	@echo "[$(LD)] $@"
	@$(CC) $(LDFLAGS) $^ -o $@

sshaesni-perf: sshaes.o aesperfni.o memory.o utils.o marshal.o
	@echo "[$(LD)] $@"
	@$(CC) $(LDFLAGS) $^ -o $@
    
sshaes-demo-%: sshaes.o aesdemo-%.o memory.o utils.o marshal.o
	@echo "[$(LD)] $@"
	@$(CC) $(LDFLAGS) $^ -o $@

aescpuid: aescpuid.c $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -o $@ -DSILENT

# Objects
aesperf.o: aesperf.c $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY)

aesperfni.o: aesperf.c $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY) -D_HW_AES

aesdemo-sw-encode.o: aesdemo.c $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY)

aesdemo-sw-decode.o: aesdemo.c $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY) -DDECODE
    
aesdemo-hw-encode.o: aesdemo.c $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY) -DNEW_INSTRUCTIONS

aesdemo-hw-decode.o: aesdemo.c $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY) -DNEW_INSTRUCTIONS -DDECODE

# Putty objects
%.o: $(PUTTY)/%.c $(MAKEFILE)
	@echo "[$(CC)] $@"
	@$(CC) $(CFLAGS) $< -c -o $@ -I $(PUTTY)

# Performance outputs
perf-original-aes.txt: sshaes-perf
	@echo "[run] $@"
	@./$^
	@mv perf-output.txt $@

perf-output-aes.txt: sshaesni-perf
	@echo "[run] $@"
	@./$^
	@mv perf-output.txt $@

%.sorted.txt: %.txt
	@echo "[sort] $@"
	@sort $^ > $@
    
perf-geomean-%.txt: perf-original-%.sorted.txt perf-output-%.sorted.txt
	@echo "[geomean] $@"
	@python ./geomean.py $^ > $@

clean:
	rm *.o *.txt sshaes-demo-* sshaes-perf sshaesni-perf aescpuid
