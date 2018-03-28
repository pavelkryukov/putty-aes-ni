[![Build Status](https://travis-ci.org/pavelkryukov/putty-aes-ni.svg?branch=master)](https://travis-ci.org/pavelkryukov/putty-aes-ni)[![Build status](https://ci.appveyor.com/api/projects/status/shr6l4t6dvqq5ytk?svg=true)](https://ci.appveyor.com/project/pavelkryukov/putty-aes-ni)

This repository contains testing and demonstration suites for encryption using [AES instruction set](https://www.intel.com/content/dam/doc/white-paper/advanced-encryption-standard-new-instructions-set-paper.pdf) and [SHA instruction set](https://software.intel.com/en-us/articles/intel-sha-extensions) in [PuTTY](http://www.putty.org/) SSH client and derived tools.

### Motivation

* 4.7x AES encryption and 13x AES decryption boost on Intel® Core™ i5-2520M.
  * CPU usage decreases from 50% to 30% for transmission of large files via Secure copy on Intel® Core™ i5-2520M.
* Attacks on AES via cache miss analysis [[Bangerter et al.](http://eprint.iacr.org/2010/594)] become impossible.

You may use [Intel® Product Specification Advanced Search](https://ark.intel.com/Search/FeatureFilter?productType=processors&AESTech=true) to check if your CPU supports AES-NI and SHA-NI.

### Impact

* PuTTY with new AES instructions was developed as a part of [MIPT Cryptography course](https://github.com/vlsergey/infosec) activity.
* Performance results were presented by [Maxim Kuznetsov](https://github.com/mkuznets) in „Radio Engineering and Cryptography“ section of [55th MIPT Conference](http://conf55.mipt.ru/info/main/). The report got „The Best Report of the Section“ achievement.
* In October 2017, AES instructions were merged to [the main PuTTY repository](https://git.tartarus.org/?p=simon/putty.git) ([2d31305](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=2d31305af9d3bf4096bb0c30e8a8336caaa70673)).
* In March 2018, SHA instructions were merged to [the main PuTTY repository](https://git.tartarus.org/?p=simon/putty.git) ([670c04f](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=670c04ff5b9877f00f850541648f9961067e135f)).
* [PR clang/34980](https://bugs.llvm.org/show_bug.cgi?id=34980) was reported to LLVM community as it affects AES NI code generation.

### Contributors

 * [Pavel Kryukov](https://github.com/pavelkryukov): AES encryption, unit tests, refactoring, CI, SHA integration
 * [Maxim Kuznetsov](https://github.com/mkuznets): original idea, AES key expansion, integration with SDE, paper
 * [Svyatoslav Kuzmich](https://github.com/skuzmich): AES decryption, infrastructure

We are pleased to thank:
 * [Sergey Vladimirov](https://github.com/vlsergey) for mentorship of our work in MIPT.
 * PuTTY creator [Simon Tatham](https://www.chiark.greenend.org.uk/~sgtatham/) for PuTTY, reviewing our code, and accepting the changes.
 * [Jeffrey Walton](https://github.com/noloader) for clean [SHA implementation](https://github.com/noloader/SHA-Intrinsics) and fixes to CPUID code.
 
Detailed contributions to PuTTY AES-NI and SHA-NI code:
 * 2012 – 2015 AES patches are available in [repository history](https://github.com/pavelkryukov/putty-aes-ni/commits/svn-head)
 * 2015 – 2017 AES patches are located in [git branch](https://github.com/pavelkryukov/putty/commits/aespatches)
 * 2018 SHA patches are pointed by [git branch](https://github.com/pavelkryukov/putty/commits/shapatches)
 * Patches applied to PuTTY main repository directly:
   * [`a27f55e`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=a27f55e819f2c39ed45425625a0fa63e06089d76) _Use correct way to detect new instructions in Clang_
   * [`1ec8a84`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=1ec8a84cf69a53e3c02d54280ff48d22ae571abb) _Add CPUID leaf checks prior to SHA checks_
   * [`d6338c2`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=d6338c22c32b9f55b71ace80f993bbb8f8c1aa6d) _Fix mishandling of IV in AES-NI CBC decryption_

### Demonstration

To verify boost of AES-NI algorithm, one may create standalone encryptor and decryptor. After cloning repository and PuTTY, they can be build `aestest` dir using Makefile targets:

| Type | SW AES | AES NI |
|:-|:-------|:-------|
| Encrypt | bin/sshaes-demo | bin/sshaesni-demo |
| Decrypt | bin/sshaes-demo-decode | bin/sshaesni-demo-decode |

Syntax is the same as 'cp' command. Please note that file size must be a multiple of 16 bytes.
`<sshdemo> src dst`

### Performance tests

To run performance test, use `./run.sh aes -p`. Be sure that CPU supports AES-NI (_Checking for AES-NI support... found_). The output is geometric mean of speedups per encryption/decryption of different data sets with memory range from 16 bytes to 16 Mbytes.
Software optimization of both AES algorithms is enabled, to turn it off, change $(OPT) to $(NOOPT) in Makefile.

Performance data is stored to **perf-original.txt** and **perf-output.txt**, the first one is for standard version, second is for AES-NI version. Format of output files is following: `code keylen blocklen time`, where code 0 is encryption, code 1 is decryption, and code 2 is sdctr

### Functional tests

To run performance test, use `./run.sh aes`. If CPU does not support AES NI, SDE will be started automatically. The output is produced into two text files: **txt/test-original-aes-100.txt** and **txt/test-output-aes-100.txt**, generated by software and NI versions respectively. Output is produced by encryption and decryption of pseudo-random data generated with 100 random seeds, using different block length, key length, and amount of blocks. The exact procedure of generating outputs is located in **aestest.c** file, so you can shrink it to localize failing test case.

SHA flow is almost the same, and can be called with `./run.sh sha` command. Likely, the procedure is located in **shatest.c** file.

### Next steps

The following ideas may be used by Information Security course students for their projects:
* Add ARM SHA and AES flows to PuTTY
* Use RdRand for random numbers generation

### Further Reading

1. [Example of usage AES-NI+OpenSSL+OpenVPN with simple performance analysis (in Russian)](http://sysadminblog.ru/freebsd/2011/01/15/freebsd-aesni-openssl-openvpn.html)
1. [Intel® 64 and IA-32 Architectures Software Developer Manuals](http://www.intel.com/content/www/us/en/processors/architectures-software-developer-manuals.html)
1. [Intel® Software Development Emulator](http://software.intel.com/en-us/articles/intel-software-development-emulator)
