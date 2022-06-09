#include "Password.h"

LPWSTR Password::generate(const int len)
{
    // This function is generating a new password in the length of len

    std::srand(std::time(0));

    // all the possible characters
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";


    wchar_t* tmp_s= new wchar_t[len + 1];
    memset(tmp_s, '\0', sizeof(tmp_s));

    // generating the password
    for (int i = 0; i < len; ++i) {
        tmp_s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    tmp_s[len] = '\0';

    return LPWSTR(tmp_s);
}