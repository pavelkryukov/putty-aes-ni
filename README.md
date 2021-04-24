**PuTTY**:
[![Build Status](https://github.com/pavelkryukov/putty-ci/actions/workflows/putty-ci.yml/badge.svg)](https://github.com/pavelkryukov/putty-ci/actions/workflows/putty-ci.yml)
[![Build status](https://ci.appveyor.com/api/projects/status/ff23l7wwucr4lle7?svg=true)](https://ci.appveyor.com/project/pavelkryukov/putty-ci)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/pavelkryukov/putty-ci.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/pavelkryukov/putty-ci/alerts/)

Demonstration suites for encryption using [Intel AES instruction set](https://www.intel.com/content/dam/doc/white-paper/advanced-encryption-standard-new-instructions-set-paper.pdf) and [ARM Cryptography Extension](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0514g/way1395175472464.html) in [PuTTY](http://www.putty.org/) SSH client and derived tools.

## _Motivation_

* 40x encryption/decryption boost over straightforward C implementation.
* 4.7x AES encryption and 13x AES decryption boost on Intel Core i5-2520M over lookup table implementation
  * CPU usage decreases from 50% to 30% for transmission of large files via Secure copy on Intel Core i5-2520M.
  * Lookup-table specific attacks on AES via cache miss analysis [[Bangerter et al.](http://eprint.iacr.org/2010/594)] become impossible.
* 9x AES boost on Ampere eMAG CPU

## _Impact_

* Hardware-accelerated versions of AES, SHA-256, SHA-1 are intergrated to [PuTTY 0.71](https://www.chiark.greenend.org.uk/~sgtatham/putty/changes.html)
* Performance results were presented by [Maxim Kuznetsov](https://github.com/mkuznets) in „Radio Engineering and Cryptography“ section of [55th MIPT Conference](http://conf55.mipt.ru/info/main/). The report got „The Best Report of the Section“ achievement.
* PuTTY with new AES instructions was developed as a part of [MIPT Cryptography course](https://github.com/vlsergey/infosec) activities.
* Established basic continious integration infrastructure (CI) for [cross-platform PuTTY builds](https://github.com/pavelkryukov/putty).
* LLVM bugs reported:
  * ~~[34980](https://bugs.llvm.org/show_bug.cgi?id=34980)~~: Bug with target attribute propagation. Fixed in Clang 6.0.1.
  * ~~[38386](https://bugs.llvm.org/show_bug.cgi?id=38386)~~: PuTTY CI needs LLVM resource compiler on Windows. Shipped in LLVM 7.0.0 package.
  * ~~[40300](https://bugs.llvm.org/show_bug.cgi?id=40300)~~: lld-link does not detect duplicate symbols with /O3. Fixed in LLVM 9.0.0.

## _Credits_
### _AES NI_
 * [Maxim Kuznetsov](https://github.com/mkuznets): original idea, AES key expansion, integration with SDE, paper
 * [Pavel Kryukov](https://github.com/pavelkryukov): AES encryption, unit tests, refactoring
 * [Svyatoslav Kuzmich](https://github.com/skuzmich): AES decryption, infrastructure

**Details:**
 * Original 2012–2015 patches are available in [repository history](https://github.com/pavelkryukov/putty-aes-ni/commits/svn-head)
 * Original 2015–2017 patches are located in [git branch](https://github.com/pavelkryukov/putty-ci/commits/aespatches)
 * Actual (applied to the main repository) patches are pointed by [git branch](https://github.com/pavelkryukov/putty-ci/commits/aespatches-fixed)
 * Patched sources for PuTTY 0.64 in [git branch](https://git.tartarus.org/?p=simon/putty.git;a=tag;h=4ad063b7cd9258a104dc3caed36c7ae1abd1f4be)
 * Fixes and enhancements by community before the code was completely rewritten:
   * [`599bab8`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=599bab84a1019ccd6228dcc5a8bf8b9a33a96452) _Condition out AES-NI support if using a too-old clang_
   * [`a27f55e`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=a27f55e819f2c39ed45425625a0fa63e06089d76) _Use correct way to detect new instructions in Clang_
   * [`d6338c2`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=d6338c22c32b9f55b71ace80f993bbb8f8c1aa6d) _Fix mishandling of IV in AES-NI CBC decryption_
   * [`7babe66`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=7babe66a839fecfe5d8b3db901b06d2fb7672cfc) _Make lots of generic data parameters into 'void *'._
   * [`f4ca28a`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=f4ca28a0f49ff23c8a9835fe62e209aa2c7b5e61) _Add a missing const_
   * [`91a624f`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=91a624fb70230a885656e74c89865270b27c9de9) _sshaes.c: add some missing clang target attributes_
   * [`3347bd8`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=3347bd81b75e4e2e5c39de70d8771db37b380daa) _Fix AES-NI SDCTR to pass the new tests._
   * [`dfdb73e`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=dfdb73e103366081045f7e5c23ed1a35e4777da4) _Complete rewrite of the AES code._
   * [`ced0f19`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=ced0f1911830eceae26c737e93b44136828f2f13) _Ensure our aes_ni_context is 16-byte aligned._
   * [`fca13a1`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=fca13a17b160da3b5069df3ceab19d4448c4f389) _Break up crypto modules containing HW acceleration._

### _SHA NI_
 * [Jeffrey Walton](https://github.com/noloader): publc domain [SHA implementation](https://github.com/noloader/SHA-Intrinsics), fixes to CPUID code.
 * [Pavel Kryukov](https://github.com/pavelkryukov): integration of SHA into PuTTY code, unit tests
 
**Details:**
 * Original patches are pointed by [git branch](https://github.com/pavelkryukov/putty-ci/commits/shapatches)
 * Fixes and enhancements by community before the code was completely rewritten:
   * [`a27f55e`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=a27f55e819f2c39ed45425625a0fa63e06089d76) _Use correct way to detect new instructions in Clang_
   * [`1ec8a84`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=1ec8a84cf69a53e3c02d54280ff48d22ae571abb) _Add CPUID leaf checks prior to SHA checks_
   * [`fbc8b7a`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=fbc8b7a8cbf49845d8fe35ffa6e66bb2638437aa) _Include `intrin.h` for hardware SHA on Windows_
   * [`cbbd464`](https://git.tartarus.org/?p=simon/putty.git;a=commit;h=cbbd464fd752821fe444d67b891c3977c426dee1) _Rewrite the SHA-256 and SHA-1 hash function modules._

### _PuTTY Continious integration_
  * [Pavel Kryukov](https://github.com/pavelkryukov)

**Details:**
  * CI repository is a [PuTTY fork](https://github.com/pavelkryukov/putty-ci)
  * Previous history is available in PuTTY-AES-NI repository: [Travis](https://github.com/pavelkryukov/putty-aes-ni/commits/master/.travis.yml), [Appveyor](https://github.com/pavelkryukov/putty-aes-ni/commits/master/appveyor.yml)

**Code Coverage Chart!**

[![Code Coverage](https://codecov.io/gh/pavelkryukov/putty-ci/branch/main/graphs/tree.svg)](https://codecov.io/gh/pavelkryukov/putty-ci)
 
## _CPUs with AES NI support_

Get sure that your CPU supports AES-NI by running `make aescpuid && ./aescpuid`.
You may use [Intel Product Specification Advanced Search](https://ark.intel.com/Search/FeatureFilter?productType=processors&AESTech=true) to check if your CPU supports AES-NI and SHA-NI.
 
## _AES encryption and decryption_

To observe boost of AES-NI algorithm, one may create standalone AES-256 encryptor and decryptor tools using PuTTY source code. After cloning repository and PuTTY submodule, binaries are buildable with CMake:

| Type | SW AES | AES NI |
|:-|:-------|:-------|
| Encrypt | aes-demo-sw-encode | aes-demo-hw-encode |
| Decrypt | aes-demo-sw-decode | aes-demo-hw-decode |

Syntax is the same as 'cp' command. Please note that file size must be a multiple of 16 bytes.
`<sshdemo> src dst`

## _AES performance tests_

To run performance test, use `make perf-geomean-aes.txt`. The output is geometric mean of speedups per encryption/decryption of different data sets with memory range from 16 bytes to 16 Mbytes.

Raw performance data is stored to **perf-original.txt** and **perf-output.txt**, the first one is for standard version, second is for AES-NI version. Format of output files is following: `code keylen blocklen time`, where code 0 is encryption, code 1 is decryption, and code 2 is for encryption in sdctr mode.

## _AES and SHA functional tests_

Since 2019, all cryptography functional tests are included to the PuTTY repository. To run the tests in Bash, simply execute following commands in a configured PuTTY workspace:

```bash
make testcrypt
export PUTTY_TESTCRYPT=/path/to/the/testcrypt
./test/cryptsuite.py
```

The same works for Windows environment if you use appropriate CMD or PowerShell commands.

## _Thanks_
 * PuTTY creator [Simon Tatham](https://www.chiark.greenend.org.uk/~sgtatham/) for PuTTY, reviewing our code, and accepting the changes.
 * [Sergey Vladimirov](https://github.com/vlsergey) for mentorship of AES NI development in MIPT.
