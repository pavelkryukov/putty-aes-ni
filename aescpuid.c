/**
 * cpuid.c
 *
 * Checks if CPU has support of AES instructions
 *
 * @author kryukov@frtk.ru
 * @version 4.0
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <stdio.h>

#if defined(__x86_64__) || defined(__i386)

#if defined(__clang__) || defined(__GNUC__)

#include <cpuid.h>
static int CheckCPUsupportAES()
{
    unsigned int CPUInfo[4];
    __cpuid(1, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    return (CPUInfo[2] & (1 << 25)) && (CPUInfo[2] & (1 << 19)); /* Check AES and SSE4.1 */
}

#else /* defined(__clang__) || defined(__GNUC__) */

static int CheckCPUsupportAES()
{
    unsigned int CPUInfo[4];
    __cpuid(CPUInfo, 1);
    return (CPUInfo[2] & (1 << 25)) && (CPUInfo[2] & (1 << 19)); /* Check AES and SSE4.1 */
}

#endif /* defined(__clang__) || defined(__GNUC__) */

#else /* defined(__x86_64__) || defined(__i386) */
    
#include <sys/auxv.h>
#include <asm/hwcap.h>

static int CheckCPUsupportAES(void)
{
#if defined HWCAP_AES
    return getauxval(AT_HWCAP) & HWCAP_AES;
#elif defined HWCAP2_AES
    return getauxval(AT_HWCAP2) & HWCAP2_AES;
#else
    return 0;
#endif
}

#endif /* defined(__x86_64__) || defined(__i386) */

int main(int argc, char ** argv)
{
    const int does_not_support = CheckCPUsupportAES() == 0;
    printf("This CPU %s AES-NI\n", does_not_support ? "does not support" : "supports");
    return does_not_support;
}
