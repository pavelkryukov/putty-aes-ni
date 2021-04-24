/**
 * cpuid.c
 *
 * Checks if CPU has support of AES instructions
 *
 * @author kryukov@frtk.ru
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <stdbool.h>
#include <stdio.h>

#if defined(__x86_64__) || defined(__i386)

#if defined(__clang__) || defined(__GNUC__)

#include <cpuid.h>
bool platform_aes_hw_available()
{
    unsigned int CPUInfo[4];
    __cpuid(1, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    return (CPUInfo[2] & (1 << 25)) != 0 && (CPUInfo[2] & (1 << 19)) != 0; /* Check AES and SSE4.1 */
}

#else /* defined(__clang__) || defined(__GNUC__) */

bool platform_aes_hw_available()
{
    unsigned int CPUInfo[4];
    __cpuid(CPUInfo, 1);
    return (CPUInfo[2] & (1 << 25)) != 0 && (CPUInfo[2] & (1 << 19)) != 0; /* Check AES and SSE4.1 */
}

#endif /* defined(__clang__) || defined(__GNUC__) */

#else /* defined(__x86_64__) || defined(__i386) */
    
#include <sys/auxv.h>
#include <asm/hwcap.h>

bool platform_aes_hw_available(void)
{
#if defined HWCAP_AES
    return (getauxval(AT_HWCAP) & HWCAP_AES) != 0;
#elif defined HWCAP2_AES
    return (getauxval(AT_HWCAP2) & HWCAP2_AES) != 0;
#else
    return false;
#endif
}

#endif /* defined(__x86_64__) || defined(__i386) */

int main(int argc, char ** argv)
{
    const bool support = platform_aes_hw_available();
    printf("This CPU %s AES-NI\n", support ? "supports" : "does not support");
    return support ? 0 : 1;
}

