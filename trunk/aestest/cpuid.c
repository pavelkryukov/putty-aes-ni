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

int main(int argc, char ** argv)
{
    int aes = CheckCPUsupportAES ();
   
    if (argc < 2)
        printf("This CPU %s support AES\n", (aes) ? "does" : "doesn't");
    return (aes) ? 0 : 1;
}
