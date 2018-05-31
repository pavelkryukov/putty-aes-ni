[![Build Status](https://travis-ci.org/pavelkryukov/putty-aes-ni.svg?branch=master)](https://travis-ci.org/pavelkryukov/putty-aes-ni)
[![Build Status](https://ci.appveyor.com/api/projects/status/shr6l4t6dvqq5ytk?svg=true)](https://ci.appveyor.com/project/pavelkryukov/putty-aes-ni)

This repository contains testing and demonstration suites for encryption using [AES instruction set](https://www.intel.com/content/dam/doc/white-paper/advanced-encryption-standard-new-instructions-set-paper.pdf) and [SHA instruction set](https://software.intel.com/en-us/articles/intel-sha-extensions) in [PuTTY](http://www.putty.org/) SSH client and derived tools.

## Motivation

* 4.7x AES encryption and 13x AES decryption boost on Intel® Core™ i5-2520M.
  * CPU usage decreases from 50% to 30% for transmission of large files via Secure copy on Intel® Core™ i5-2520M.
* Attacks on AES via cache miss analysis [[Bangerter et al.](http://eprint.iacr.org/2010/594)] become impossible.

## Impact

* PuTTY 0.71 will contain AES and SHA new instructions.
* Performance results were presented by [Maxim Kuznetsov](https://github.com/mkuznets) in „Radio Engineering and Cryptography“ section of [55th MIPT Conference](http://conf55.mipt.ru/info/main/). The report got „The Best Report of the Section“ achievement.
* PuTTY with new AES instructions was developed as a part of [MIPT Cryptography course](https://github.com/vlsergey/infosec) activity.
* ~~[PR clang/34980](https://bugs.llvm.org/show_bug.cgi?id=34980)~~ was reported, fix is expected in next LLVM release.

## Contributors

### _AES NI_
 * [Maxim Kuznetsov](https://github.com/mkuznets): original idea, AES key expansion, integration with SDE, paper
 * [Pavel Kryukov](https://github.com/pavelkryukov): AES encryption, unit tests, refactoring, CI
 * [Svyatoslav Kuzmich](https://github.com/skuzmich): AES decryption, infrastructure
 
Details:
 * Original 2012 – 2015 patches are available in [repository history](https://github.com/pavelkryukov/putty-aes-ni/commits/svn-head)
 * Original 2015 – 2017 patches are located in [git branch](https://github.com/pavelkryukov/putty/commits/aespatches)
 * Actual (applied to the main repository) patches are pointed by [git branch](https://github.com/pavelkryukov/putty/commits/aespatches-fixed)
 * Fixes and enhancements by community:
   * [`599bab8`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=599bab84a1019ccd6228dcc5a8bf8b9a33a96452) _Condition out AES-NI support if using a too-old clang_
   * [`a27f55e`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=a27f55e819f2c39ed45425625a0fa63e06089d76) _Use correct way to detect new instructions in Clang_
   * [`d6338c2`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=d6338c22c32b9f55b71ace80f993bbb8f8c1aa6d) _Fix mishandling of IV in AES-NI CBC decryption_
   * [`7babe66`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=7babe66a839fecfe5d8b3db901b06d2fb7672cfc) _Make lots of generic data parameters into 'void *'._
   * [`f4ca28a`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=f4ca28a0f49ff23c8a9835fe62e209aa2c7b5e61) _Add a missing const_

### _SHA NI_
 * [Jeffrey Walton](https://github.com/noloader) clean free [SHA implementation](https://github.com/noloader/SHA-Intrinsics), fixes to CPUID code.
 * [Pavel Kryukov](https://github.com/pavelkryukov): integration of SHA into PuTTY code, unit tests, CI
 
Details:
 * Original patches are pointed by [git branch](https://github.com/pavelkryukov/putty/commits/shapatches)
 * Fixes and enhancements by community:
   * [`a27f55e`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=a27f55e819f2c39ed45425625a0fa63e06089d76) _Use correct way to detect new instructions in Clang_
   * [`1ec8a84`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=1ec8a84cf69a53e3c02d54280ff48d22ae571abb) _Add CPUID leaf checks prior to SHA checks_
   * [`fbc8b7a`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=fbc8b7a8cbf49845d8fe35ffa6e66bb2638437aa) _Include <intrin.h> for hardware SHA on Windows_
 
### _Thanks_
 * PuTTY creator [Simon Tatham](https://www.chiark.greenend.org.uk/~sgtatham/) for PuTTY, reviewing our code, and accepting the changes.
 * [Sergey Vladimirov](https://github.com/vlsergey) for mentorship of AES NI development in MIPT.
 
## Demonstration

### _AES encryption and decryption_

To verify boost of AES-NI algorithm, one may create standalone AES encryptor and decryptor tools. After cloning repository and PuTTY, binaries are buildable with GNU Makefile:

| Type | SW AES | AES NI |
|:-|:-------|:-------|
| Encrypt | bin/sshaes-demo | bin/sshaesni-demo |
| Decrypt | bin/sshaes-demo-decode | bin/sshaesni-demo-decode |

Syntax is the same as 'cp' command. Please note that file size must be a multiple of 16 bytes.
`<sshdemo> src dst`

You may use [Intel® Product Specification Advanced Search](https://ark.intel.com/Search/FeatureFilter?productType=processors&AESTech=true) to check if your CPU supports AES-NI and SHA-NI.

### _Performance tests_

To run performance test, use `./run.sh aes -p`. Be sure that CPU supports AES-NI (_Checking for AES-NI support... found_). The output is geometric mean of speedups per encryption/decryption of different data sets with memory range from 16 bytes to 16 Mbytes.
Software optimization of both AES algorithms is enabled, to turn it off, change(OPT) to(NOOPT) in Makefile.

Performance data is stored to **perf-original.txt** and **perf-output.txt**, the first one is for standard version, second is for AES-NI version. Format of output files is following: `code keylen blocklen time`, where code 0 is encryption, code 1 is decryption, and code 2 is for encryption in sdctr mode.

### _Functional tests_

To run performance test, use `./run.sh aes`. If CPU does not support AES NI, [Intel® Software Development Emulator](http://software.intel.com/en-us/articles/intel-software-development-emulator) starts automatically if installed. The output is produced into two text files: **txt/test-original-aes-100.txt** and **txt/test-output-aes-100.txt**, generated by software and NI versions respectively. Output is produced by encryption and decryption of pseudo-random data generated with 100 random seeds, using different block length, key length, and amount of blocks. The exact procedure of generating outputs is located in **aestest.c** file, so you can shrink it to localize failing test case.

SHA flow is almost the same, and can be called with `./run.sh sha` command. Likely, the procedure is located in **shatest.c** file.

## Next steps

Following ideas for PuTTY enhancements may be used by Information Security course students for their projects:
* Use RdRand for random numbers generation
* Add ARM AES and SHA flows _(low priority as Windows runs on ARMv7, while the extensions are for ARMv8 only)_
