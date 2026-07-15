#include <stdlib.h>
#include <string.h>
#include "xstdio.h"

#define BUFF_LEN 0x18

static u8 ldigs[] = "0123456789abcdef";
static u8 udigs[] = "0123456789ABCDEF";

#ifdef __GNUC__
extern void __ull_divremi(unsigned long long *div, unsigned long long *rem,
                          unsigned long long value, unsigned short divisor);
#endif

void _Litob(printf_struct *args, u8 type)
{
  u8 buff[BUFF_LEN];
  const u8 *digs;
  s32 base;
  s32 i;
  unsigned long long ullval;

  if (type == 'X')
  {
    digs = udigs;
  }
  else
  {
    digs = ldigs;
  }

  base = (type == 'o') ? 8 : ((type != 'x' && type != 'X') ? 10 : 16);
  i = BUFF_LEN;
  ullval = args->value.s64;

  if ((type == 'd' || type == 'i') && args->value.s64 < 0)
  {
    ullval = -ullval;
  }

#ifdef __GNUC__
  {
    unsigned long long quotient;
    unsigned long long remainder;

    __ull_divremi(&quotient, &remainder, ullval, base);

    if (ullval != 0 || args->precision != 0)
    {
      buff[--i] = digs[remainder];
    }

    args->value.s64 = quotient;

    while (args->value.s64 > 0 && i > 0)
    {
      __ull_divremi(&quotient, &remainder, args->value.s64, base);
      args->value.s64 = quotient;
      buff[--i] = digs[remainder];
    }
  }
#else
  if (ullval != 0 || args->precision != 0)
  {
    buff[--i] = digs[ullval % base];
  }

  args->value.s64 = ullval / base;

  while (args->value.s64 > 0 && i > 0)
  {

    lldiv_t qr = lldiv(args->value.s64, base);
    args->value.s64 = qr.quot;
    buff[--i] = digs[qr.rem];
  }
#endif

  args->part2_len = BUFF_LEN - i;

  memcpy(args->buff, buff + i, args->part2_len);

  if (args->part2_len < args->precision)
  {
    args->num_leading_zeros = args->precision - args->part2_len;
  }

  if (args->precision < 0 && (args->flags & (FLAGS_ZERO | FLAGS_MINUS)) == FLAGS_ZERO)
  {
    i = args->width - args->n0 - args->num_leading_zeros - args->part2_len;
    if (i > 0)
    {
      args->num_leading_zeros += i;
    }
  }
}
