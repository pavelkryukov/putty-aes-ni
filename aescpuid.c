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

extern int platform_aes_hw_available();

int main(int argc, char ** argv)
{
    const int res = !platform_aes_hw_available();
    printf("This CPU %s AES-NI\n", res ? "does not support" : "supports");
    return res;
}
