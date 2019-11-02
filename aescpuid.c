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

#if (defined __arm__ || defined __aarch64__) /* ARM */

#ifdef __linux__

#include <sys/auxv.h>
#include <asm/hwcap.h>

static int CheckCPUsupportAES()
{
#if defined HWCAP_AES
    return getauxval(AT_HWCAP) & HWCAP_AES;
#elif defined HWCAP2_AES
    return getauxval(AT_HWCAP2) & HWCAP2_AES;
#else
    return false;
#endif
}

#else /* Windows */

static int CheckCPUsupportAES()
{
    return IsProcessorFeaturePresent(PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE);
}

#endif /* Windows or Linux */

#else /* x86 */

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

#endif /* ARM / x86 */

int main(int argc, char ** argv)
{
    const int res = !CheckCPUsupportAES();
    printf("This CPU %s AES-NI\n", res ? "does not support" : "supports");
    return res;
}
