#ifndef __IO_H__
#define __IO_H__

#define LITTLE_ENDIAN 1
#define __CHECKER__ 1

#define __force 

typedef uint8_t __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif
#ifdef __CHECK_ENDIAN__
#define __bitwise __bitwise__
#else
#define __bitwise
#endif

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
#if defined(__GNUC__)
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;
#endif
typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;

/*
 * Allow constant folding
 */
#if defined(__GNUC__) && (__GNUC__ >= 2) && defined(__OPTIMIZE__)
#  define __swab16(x) \
    (__builtin_constant_p((__u16)(x)) ?         \
     ___swab16((x)) :                           \
     __fswab16((x)))
#  define __swab32(x) \
    (__builtin_constant_p((__u32)(x)) ?         \
     ___swab32((x)) :                           \
     __fswab32((x)))
#  define __swab64(x) \
    (__builtin_constant_p((__u64)(x)) ?         \
     ___swab64((x)) :                           \
     __fswab64((x)))
#else
#  define __swab16(x) __fswab16(x)
#  define __swab32(x) __fswab32(x)
#  define __swab64(x) __fswab64(x)
#endif /* OPTIMIZE */

#if 0 /*big endian*/

#define __cpu_to_le64(x) ((__force __le64)__swab64((x)))
#define __le64_to_cpu(x) __swab64((__force __u64)(__le64)(x))
#define __cpu_to_le32(x) ((__force __le32)__swab32((x)))
#define __le32_to_cpu(x) __swab32((__force __u32)(__le32)(x))
#define __cpu_to_le16(x) ((__force __le16)__swab16((x)))
#define __le16_to_cpu(x) __swab16((__force __u16)(__le16)(x))
#define __cpu_to_be64(x) ((__force __be64)(__u64)(x))
#define __be64_to_cpu(x) ((__force __u64)(__be64)(x))
#define __cpu_to_be32(x) ((__force __be32)(__u32)(x))
#define __be32_to_cpu(x) ((__force __u32)(__be32)(x))
#define __cpu_to_be16(x) ((__force __be16)(__u16)(x))
#define __be16_to_cpu(x) ((__force __u16)(__be16)(x))

#else /* little endian*/

#define __cpu_to_le64(x) ((__force __le64)(__u64)(x))
#define __le64_to_cpu(x) ((__force __u64)(__le64)(x))
#define __cpu_to_le32(x) ((__force __le32)(__u32)(x))
#define __le32_to_cpu(x) ((__force __u32)(__le32)(x))
#define __cpu_to_le16(x) ((__force __le16)(__u16)(x))
#define __le16_to_cpu(x) ((__force __u16)(__le16)(x))
#define __cpu_to_be64(x) ((__force __be64)__swab64((x)))
#define __be64_to_cpu(x) __swab64((__force __u64)(__be64)(x))
#define __cpu_to_be32(x) ((__force __be32)__swab32((x)))
#define __be32_to_cpu(x) __swab32((__force __u32)(__be32)(x))
#define __cpu_to_be16(x) ((__force __be16)__swab16((x)))
#define __be16_to_cpu(x) __swab16((__force __u16)(__be16)(x))

#endif

#define cpu_to_le64(x)		__cpu_to_le64(x)
#define e64_to_cpu(x)		__le64_to_cpu(x)
#define cpu_to_le32(x)		__cpu_to_le32(x)
#define le32_to_cpu(x)		__le32_to_cpu(x)
#define cpu_to_le16(x)		__cpu_to_le16(x)
#define le16_to_cpu(x)		__le16_to_cpu(x)
#define cpu_to_be64(x)	__cpu_to_be64(x)
#define be64_to_cpu(x)	__be64_to_cpu(x)
#define pu_to_be32(x)		__cpu_to_be32(x)
#define be32_to_cpu(x)	__be32_to_cpu(x)
#define cpu_to_be16(x)	__cpu_to_be16(x)
#define be16_to_cpu(x)	 __be16_to_cpu(x)

/*
 * TODO: The kernel offers some more advanced versions of barriers, it might
 * have some advantages to use them instead of the simple one here.
 */
#define mb()		asm volatile("dsb sy" : : : "memory")
#define dmb()	__asm__ __volatile__ ("" : : : "memory")

#define __iormb()  	dmb()
#define __iowmb() 	dmb()

#define __arch_getb(a)		(*(volatile unsigned char *)(a))
#define __arch_getw(a)	(*(volatile unsigned short *)(a))
#define __arch_getl(a)		(*(volatile unsigned int *)(a))
#define __arch_getq(a)		(*(volatile unsigned long long *)(a))

#define __arch_putb(v, a)		(*(volatile unsigned char *)(a) = (v))
#define __arch_putw(v, a)		(*(volatile unsigned short *)(a) = (v))
#define __arch_putl(v, a)		(*(volatile unsigned int *)(a) = (v))
#define __arch_putq(v, a)		(*(volatile unsigned long long *)(a) = (v))

#define __raw_readb(a)	__arch_getb(a)
#define __raw_readw(a)	__arch_getw(a)
#define __raw_readl(a)		__arch_getl(a)
#define __raw_readq(a)	__arch_getq(a)

#define __raw_writeb(v, a) 	__arch_putb(v, a)
#define __raw_writew(v, a) 	__arch_putw(v, a)
#define __raw_writel(v, a)		__arch_putl(v, a)
#define __raw_writeq(v, a) 	__arch_putq(v, a)

#define writeb(v,c)	({ u8  __v = v; __iowmb(); __arch_putb(__v,c); __v; })
#define writew(v,c)	({ u16 __v = v; __iowmb(); __arch_putw(__v,c); __v; })
#define writel(v,c)		({ u32 __v = v; __iowmb(); __arch_putl(__v,c); __v; })
#define writeq(v,c)	({ u64 __v = v; __iowmb(); __arch_putq(__v,c); __v; })

#define readb(c)   	({ u8  __v = __arch_getb(c); __iormb(); __v; })
#define readw(c)   ({ u16 __v = __arch_getw(c); __iormb(); __v; })
#define readl(c)   	({ u32 __v = __arch_getl(c); __iormb(); __v; })
#define readq(c)   	({ u64 __v = __arch_getq(c); __iormb(); __v; })

#define out_arch(type,endian,a,v) __raw_write##type(cpu_to_##endian(v),a)
#define in_arch(type,endian,a) endian##_to_cpu(__raw_read##type(a))

#define out_le64(a,v)  out_arch(q,le64,a,v)
#define out_le32(a,v)  out_arch(l,le32,a,v)
#define out_le16(a,v)  out_arch(w,le16,a,v)

#define in_le64(a) in_arch(q,le64,a)
#define in_le32(a) in_arch(l,le32,a)
#define in_le16(a) in_arch(w,le16,a)

#define clrsetbits(type, addr, clear, set) \
    out_##type((addr), (in_##type(addr) & ~(clear)) | (set))
#define clrsetbits_le32(addr, clear, set) clrsetbits(le32, addr, clear, set)

#define setbits(type, addr, set) \
    out_##type((addr), in_##type(addr) | (set))
#define setbits_le32(addr, set) setbits(le32, addr, set)

#endif /*__IO_H__*/