/**
 * aesperf.c
 *
 * Measuring time of AES cryptoalgorithm
 *
 * @author kryukov@frtk.ru
 * @version 2.1
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <benchmark/benchmark.h>
#include <memory>

#include "intf.h"

enum TestType
{
    ENCRYPT,
    DECRYPT,
    SDCTR
};

static const size_t MAX_BLK = 1 << 24;
static const int accelerated = 1;
static const int software = 0;

template<int keytype, TestType test, int is_accelerated>
static void bench(benchmark::State& state)
{
    auto ptr = std::make_unique<unsigned char[]>(state.range(0) + 2*256);
    unsigned char* const key = ptr.get() + state.range(0);
    unsigned char* const blk = ptr.get();
    unsigned char* const iv = ptr.get() + keytype + state.range(0);

    auto handle = create_handle(keytype, is_accelerated, key, iv);

    for (auto _ : state) {
       if (test == DECRYPT)
           decrypt(handle, blk, state.range(0));
       else
           encrypt(handle, blk, state.range(0));
    }

    free_handle(handle);
    state.SetComplexityN(state.range(0));
}

BENCHMARK_TEMPLATE(bench, 16, ENCRYPT, software)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 24, ENCRYPT, software)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 32, ENCRYPT, software)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 16, DECRYPT, software)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 24, DECRYPT, software)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 32, DECRYPT, software)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 16, SDCTR, software)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 24, SDCTR, software)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 32, SDCTR, software)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);

BENCHMARK_TEMPLATE(bench, 16, ENCRYPT, accelerated)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 24, ENCRYPT, accelerated)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 32, ENCRYPT, accelerated)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 16, DECRYPT, accelerated)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 24, DECRYPT, accelerated)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 32, DECRYPT, accelerated)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 16, SDCTR, accelerated)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 24, SDCTR, accelerated)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);
BENCHMARK_TEMPLATE(bench, 32, SDCTR, accelerated)->RangeMultiplier(2)->Range(16, MAX_BLK)->Complexity(benchmark::oN);

BENCHMARK_MAIN();

