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

#ifndef SILENT
#include <stdio.h>
#endif

#if defined(__clang__) || defined(__GNUC__)

#include <cpuid.h>
static int CheckCPUsupportAES()
{
    unsigned int CPUInfo[4];
    __cpuid(1, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
    return CPUInfo[2] & (1 << 25);
}

#else /* defined(__clang__) || defined(__GNUC__) */

static int CheckCPUsupportAES()
{
    unsigned int CPUInfo[4];
    __cpuid(CPUInfo, 1);
    return CPUInfo[2] & (1 << 25);
}

#endif /* defined(__clang__) || defined(__GNUC__) */

int main(int argc, char ** argv)
{
    const int res = !CheckCPUsupportAES();
#ifndef SILENT
    printf("This CPU %s AES-NI\n", res ? "does not support" : "supports");
#endif
    return res;
}
