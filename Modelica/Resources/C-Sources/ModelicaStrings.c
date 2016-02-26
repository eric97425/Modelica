/* ModelicaStrings.c - External functions for Modelica.Functions.Strings

   Copyright (C) 2002-2016, Modelica Association and DLR
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* The functions are mostly non-portable. The following #define's are used
   to define the system calls of the operating system

   _MSC_VER       : Microsoft Visual C++
   __GNUC__       : GNU C compiler
   MODELICA_EXPORT: Prefix used for function calls. If not defined, blank is used
                    Useful definitions:
                    - "static" that is all functions become static
                      (useful if file is included with other C-sources for an
                       embedded system)
                    - "__declspec(dllexport)" if included in a DLL and the
                      functions shall be visible outside of the DLL

   Release Notes:
      Oct. 27, 2015: by Thomas Beutlich, ITI GmbH
                     Added nonnull attributes/annotations (ticket #1436)

      Oct. 05, 2015: by Thomas Beutlich, ITI GmbH
                     Added function ModelicaStrings_hashString from ModelicaRandom.c
                     of https://github.com/DLR-SR/Noise (ticket #1662)

      Mar. 26, 2013: by Martin Otter, DLR
                     Introduced three (int) casts to avoid warning messages (ticket #1032)

      Jan. 11, 2013: by Jesper Mattsson, Modelon AB
                     Made code C89 compatible

      Jan.  5, 2013: by Martin Otter, DLR
                     Removed "static" declarations from the Modelica interface functions

      Sep. 24, 2004: by Martin Otter, DLR
                     Final cleaning up of the code

      Sep.  9, 2004: by Dag Bruck, Dynasim AB
                     Implementation of scan functions

      Aug. 19, 2004: by Martin Otter, DLR
                     Changed according to the decisions of the 37th
                     design meeting in Lund (see minutes)

      Jan.  7, 2002: by Martin Otter, DLR
                     Implemented a first version
*/

#if !defined(MODELICA_EXPORT)
#   define MODELICA_EXPORT
#endif
#if defined(__gnu_linux__)
#define _GNU_SOURCE 1
#endif

#include "ModelicaUtilities.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

/*
 * Non-null pointers and esp. null-terminated strings need to be passed to
 * external functions.
 *
 * The following macros handle nonnull attributes for GNU C and Microsoft SAL.
 */
#if defined(__GNUC__)
#define MODELICA_NONNULLATTR __attribute__((nonnull))
#if defined(__GNUC_MINOR__) && (__GNUC__ > 3 && __GNUC_MINOR__ > 8)
#define MODELICA_RETURNNONNULLATTR __attribute__((returns_nonnull))
#else
#define MODELICA_RETURNNONNULLATTR
#endif
#elif defined(__ATTR_SAL)
#define MODELICA_NONNULLATTR
#define MODELICA_RETURNNONNULLATTR _Ret_z_ /* _Ret_notnull_ and null-terminated */
#else
#define MODELICA_NONNULLATTR
#define MODELICA_RETURNNONNULLATTR
#endif
#if !defined(__ATTR_SAL)
#define _In_z_
#define _Out_
#endif

MODELICA_EXPORT MODELICA_RETURNNONNULLATTR const char* ModelicaStrings_substring(
    _In_z_ const char* string, int startIndex, int endIndex) MODELICA_NONNULLATTR;
MODELICA_EXPORT int ModelicaStrings_length(_In_z_ const char* string) MODELICA_NONNULLATTR;
MODELICA_EXPORT int ModelicaStrings_skipWhiteSpace(_In_z_ const char* string,
    int i) MODELICA_NONNULLATTR;
MODELICA_EXPORT void ModelicaStrings_scanIdentifier(_In_z_ const char* string,
    int startIndex, _Out_ int* nextIndex, _Out_ const char** identifier) MODELICA_NONNULLATTR;
MODELICA_EXPORT void ModelicaStrings_scanInteger(_In_z_ const char* string,
    int startIndex, int unsignedNumber, _Out_ int* nextIndex,
    _Out_ int* integerNumber) MODELICA_NONNULLATTR;
MODELICA_EXPORT void ModelicaStrings_scanReal(_In_z_ const char* string, int startIndex,
    int unsignedNumber, _Out_ int* nextIndex, _Out_ double* number) MODELICA_NONNULLATTR;
MODELICA_EXPORT void ModelicaStrings_scanString(_In_z_ const char* string, int startIndex,
    _Out_ int* nextIndex, _Out_ const char** result) MODELICA_NONNULLATTR;
MODELICA_EXPORT int ModelicaStrings_hashString(_In_z_ const char* str) MODELICA_NONNULLATTR;

MODELICA_EXPORT const char* ModelicaStrings_substring(const char* string, int startIndex, int endIndex) {
    /* Return string1(startIndex:endIndex) if endIndex >= startIndex,
       or return string1(startIndex:startIndex), if endIndex = 0.
       An assert is triggered, if startIndex/endIndex are not valid.
     */
    char* substring;
    int len1 = (int) strlen(string);
    int len2;

    /* Check arguments */
    if ( startIndex < 1 ) {
        ModelicaFormatError("Wrong call of Utilities.Strings.substring:\n"
                            "  startIndex = %d (has to be > 0).\n"
                            "  string     = \"%s\"\n", startIndex, string);
    }
    else if ( endIndex == -999 ) {
        endIndex = startIndex;
    }
    else if ( endIndex < startIndex ) {
        ModelicaFormatError("Wrong call of  Utilities.Strings.substring:\n"
                            "  startIndex = %d\n"
                            "  endIndex   = %d (>= startIndex required)\n"
                            "  string     = \"%s\"\n", startIndex, endIndex, string);
    }
    else if ( endIndex > len1 ) {
        ModelicaFormatError("Wrong call of Utilities.Strings.substring:\n"
                            "  endIndex = %d (<= %d required (=length(string)).\n"
                            "  string   = \"%s\"\n", endIndex, len1, string);
    }

    /* Allocate memory and copy string */
    len2 = endIndex - startIndex + 1;
    substring = ModelicaAllocateString(len2);
    strncpy(substring, &string[startIndex-1], len2);
    substring[len2] = '\0';
    return substring;
}

MODELICA_EXPORT int ModelicaStrings_length(const char* string) {
    /* Return the number of characters "string" */
    return (int) strlen(string);
}

MODELICA_EXPORT int ModelicaStrings_compare(const char* string1, const char* string2, int caseSensitive) {
    /* Compare two strings, optionally ignoring case */
    int result;
    if (string1 == 0 || string2 == 0) {
        return 2;
    }

    if (caseSensitive) {
        result = strcmp(string1, string2);
    }
    else {
        while (tolower((unsigned char)*string1) == tolower((unsigned char)*string2) && *string1 != '\0') {
            string1++;
            string2++;
        }
        result = (int)(tolower((unsigned char)*string1)) - (int)(tolower((unsigned char)*string2));
    }

    if ( result < 0 ) {
        result = 1;
    }
    else if ( result == 0 ) {
        result = 2;
    }
    else {
        result = 3;
    }
    return result;
}

#define MAX_TOKEN_SIZE 100

MODELICA_EXPORT int ModelicaStrings_skipWhiteSpace(const char* string, int i) {
    /* Return index in string after skipping ws, or position of terminating nul. */
    while (string[i-1] != '\0' && isspace((unsigned char)string[i-1])) {
        ++i;
    }
    return i;
}

/* ----------------- utility functions used in scanXXX functions ----------- */

static int InSet(const char* string, int i, const char* separators) {
    /* Return true if string[i] is one of the characters in separators. */
    return strchr(separators, string[i-1]) != NULL;
}

static int SkipNonWhiteSpaceSeparator(const char* string, int i, const char* separators) {
    /* Return index in string of first character which is ws or character in separators,
       or position of terminating nul.
     */
    while (string[i-1] != '\0' && (isspace((unsigned char)string[i-1]) || InSet(string, i, separators))) {
        ++i;
    }
    return i;
}

static int get_token(const char* string, int startIndex, const char* separators,
                     int* output_index, int* token_start, int* token_length) {
    int past_token;
    int sep_pos;

    *token_start = ModelicaStrings_skipWhiteSpace(string, startIndex);
    /* Index of first char of token, after ws. */

    past_token = SkipNonWhiteSpaceSeparator(string, *token_start, separators);
    /* Index of first char after token, ws or separator. */

    sep_pos = ModelicaStrings_skipWhiteSpace(string, past_token);
    /* Index of first char after ws after token, maybe a separator. */

    *output_index = InSet(string, sep_pos, separators) ? sep_pos + 1 : sep_pos;
    /* Skip any separator. */

    *token_length = past_token-*token_start;

    if (*token_length == 0 || *token_length > MAX_TOKEN_SIZE) {
        /* Token missing or too long. */
        *output_index = startIndex;
        return 0; /* error */
    }

    return 1; /* ok */
}

static int MatchUnsignedInteger(const char* string, int start) {
    /* Starts matching character which make an unsigned integer. The matching
       begins at the start index (first char has index 1). Returns the number
       of characters that could be matched, or zero if the first character
       was not a digit.
     */
    const char* begin = &string[start-1];
    const char* p = begin;
    while (*p != '\0' && isdigit((unsigned char)*p)) {
        ++p;
    }
    return (int) (p - begin);
}

/* --------------- end of utility functions used in scanXXX functions ----------- */

MODELICA_EXPORT void ModelicaStrings_scanIdentifier(const char* string, int startIndex, int* nextIndex, const char** identifier) {
    int token_start = ModelicaStrings_skipWhiteSpace(string, startIndex);
    /* Index of first char of token, after ws. */

    if (isalpha((unsigned char)string[token_start-1])) {
        /* Identifier has begun. */
        int token_length = 1;
        while (string[token_start+token_length-1] != '\0' &&
            (isalpha((unsigned char)string[token_start+token_length-1]) ||
            isdigit((unsigned char)string[token_start+token_length-1]) ||
            string[token_start+token_length-1] == '_')) {
            ++token_length;
        }

        {
            char* s = ModelicaAllocateString(token_length);
            strncpy(s, string+token_start-1, token_length);
            s[token_length] = '\0';
            *nextIndex = token_start + token_length;
            *identifier = s;
            return;
        }
    }

    /* Token missing or not identifier. */
    *nextIndex  = startIndex;
    *identifier = ModelicaAllocateString(0);
    return;
}

MODELICA_EXPORT void ModelicaStrings_scanInteger(const char* string, int startIndex, int unsignedNumber,
                                 int* nextIndex, int* integerNumber) {
    int sign = 0;
    /* Number of characters used for sign. */

    int token_start = ModelicaStrings_skipWhiteSpace(string, startIndex);
    /* Index of first char of token, after ws. */

    if (string[token_start-1] == '+' || string[token_start-1] == '-') {
        sign = 1;
    }

    if (unsignedNumber==0 || (unsignedNumber==1 && sign==0)) {
        int number_length = MatchUnsignedInteger(string, token_start + sign);
        /* Number of characters in unsigned number. */

        if (number_length > 0 && sign + number_length < MAX_TOKEN_SIZE) {
            /* check if the scanned string is no Real number */
            int next = token_start + sign + number_length - 1;
            if ( string[next] == '\0' ||
                (string[next] != '\0' && string[next] != '.'
                                      && string[next] != 'e'
                                      && string[next] != 'E') ) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
                _locale_t loc = _create_locale(LC_NUMERIC, "C");
#elif defined(__GLIBC__) && defined(__GLIBC_MINOR__) && ((__GLIBC__ << 16) + __GLIBC_MINOR__ >= (2 << 16) + 3)
                locale_t loc = newlocale(LC_NUMERIC, "C", NULL);
#endif
                char buf[MAX_TOKEN_SIZE+1];
                /* Buffer for copying the part recognized as the number for passing to strtol(). */
                char* endptr;
                /* For error checking of strtol(). */
                int x;
                /* For receiving the result. */

                strncpy(buf, string+token_start-1, sign + number_length);
                buf[sign + number_length] = '\0';
#if defined(_MSC_VER) && _MSC_VER >= 1400
                x = (int)_strtol_l(buf, &endptr, 10, loc);
                _free_locale(loc);
#elif defined(__GLIBC__) && defined(__GLIBC_MINOR__) && ((__GLIBC__ << 16) + __GLIBC_MINOR__ >= (2 << 16) + 3)
                x = (int)strtol_l(buf, &endptr, 10, loc);
                freelocale(loc);
#else
                x = (int)strtol(buf, &endptr, 10);
#endif
                if (*endptr == 0) {
                    *integerNumber = x;
                    *nextIndex = token_start + sign + number_length;
                    return;
                }
            }
            else {
                ++number_length;
            }
        }
    }

    /* Token missing or cannot be converted to result type. */
    *nextIndex     = startIndex;
    *integerNumber = 0;
    return;
}

MODELICA_EXPORT void ModelicaStrings_scanReal(const char* string, int startIndex, int unsignedNumber,
                              int* nextIndex, double* number) {
    /*
    Grammar of real number:

    real ::= [sign] unsigned [fraction] [exponent]
    sign ::= '+' | '-'
    unsigned ::= digit [unsigned]
    fraction ::= '.' [unsigned]
    exponent ::= ('e' | 'E') [sign] unsigned
    digit ::= '0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'
    */

    int len = 0;
    /* Temporary variable for the length of a matched unsigned number. */

    int total_length = 0;
    /* Total number of characters recognized as part of a decimal number. */

    int token_start = ModelicaStrings_skipWhiteSpace(string, startIndex);
    /* Index of first char of token, after ws. */

    int exp_len = 0;
    /* Total number of characters recognized as part of the non-numeric parts
     * of exponent (the 'e' and the sign). */

    /* Scan sign of decimal number */

    if (string[token_start-1] == '+' || string[token_start-1] == '-') {
        total_length = 1;
        if (unsignedNumber == 1) {
            goto Modelica_ERROR;
        }
    }

    /* Scan integer part of mantissa. */

    len = MatchUnsignedInteger(string, token_start + total_length);
    total_length += len;

    /* Scan decimal part of mantissa. */

    if (string[token_start + total_length-1] == '.') {
        total_length += 1;
        len = MatchUnsignedInteger(string, token_start + total_length);
        if (len > 0) {
            total_length += len;
        }
    }

    /* Scan exponent part of mantissa. */

    if (string[token_start + total_length-1] == 'e' || string[token_start + total_length-1] == 'E') {
        /* total_length += 1; */
        exp_len = 1;

        if (string[token_start + total_length] == '+' || string[token_start + total_length] == '-') {
            exp_len += 1;
        }
        len = MatchUnsignedInteger(string, token_start + total_length + exp_len);
        if (len == 0) {
            goto Modelica_ERROR;
        }
        total_length += exp_len + len;
    }

    /* Convert accumulated characters into a number. */

    if (total_length > 0 && total_length < MAX_TOKEN_SIZE) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
        _locale_t loc = _create_locale(LC_NUMERIC, "C");
#elif defined(__GLIBC__) && defined(__GLIBC_MINOR__) && ((__GLIBC__ << 16) + __GLIBC_MINOR__ >= (2 << 16) + 3)
        locale_t loc = newlocale(LC_NUMERIC, "C", NULL);
#else
        char* dec = localeconv()->decimal_point;
#endif
        char buf[MAX_TOKEN_SIZE+1];
        /* Buffer for copying the part recognized as the number for passing to strtod(). */
        char* endptr;
        /* For error checking of strtod(). */
        double x;
        /* For receiving the result. */

        strncpy(buf, string+token_start-1, total_length);
        buf[total_length] = '\0';
#if defined(_MSC_VER) && _MSC_VER >= 1400
        x = _strtod_l(buf, &endptr, loc);
        _free_locale(loc);
#elif defined(__GLIBC__) && defined(__GLIBC_MINOR__) && ((__GLIBC__ << 16) + __GLIBC_MINOR__ >= (2 << 16) + 3)
        x = strtod_l(buf, &endptr, loc);
        freelocale(loc);
#else
        if (*dec == '.') {
            x = strtod(buf, &endptr);
        }
        else if (NULL == strchr(buf, '.')) {
            x = strtod(buf, &endptr);
        }
        else {
            char* p = strchr(buf, '.');
            *p = *dec;
            x = strtod(buf, &endptr);
        }
#endif
        if (*endptr == 0) {
            *number = x;
            *nextIndex = token_start + total_length;
            return;
        }
    }

    /* Token missing or cannot be converted to result type. */

Modelica_ERROR:
    *nextIndex = startIndex;
    *number = 0;
    return;
}

MODELICA_EXPORT void ModelicaStrings_scanString(const char* string, int startIndex,
                                int* nextIndex, const char** result) {
    int i, token_start, past_token, token_length;

    token_length = 0;
    token_start = ModelicaStrings_skipWhiteSpace(string, startIndex);
    i = token_start;
    if (string[token_start-1] != '"') {
        goto Modelica_ERROR;
    }
    /* Index of first char of token, after ws. */

    ++i;
    while (1) {
        if (string[i-1] == '\0') {
            goto Modelica_ERROR;
        }
        if (string[i-2] == '\\' && string[i-1] == '"')
            ; /* escaped quote, consume */
        else if (string[i-1] == '"') {
            break; /* end quote */
        }
        ++i;
    }
    past_token = i + 1;
    /* Index of first char after token, ws or separator. */

    token_length = past_token-token_start-2;

    if (token_length > 0) {
        char* s = ModelicaAllocateString(token_length);
        strncpy(s, string+token_start, token_length);
        s[token_length] = '\0';
        *result = s;
        *nextIndex = past_token;
        return;
    }

Modelica_ERROR:
    *result = ModelicaAllocateString(0);
    *nextIndex = startIndex;
    return;
}

MODELICA_EXPORT int ModelicaStrings_hashString(const char* inStr) {
    /* Compute an unsigned int hash code from a character string
     *
     * Author: Arash Partow - 2002                                            *
     * URL: http://www.partow.net                                             *
     * URL: http://www.partow.net/programming/hashfunctions/index.html        *
     *                                                                        *
     * Copyright notice:                                                      *
     * Free use of the General Purpose Hash Function Algorithms Library is    *
     * permitted under the guidelines and in accordance with the most current *
     * version of the Common Public License.                                  *
     * http://www.opensource.org/licenses/cpl1.0.php                          *
     */
    unsigned int hash = 0xAAAAAAAA;
    unsigned int i    = 0;
    unsigned int len  = (unsigned int)strlen(inStr);
    const unsigned char *str =(const unsigned char*)(inStr); 
    /* Use unsigned char to be independent of compiler settings, */
    /* https://trac.modelica.org/Modelica/ticket/1926 */

    union hash_tag {
        unsigned int iu;
        int          is;
    } h;

    for(i = 0; i < len; str++, i++) {
        hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^  (*str) * (hash >> 3)) :
                                 (~((hash << 11) + ((*str) ^ (hash >> 5))));
    }

    h.iu = hash;
    return h.is;
}
