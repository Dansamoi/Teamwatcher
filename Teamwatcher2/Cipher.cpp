#include "Cipher.h"

void Cipher::xor_all(char* pdata, int buflen, char* pkey, int keylen)
{
    // This function performs xor encryption\decryption
    for (int i = 0; i < buflen; i++) {
        *(pdata + i) = *(pdata + i) ^ *(pkey + (i % keylen));
    }
}

long long int Cipher::power(long long int a, long long int b, long long int P)
    // This function return (a power b) % P
{
    if (b == 1)
        return a;

    else
        return (((long long int)pow(a, b)) % P);
}

long long int Cipher::random_num()
{
    // This function return a random number between 0 < x < P - 1 
    std::srand(std::time(0));
    return std::rand() % (P - 2) + 1;
}
