[PuTTY](http://www.putty.org/) is one of the few secure applications without support of [AES instruction set](http://en.wikipedia.org/wiki/AES_instruction_set) introduced in 2008. PuTTY-AES-NI adds support of these instructions along with demonstration suite to see impact on PuTTY and pscp performance.

---
### Advantages

* 4.7x encryption and 13x decryption boost on Intel® Core™ i5-2520M.
* CPU usage decrease from 50% to 30% for transmission of large files via Secure copy on Intel® Core™ i5-2520M.
* Attacks on AES via cache miss analysis [[Bangerter et. al](http://eprint.iacr.org/2010/594)] become impossible.

----
### Authors

The developers team consisted of:
 * [Pavel Kryukov](https://github.com/pavelkryukov)
 * [Maxim Kuznetsov](https://github.com/mkuznets)
 * [Svyatoslav Kuzmich](https://github.com/skuzmich)

Project is made as [MIPT Cryptography course project](http://radio.fizteh.ru/infsec/). The results were presented in [55th MIPT Conference](http://conf55.mipt.ru/info/main/), "Radio Engineering and Cryptography" section and got a "Best Presentation of Section" achievement.

----
### Source code

[![Build Status](https://travis-ci.org/pavelkryukov/putty-aes-ni.svg?branch=master)](https://travis-ci.org/pavelkryukov/putty-aes-ni)[![Build status](https://ci.appveyor.com/api/projects/status/shr6l4t6dvqq5ytk?svg=true)](https://ci.appveyor.com/project/pavelkryukov/putty-aes-ni)

PuTTY-AES-NI code is available in [the additional repository](https://github.com/pavelkryukov/putty-aes-ni2). There are no changes to build flow comparing to the original PuTTY.

[The main repository](https://github.com/pavelkryukov/putty-aes-ni) contains demonstration suite, functional and performance tests, and detailed list of contributions.

----
### Demonstration

To verify boost of AES-NI algorithm, one may create standalone encryptor and decryptor. After cloning repository and PuTTY, they can be build `aestest` dir using Makefile targets:

| Type | SW AES | AES NI |
|:-|:-------|:-------|
| Encrypt | bin/sshaes-demo | bin/sshaesni-demo |
| Decrypt | bin/sshaes-demo-decode | bin/sshaesni-demo-decode |

Syntax is the same as 'cp' command:
`<sshdemo> src dst`

**Known issues:**
  * If file size is not multiple of 16 bytes, last bytes may be encrypted/decrypted incorrect.

----
### Performance tests

To run performance test, use `./run.sh -p` in `aestest` dir. Be sure that CPU supports AES-NI (Checking for AES-NI support... found). The output is geometric mean of speedups per encryption/decryption of different data sets with memory range from 16 bytes to 256 Mbytes.
Optimization of both AES algorithms is enabled, to turn it off, change $(OPT) to $(NOOPT) in Makefile.

Performance data is stored to `perf-original.txt` and `perf-output.txt`, the first one is for standard version, second is for AES-NI version. Format of output files is following: `code keylen blocklen time`, where code 0 is encryption, code 1 is decryption, and code 2 is sdctr

---
### Additional links

1. PuTTY project website - http://www.chiark.greenend.org.uk/~sgtatham/putty/
1. Intel® Advanced Encryption Standard (AES) Instructions Set -
http://software.intel.com/en-us/articles/intel-advanced-encryption-standard-aes-instructions-set
1. Article with some additional info and links on AES-NI -
http://software.intel.com/en-us/articles/intel-advanced-encryption-standard-instructions-aes-ni
1. Example of usage AES-NI+OpenSSL+OpenVPN with simple performance analysis - http://sysadminblog.ru/freebsd/2011/01/15/freebsd-aesni-openssl-openvpn.html (rus)
1. Intel® 64 and IA-32 Architectures Software Developer Manuals  - http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html
1. List of Intel processors with AES-NI support: http://ark.intel.com/search/advanced?AESTech=true
1. Intel® Software Development Emulator http://software.intel.com/en-us/articles/intel-software-development-emulator
