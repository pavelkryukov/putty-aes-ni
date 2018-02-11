/**
 * cpuid.c
 *
 * Checks if CPU has support of SHA instructions
 *
 * @author kryukov@frtk.ru
 * @version 4.0
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#ifndef SILENT
#include <stdio.h>
#endif

#if defined(__clang__) || defined(__GNUC__)

#include <cpuid.h>
static int CheckCPUsupportSHA()
{
    unsigned int CPUInfo[4];
    __cpuid(7, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    return CPUInfo[1] & (1 << 29); /* Check SHA */
}

#else /* defined(__clang__) || defined(__GNUC__) */

static int CheckCPUsupportAES()
{
    unsigned int CPUInfo[4];
    __cpuid(CPUInfo, 7);
    return CPUInfo[1] & (1 << 29); /* Check SHA */
}

#endif /* defined(__clang__) || defined(__GNUC__) */

int main(int argc, char ** argv)
{
    const int res = !CheckCPUsupportSHA();
#ifndef SILENT
    printf("This CPU %s SHA-NI\n", res ? "does not support" : "supports");
#endif
    return res;
}
