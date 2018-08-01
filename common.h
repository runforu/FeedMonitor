#ifndef _COMMON_H_
#define _COMMON_H_

#define MAX_SYMBOL_SIZE 128

#define TERMINATE_STR(str) str[sizeof(str) - 1] = 0;
#define COPY_STR(dst, src)                    \
    {                                         \
        strncpy_s(dst, src, sizeof(dst) - 1); \
        dst[sizeof(dst) - 1] = 0;             \
    }

double __fastcall NormalizeDouble(const double val, int digits);


double DecPow(const int digits);

char* RemoveWhiteChar(char* str);

char* StrRange(char* str, const char begin, const char end, char** buf);

int CStrToInt(char* string);

int ToOrderType(const char* type, int default_value);

bool IsDigitalStr(char* string);

#endif  // !_COMMON_H_