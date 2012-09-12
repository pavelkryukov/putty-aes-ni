CFLAGS="-I ../Putty -std=c89 -Wall -pedantic -ansi -Werror"
gcc ./aestest.c ./puttymem.c $CFLAGS -c
gcc ./aestest.o ./puttymem.o ../Putty/sshaes.c   $CFLAGS -o sshaes.out
gcc ./aestest.o ./puttymem.o ../Putty/sshaesni.c $CFLAGS -o sshaesni.out
rm aestest.o puttymem.o
