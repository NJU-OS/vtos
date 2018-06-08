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

char *strtok_r(char *s, const char *delim, char **save_ptr);
size_t strspn(const char *s, const char *accept);
char *strpbrk(const char *s, const char *accept);
char *strchr(const char *s, int c);

char *strtok_r(char *s, const char *delim, char **save_ptr)
{
    char *token;

    if (s == NULL)
        s = *save_ptr;

    s += strspn(s, delim);
    if (*s == '\0')
        return NULL;

    token = s;
    s = strpbrk(token, delim);
    if (s == NULL)
        *save_ptr = strchr(token, '\0');
    else {
        s = '\0';
        *save_ptr = s + 1;
    }
    return token;
}

char *strtok(char *s, const char *delim)
{
    static char *last;
    return strtok_r(s, delim, &last);
}

/* Find the first occurrence in S of any character in ACCEPT.  */
char *strpbrk (const char *s, const char *accept)
{
    while (*s != '\0')
    {
        const char *a = accept;
        while (*a != '\0')
            if (*a++ == *s)
                return (char *) s;
        ++s;
    }
    return NULL;
}

size_t strspn (const char *s, const char *accept)
{
    const char *p;
    const char *a;
    size_t count = 0;

    for (p = s; *p != '\0'; ++p)
    {
        for (a = accept; *a != '\0'; ++a)
            if (*p == *a)
                break;
        if (*a == '\0')
            return count;
        else
            ++count;
    }
    return count;
}


char *strchr (const char *s, int c_in)
{
    const unsigned char * __attribute__((aligned(__alignof__(const unsigned long int *)))) char_ptr ;
    const unsigned long int *longword_ptr;
    unsigned long int longword, magic_bits, charmask;
    unsigned char c;

    c = (unsigned char) c_in;

    /* Handle the first few characters by reading one character at a time.
       Do this until CHAR_PTR is aligned on a longword boundary.  */
    for (char_ptr = (const unsigned char *)s; ((unsigned long int) char_ptr
                        & (sizeof (longword) - 1)) != 0;
         ++char_ptr)
        if (*char_ptr == c)
            return (void *) char_ptr;
        else if (*char_ptr == '\0')
            return NULL;

    /* All these elucidatory comments refer to 4-byte longwords,
       but the theory applies equally well to 8-byte longwords.  */

    longword_ptr = (const unsigned long int *)char_ptr;
    
    /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
       the "holes."  Note that there is a hole just to the left of
       each byte, with an extra at the end:

       bits:  01111110 11111110 11111110 11111111
       bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

       The 1-bits make sure that carries propagate to the next 0-bit.
       The 0-bits provide holes for carries to fall into.  */
    switch (sizeof (longword)) {
    case 4:
        magic_bits = 0x7efefeffL;
        break;
    case 8:
        magic_bits = (long int)((0x7efefefeL << 16) << 16) | 0xfefefeffL;
        break;
    default:
        // here should call a abort, but there isn't concrete this func.
        // abort ();
        return NULL;
    }

    /* Set up a longword, each of whose bytes is C.  */
    charmask = c | (c << 8);
    charmask |= charmask << 16;
    if (sizeof (longword) > 4)
        /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
        charmask |= (charmask << 16) << 16;
    if (sizeof (longword) > 8)
        return NULL; // should abort ();


    /* Instead of the traditional loop which tests each character,
       we will test a longword at a time.  The tricky part is testing
       if *any of the four* bytes in the longword in question are zero.  */
    for (;;) {
        /* We tentatively exit the loop if adding MAGIC_BITS to
           LONGWORD fails to change any of the hole bits of LONGWORD.

           1) Is this safe?  Will it catch all the zero bytes?
           Suppose there is a byte with all zeros.  Any carry bits
           propagating from its left will fall into the hole at its
           least significant bit and stop.  Since there will be no
           carry from its most significant bit, the LSB of the
           byte to the left will be unchanged, and the zero will be
           detected.

           2) Is this worthwhile?  Will it ignore everything except
           zero bytes?  Suppose every byte of LONGWORD has a bit set
           somewhere.  There will be a carry into bit 8.  If bit 8
           is set, this will carry into bit 16.  If bit 8 is clear,
           one of bits 9-15 must be set, so there will be a carry
           into bit 16.  Similarly, there will be a carry into bit
           24.  If one of bits 24-30 is set, there will be a carry
           into bit 31, so all of the hole bits will be changed.

           The one misfire occurs when bits 24-30 are clear and bit
           31 is set; in this case, the hole at bit 31 is not
           changed.  If we had access to the processor carry flag,
           we could close this loophole by putting the fourth hole
           at bit 32!

           So it ignores everything except 128's, when they're aligned
           properly.

           3) But wait!  Aren't we looking for C as well as zero?
           Good point.  So what we do is XOR LONGWORD with a longword,
           each of whose bytes is C.  This turns each byte that is C
           into a zero.  */

        longword = *longword_ptr++;

        /* Add MAGIC_BITS to LONGWORD.  */
        if ((((longword + magic_bits)

              /* Set those bits that were unchanged by the addition.  */
              ^ ~longword)

             /* Look at only the hole bits.  If any of the hole bits
                are unchanged, most likely one of the bytes was a
                zero.  */
             & ~magic_bits) != 0 ||

            /* That caught zeroes.  Now test for C.  */
            ((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask))
             & ~magic_bits) != 0)
        {
            /* Which of the bytes was C or zero?
               If none of them were, it was a misfire; continue the search.  */

            const unsigned char *cp = (const unsigned char *) (longword_ptr - 1);

            if (*cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (*++cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (*++cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (*++cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (sizeof (longword) > 4)
            {
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
            }
        }
    }

    return NULL;
}

#endif // __STRTOK_H__char *