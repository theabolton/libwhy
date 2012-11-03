/* escape_for_shell()
 *
 * Escape a string for use as an argument to a Bourne shell. The returned
 * string should be free()ed.
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
#include <stdlib.h>
#include <string.h>

void
escape_for_shell(char **p, const char *text)
{
    int len, i, c;

    if (*p) free(*p);

    if (!text || (len = strlen(text)) == 0) {
        *p = strdup("''");
        return;
    }

    for (i = 0, c = 0; i < len; i++)
        if (text[i] == '\'') c++;

    if (c == 0) {
        *p = (char *)malloc(len + 3);
        sprintf(*p, "'%s'", text);
        return;
    }

    *p = (char *)malloc(len + c * 3 + 3);
    c = 0;
    if (text[0] != '\'') {
        (*p)[c++] = '\'';
    }
    for (i = 0; i < len; i++) {
        if (text[i] == '\'') {
            if (i != 0) (*p)[c++] = '\'';
            (*p)[c++] = '\\';
            (*p)[c++] = '\'';
            if (i != len - 1) (*p)[c++] = '\'';
        } else {
            (*p)[c++] = text[i];
        }
    }
    if (text[len - 1] != '\'') {
        (*p)[c++] = '\'';
    }
    (*p)[c++] = '\0';
}

