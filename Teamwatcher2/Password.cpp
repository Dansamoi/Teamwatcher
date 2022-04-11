#include "Password.h"

LPWSTR Password::generate(const int len)
{
    std::srand(std::time(0));
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    wchar_t* tmp_s= new wchar_t[len + 1];
    memset(tmp_s, '\0', sizeof(tmp_s));

    for (int i = 0; i < len; ++i) {
        tmp_s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    tmp_s[len] = '\0';

    return LPWSTR(tmp_s);
}
