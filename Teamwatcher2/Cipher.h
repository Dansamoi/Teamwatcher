#pragma once
#include <math.h>
#include <cstdlib>
#include <iostream>
#include <ctime>

class Cipher
    // Class of all the chiper for the data transfer
{
private:
    long long int P = 10472912312; // Prime number P
    long long int G = 3126565; // Primitive root of P

public:
    // Xor Encryption\Decryption function
    void xor_all(char* pdata, int buflen, char* pkey, int keylen);

    // power a**b % P, return 8 bytes long number
    long long int power(long long int a, long long int b, long long int P);

    long long int random_num();
};

