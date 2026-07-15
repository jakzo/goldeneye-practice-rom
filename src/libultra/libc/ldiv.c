#include <stdlib.h>

ldiv_t ldiv(long num, long denom)
{
    ldiv_t ret;

    ret.quot = num / denom;
    ret.rem = num - denom * ret.quot;
    if (ret.quot < 0 && ret.rem > 0)
    {
        ret.quot++;
        ret.rem -= denom;
    }

    return ret;
}

lldiv_t lldiv(long long num, long long denom)
{
    lldiv_t ret;

#ifdef __GNUC__
    extern long long __ll_div(long long, long long);
    extern unsigned long long __ll_mul(unsigned long long,
                                       unsigned long long);

    ret.quot = __ll_div(num, denom);
    ret.rem = num - (long long)__ll_mul((unsigned long long)denom,
                                        (unsigned long long)ret.quot);
#else
    ret.quot = num / denom;
    ret.rem = num - denom * ret.quot;
#endif
    if (ret.quot < 0 && ret.rem > 0)
    {
        ret.quot++;
        ret.rem -= denom;
    }

    return ret;
}
