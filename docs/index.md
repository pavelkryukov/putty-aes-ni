[PuTTY](http://www.putty.org/) is one of the few programs without support of [AES instruction set](http://en.wikipedia.org/wiki/AES_instruction_set) introduced in 2008. We decided to make support of these instructions and see impact on PuTTY and pscp performance after these changes. The developers team consisted of:
 * [Pavel Kryukov](https://github.com/pavelkryukov)
 * [Maxim Kuznetsov](https://github.com/mkuznets)
 * [Svyatoslav Kuzmich](https://github.com/skuzmich)

If you want to know if your processor supports these instruction set, [look at this table](http://ark.intel.com/search/advanced?AESTech=true). You can also use [Intel® Software Development Emulator](http://software.intel.com/en-us/articles/intel-software-development-emulator), it may execute AES instruction set.

Project is made as [MIPT Cryptography course project](http://radio.fizteh.ru/infsec/), team showed results on [55th MIPT Conference](http://conf55.mipt.ru/info/main/), "Radio Engineering and Cryptography" section and got a "Best Presentation of Section" achievement.

[![Build Status](https://travis-ci.org/pavelkryukov/putty-aes-ni.svg?branch=master)](https://travis-ci.org/pavelkryukov/putty-aes-ni)

----
### How to run demonstration

After cloning repository and PuTTY, aou are able to build 4 binaries in `aestest` dir:

| Type | SW AES | AES NI |
|:-|:-------|:-------|
| Encrypt | bin/sshaes-demo | bin/sshaesni-demo |
| Decrypt | bin/sshaes-demo-decode | bin/sshaesni-demo-decode |

Syntax is the same as 'cp' command:
`<sshdemo> src dst`

**Known issues:**
  * If file size is not multiple of 16 bytes, last bytes may be encrypt/decrypt incorrect.

----
### Performance tests

To run performance test, use `./run.sh -p` in `aestest` dir. Be sure that CPU supports AES-NI (`Checking for AES-NI support... found`). Performance data is stored to `perf-original.txt` and `perf-output.txt`, the first one is for standard version, second is for AES-NI version. Format of output files is following:

`code keylen blocklen time`

where code 0 is encryption, code 1 is decryption, and code 2 is sdctr

Every experiment is performed 30 times with different data sets. Range of memory is from 16 bytes to 256 Mbytes, SDCTR tests are disabled.
Optimization of both AES algorithms is enabled, to turn it off, change $(OPT) to $(NOOPT) in Makefile.
