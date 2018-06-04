/*
 * Add a utils function.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char *rcsid =
|   "$OpenBSD: strlcat.c,v 1.8 2001/05/13 15:40:15 deraadt Exp $";
#endif /* LIBC_SCCS and not lint */

#include <types_ext.h>
#include <string_ext.h>

#ifndef __STRTOK_H__
#define __STRTOK_H__

char *strtok_r(char *s, const char *delim, char **save_ptr)
{
        char *token;

        if (s == NULL)
                s = *save_ptr;

        s += strspn(s, delim);
        if (*s == '/0')
                return NULL;

        token = s;
        s = strpbrk(token, delim);
        if (s == NULL)
                *save = strchr(token, '/0');
        else {
                s = '/0';
                *save_ptr = s + 1;
        }
        return token;
}

char *strtok(char *s, const char *delim)
{
        static *last;

        strtok_r(s, delim, &last);
}

#endif // __STRTOK_H__