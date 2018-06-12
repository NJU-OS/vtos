#ifndef __MATH_H__
#define __MATH_H__

#if defined(CC_USING_HOTPATCH) && !defined(__CHECKER__)
#define notrace __attribute__((hotpatch(0,0)))
#else
#define notrace __attribute__((no_instrument_function))
#endif


uint32_t notrace __div64_32(uint64_t *n, uint32_t base)
{
    uint64_t rem = *n;
    uint64_t b = base;
    uint64_t res, d = 1;
    uint32_t high = rem >> 32;
    /* Reduce the thing a bit first */
    res = 0;
    if (high >= base) {
        high /= base;
        res = (uint64_t) high << 32;
        rem -= (uint64_t) (high*base) << 32;
    }

    while ((int64_t)b > 0 && b < rem) {
        b = b+b;
        d = d+d;
    }

    do {
        if (rem >= b) {
            rem -= b;
            res += d;
        }
        b >>= 1;
        d >>= 1;
    } while (d);

    *n = res;
    return rem;
}


# define do_div(n,base) ({              \
            uint32_t __base = (base);   \
            uint32_t __rem;               \
            (void)(((typeof((n)) *)0) == ((uint64_t *)0));  \
            if (((n) >> 32) == 0) {                         \
                __rem = (uint32_t)(n) % __base;             \
                (n) = (uint32_t)(n) / __base;               \
            } else \
                __rem = __div64_32(&(n), __base);   \
            __rem;                                  \
        })

#endif /* __MATH_H__*/