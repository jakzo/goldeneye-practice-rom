#include <string.h>

#ifdef __GNUC__
char *strchr(const char *s, int c)
#else
unsigned char *strchr(const unsigned char *s, int c)
#endif
{
    const unsigned char ch = c;
    while (*s != ch)
    {
        if (*s == 0)
            return NULL;
        s++;
    }
#ifdef __GNUC__
    return (char *)s;
#else
    return (unsigned char *)s;
#endif
}

#ifdef __GNUC__
size_t strlen(const char *s)
#else
size_t strlen(const unsigned char *s)
#endif
{
#ifdef __GNUC__
    const char *sc = s;
#else
    const unsigned char *sc = s;
#endif
    while (*sc)
        sc++;
    return sc - s;
}
void *memcpy(void *s1, const void *s2, size_t n)
{
    unsigned char *su1 = (unsigned char *)s1;
    const unsigned char *su2 = (const unsigned char *)s2;
    while (n > 0)
    {
        *su1++ = *su2++;
        n--;
    }
    return (void *)s1;
}
