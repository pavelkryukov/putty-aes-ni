[![Build Status](https://travis-ci.org/pavelkryukov/putty-aes-ni.svg?branch=master)](https://travis-ci.org/pavelkryukov/putty-aes-ni)[![Build status](https://ci.appveyor.com/api/projects/status/shr6l4t6dvqq5ytk?svg=true)](https://ci.appveyor.com/project/pavelkryukov/putty-aes-ni)

This repository contains testing and demonstration suites for AES encryption using [AES instruction set](http://software.intel.com/en-us/articles/intel-advanced-encryption-standard-aes-instructions-set) in [PuTTY](http://www.putty.org/) SSH client.

---
### Motivation

* 4.7x encryption and 13x decryption boost on Intel® Core™ i5-2520M.
* CPU usage decreases from 50% to 30% for transmission of large files via Secure copy on Intel® Core™ i5-2520M.
* Attacks on AES via cache miss analysis [[Bangerter et al.](http://eprint.iacr.org/2010/594)] become impossible.

You may use [Intel® Product Specification Advanced Search](https://ark.intel.com/Search/FeatureFilter?productType=processors&AESTech=true) to check if your CPU supports AES-NI.

----
### Impact

PuTTY with new AES instructions was developed as a part of [MIPT Cryptography course](https://github.com/vlsergey/infosec) activity.

Performance results were presented by [Maxim Kuznetsov](https://github.com/mkuznets) in „Radio Engineering and Cryptography“ section of [55th MIPT Conference](http://conf55.mipt.ru/info/main/). The report got „The Best Report of the Section“ achievement.

In October 2017, our changes were merged to [the main PuTTY repository](https://git.tartarus.org/?p=simon/putty.git) ([2d31305](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=2d31305af9d3bf4096bb0c30e8a8336caaa70673)).

A bug in AES NI code generation was reported to LLVM community ([#34980](https://bugs.llvm.org/show_bug.cgi?id=34980)).

---
### Contributors

 * [Pavel Kryukov](https://github.com/pavelkryukov)
 * [Maxim Kuznetsov](https://github.com/mkuznets)
 * [Svyatoslav Kuzmich](https://github.com/skuzmich)

We are pleased to thank:
 * [Sergey Vladimirov](https://github.com/vlsergey) for mentorship of our work in MIPT.
 * PuTTY creator [Simon Tatham](https://www.chiark.greenend.org.uk/~sgtatham/) for PuTTY, reviewing our code, and accepting the changes.

----
### Demonstration

To verify boost of AES-NI algorithm, one may create standalone encryptor and decryptor. After cloning repository and PuTTY, they can be build `aestest` dir using Makefile targets:

| Type | SW AES | AES NI |
|:-|:-------|:-------|
| Encrypt | bin/sshaes-demo | bin/sshaesni-demo |
| Decrypt | bin/sshaes-demo-decode | bin/sshaesni-demo-decode |

Syntax is the same as 'cp' command. Please note that file size must be a multiple of 16 bytes.
`<sshdemo> src dst`

----
### Performance tests

To run performance test, use `./run.sh -p` in `aestest` dir. Be sure that CPU supports AES-NI (Checking for AES-NI support... found). The output is geometric mean of speedups per encryption/decryption of different data sets with memory range from 16 bytes to 256 Mbytes.
Optimization of both AES algorithms is enabled, to turn it off, change $(OPT) to $(NOOPT) in Makefile.

Performance data is stored to `perf-original.txt` and `perf-output.txt`, the first one is for standard version, second is for AES-NI version. Format of output files is following: `code keylen blocklen time`, where code 0 is encryption, code 1 is decryption, and code 2 is sdctr

---
### Further Reading

1. [Article with some additional info and links on AES-NI](http://software.intel.com/en-us/articles/intel-advanced-encryption-standard-instructions-aes-ni)
1. [Example of usage AES-NI+OpenSSL+OpenVPN with simple performance analysis (in Russian)](http://sysadminblog.ru/freebsd/2011/01/15/freebsd-aesni-openssl-openvpn.html)
1. [Intel® 64 and IA-32 Architectures Software Developer Manuals](http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html)
1. [Intel® Software Development Emulator](http://software.intel.com/en-us/articles/intel-software-development-emulator)
