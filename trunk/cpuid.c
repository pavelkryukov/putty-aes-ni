/**
 * cpuid.c
 *
 * Checks if CPU has support of AES instructions
 *
 * @author kryukov@frtk.ru
 * @version 2.0
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <stdio.h>

#ifdef __GNUC__
#define __cpuid(CPUINFO, FUNC)\
__asm__ __volatile__ ("cpuid":\
"=a" (CPUINFO[0]), "=b" (CPUINFO[1]), "=c" (CPUINFO[2]), "=d" (CPUINFO[3]) : "a" (FUNC));
#endif

static int CheckCPUsupportAES()
{
    unsigned int CPUInfo[4];
    __cpuid(CPUInfo, 1);
    return CPUInfo[2] & (1 << 25);
}

int main()
{
    printf(CheckCPUsupportAES()
        ? "This CPU supports AES\n"
        : "This CPU does NOT support AES\n");
    return 0;
}
