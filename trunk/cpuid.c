/**
 * cpuid.c
 *
 * Checks if CPU has support of AES instructions
 *
 * @author kryukov@frtk.ru
 * @version 1.0
 *
 * For Putty AES NI project
 * http://putty-aes-ni.googlecode.com/
 */

#include <stdio.h>

int Check_CPU_support_AES()
{
    unsigned int a,b,c,d;
    __asm__ __volatile__ ("cpuid":"=a" (a), "=b" (b), "=c" (c), "=d" (d) : "a" (1));
    return (c & 0x2000000);
}

int main()
{
    printf(Check_CPU_support_AES() ? "This CPU support AES\n" : "This CPU does NOT support AES\n");
    return 0;
}
