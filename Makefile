# Makefile for unit tests
#
# @author kryukov@frtk.ru
# @version 4.3
#
# For Putty AES NI project
# http://putty-aes-ni.googlecode.com/

BUILD_DIR?=build

# Performance outputs
perf-original-aes.txt: ${BUILD_DIR}/sshaes-perf
	@echo "[run] $@"
	@./$^
	@mv perf-output.txt $@

perf-output-aes.txt: ${BUILD_DIR}/sshaes-perf-ni
	@echo "[run] $@"
	@./$^
	@mv perf-output.txt $@

%.sorted.txt: %.txt
	@echo "[sort] $@"
	@sort $^ > $@
    
perf-geomean-%.txt: geomean.py perf-original-%.sorted.txt perf-output-%.sorted.txt
	@echo "[geomean] $@"
	@python3 ./$< $(filter-out $<,$^) > $@

clean:
	rm perf-*.txt
