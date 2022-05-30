#ifndef PASSWORD_H
#define PASSWORD_H

#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#pragma once
class Password
{
public:
	static LPWSTR generate(const int len);

	static void xor_encypt(char* pdata, int buflen, char* pkey, int keylen);
};

#endif // PASSWORD_H