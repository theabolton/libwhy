/* y_sscanf.c - dual-locale sscanf
 *
 * Like the standard sscanf(3), but this version accepts floating point
 * numbers containing either the locale-specified decimal point or the
 * "C" locale decimal point (".").  That said, there are important
 * differences between this and a standard sscanf(3):
 * 
 *  - this function only skips whitespace when there is explicit ' ' in
 *      the format,
 *  - it doesn't return EOF like sscanf(3), just the number of sucessful
 *      conversions,
 *  - it doesn't match character classes,
 *  - there is only one length modifier: 'l', and it only applies to 'd'
 *      (for int64_t) and 'f' (for double),
 *  - '%s' with no field-width specified is an error,
 *  - there are no 'i, 'o', 'p', or 'u' conversions, similarly
 *  - use 'f' instead of 'a', 'A', 'e', 'E', 'F', 'g', or 'G', and
 *  - '%f' doesn't do hexadecimal, infinity or NaN.
 *
 * As part of libwhy, this file is licensed under the GNU Lesser General
 * Public License, version 2.1.
 *
 * Separately, this file is licensed under an MIT-style license as
 * follows:
 *
 * Copyright (c) 2010 Sean Bolton.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <locale.h>

#include "whyutil.h"

static int
_is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

static int
_is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

static int
_is_hexdigit(char c)
{
    return ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F'));
}

int
y_vsscanf(const char *buffer, const char *format, va_list ap)
{
    char fc;
    const char *bp = buffer;
    int conversions = 0;

    while ((fc = *format++)) {

        if (_is_whitespace(fc)) { /* skip whitespace */

            while (_is_whitespace(*bp))
                bp++;

        } else if (fc == '%') { /* a conversion */

            int skip = 0;   /* '*' no-store modifier */
            int width = 0;  /* field width */
            int big = 0;    /* storage length modifier */

            if (*format == '*') {
                skip = 1;
                format++;
            }
            while (_is_digit(*format)) {
                width = width * 10 + (*format - '0');
                format++;
            }
            if (*format == 'l') {
                big = 1;
                format++;
            }

            if (*format == '%') {  /* '%' - literal percent character */

                if (*bp == '%')
                    bp++;
                else
                    break;
                format++;

            } else if (*format == 'c') {  /* 'c' - one or more characters */

                int i;
                char *sp = va_arg(ap, char *);

                if (width == 0) width = 1;
                for (i = 0; i < width && *bp != 0; i++, bp++)
                    if (!skip) *sp++ = *bp;
                if (i > 0 && !skip)
                    conversions++;
                format++;

            } else if (*format == 'd') {  /* 'd' - 32 or 64 bit signed decimal integer */

                int negative = 0;
                int i;
                int64_t n = 0;

                if (*bp == '-') {
                    negative = 1;
                    bp++;
                }
                for (i = negative; (width == 0 || i < width) && _is_digit(*bp); i++, bp++)
                    n = n * 10 + (*bp - '0');
                if (i == negative) /* no digits converted */
                    break;
                if (negative) n = -n;
                if (!skip) {
                    if (big)
                        *va_arg(ap, int64_t *) = n;
                    else
                        *va_arg(ap, int32_t *) = n;
                    conversions++;
                }
                format++;

            } else if (*format == 'f') {  /* 'f' - float or double */

                double d;
                int n = y_atof(bp, &d);
                if (n == 0)  /* no digits converted */
                    break;
                if (!skip) {
                    if (big)
                        *va_arg(ap, double *) = d;
                    else
                        *va_arg(ap, float *) = (float)d;
                    conversions++;
                }
                bp += n;
                format++;

            } else if (*format == 'n') {  /* 'n' - store number of characters scanned so far */

                if (!skip) *va_arg(ap, int *) = bp - buffer;
                format++;

            } else if (*format == 's') {  /* 's' - string of non-whitespace characters */

                int i;
                char *sp = va_arg(ap, char *);
                if (width == 0)
                    break; /* must specify a width */
                for (i = 0; i < width && *bp != 0 && !_is_whitespace(*bp); i++, bp++)
                    if (!skip) *sp++ = *bp;
                if (i > 0) {
                    if (!skip) {
                        *sp = 0;
                        conversions++;
                    }
                } else
                    break;  /* conversion failed */
                format++;

            } else if (*format == 'x') {  /* 'x' - 32 or 64 bit signed hexidecimal integer */
                int i;
                int64_t n = 0;

                for (i = 0; (width == 0 || i < width) && _is_hexdigit(*bp); i++, bp++) {
                    n = n * 16;
                    if (*bp >= 'a') n += *bp - 'a' + 10;
                    else if (*bp >= 'A') n += *bp - 'A' + 10;
                    else n += *bp - '0';
                }
                if (i == 0) /* no digits converted */
                    break;
                if (!skip) {
                    if (big)
                        *va_arg(ap, int64_t *) = n;
                    else
                        *va_arg(ap, int32_t *) = n;
                    conversions++;
                }
                format++;

            } else {
                break; /* bad conversion character */
            }

        } else if (fc == *bp) { /* a literal match */

            bp++;

        } else {  /* match fail */
            break;
        }
    }

    return conversions;
}

/* The following function is based on sqlite3AtoF() from sqlite 3.6.18.
 * The sqlite author disclaims copyright to the source code from which
 * this was adapted. */
int
y_atof(const char *z, double *pResult){
  const char *zBegin = z;
  /* sign * significand * (10 ^ (esign * exponent)) */
  int sign = 1;   /* sign of significand */
  int64_t s = 0;  /* significand */
  int d = 0;      /* adjust exponent for shifting decimal point */
  int esign = 1;  /* sign of exponent */
  int e = 0;      /* exponent */
  double result;
  int nDigits = 0;
  struct lconv *lc = localeconv();
  int dplen = strlen(lc->decimal_point);

  /* skip leading spaces */
  /* while( _is_whitespace(*z) ) z++; */
  /* get sign of significand */
  if( *z=='-' ){
    sign = -1;
    z++;
  }else if( *z=='+' ){
    z++;
  }
  /* skip leading zeroes */
  while( z[0]=='0' ) z++, nDigits++;

  /* copy max significant digits to significand */
  while( _is_digit(*z) && s<((INT64_MAX-9)/10) ){
    s = s*10 + (*z - '0');
    z++, nDigits++;
  }
  /* skip non-significant significand digits
  ** (increase exponent by d to shift decimal left) */
  while( _is_digit(*z) ) z++, nDigits++, d++;

  /* if decimal point is present */
  if( *z=='.' || !strncmp(z, lc->decimal_point, dplen) ) {
    if (*z=='.')
      z++;
    else
      z += dplen;
    /* copy digits from after decimal to significand
    ** (decrease exponent by d to shift decimal right) */
    while( _is_digit(*z) && s<((INT64_MAX-9)/10) ){
      s = s*10 + (*z - '0');
      z++, nDigits++, d--;
    }
    /* skip non-significant digits */
    while( _is_digit(*z) ) z++, nDigits++;
  } else if (nDigits == 0)
      return 0;  /* no significand digits converted */

  /* if exponent is present */
  if( *z=='e' || *z=='E' ){
    int eDigits = 0;
    z++;
    /* get sign of exponent */
    if( *z=='-' ){
      esign = -1;
      z++;
    }else if( *z=='+' ){
      z++;
    }
    /* copy digits to exponent */
    while( _is_digit(*z) ){
      e = e*10 + (*z - '0');
      z++, eDigits++;
    }
    if (eDigits == 0)
        return 0; /* malformed exponent */
  }

  /* adjust exponent by d, and update sign */
  e = (e*esign) + d;
  if( e<0 ) {
    esign = -1;
    e *= -1;
  } else {
    esign = 1;
  }

  /* if 0 significand */
  if( !s ) {
    /* In the IEEE 754 standard, zero is signed.
    ** Add the sign if we've seen at least one digit */
    result = (sign<0 && nDigits) ? -(double)0 : (double)0;
  } else {
    /* attempt to reduce exponent */
    if( esign>0 ){
      while( s<(INT64_MAX/10) && e>0 ) e--,s*=10;
    }else{
      while( !(s%10) && e>0 ) e--,s/=10;
    }

    /* adjust the sign of significand */
    s = sign<0 ? -s : s;

    /* if exponent, scale significand as appropriate
    ** and store in result. */
    if( e ){
      double scale = 1.0;
      /* attempt to handle extremely small/large numbers better */
      if( e>307 && e<342 ){
        while( e%308 ) { scale *= 1.0e+1; e -= 1; }
        if( esign<0 ){
          result = s / scale;
          result /= 1.0e+308;
        }else{
          result = s * scale;
          result *= 1.0e+308;
        }
      }else{
        /* 1.0e+22 is the largest power of 10 than can be 
        ** represented exactly. */
        while( e%22 ) { scale *= 1.0e+1; e -= 1; }
        while( e>0 ) { scale *= 1.0e+22; e -= 22; }
        if( esign<0 ){
          result = s / scale;
        }else{
          result = s * scale;
        }
      }
    } else {
      result = (double)s;
    }
  }

  /* store the result */
  *pResult = result;

  /* return number of characters used */
  return (int)(z - zBegin);
}

int
y_sscanf(const char *str, const char *format, ...)
{
    va_list ap;
    int conversions;

    va_start(ap, format);
    conversions = y_vsscanf(str, format, ap);
    va_end(ap);

    return conversions;
}

