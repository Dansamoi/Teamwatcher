#pragma once
#ifndef CIPHER_H
#define CIPHER_H

#include <math.h>
#include <cstdlib>
#include <iostream>
#include <ctime>

class Cipher
    // Class of all the cipher for the data transfer
{
public:
    static const long long int P = 13; // Prime number P
    static const long long int G = 7; // Primitive root of P

    // Xor Encryption\Decryption function
    static void xor_all(char* pdata, int buflen, char* pkey, int keylen);

    // power a**b % P, return 8 bytes long number
    static long long int power(long long int a, long long int b, long long int P);

    // return 8 bytes long random number between 1 to p-1
    static long long int random_num();
};

#endif // CIPHER_H