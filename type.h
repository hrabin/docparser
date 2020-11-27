#ifndef _TYPE_H_INCLUDED_
#define _TYPE_H_INCLUDED_

#define  U8VALUE_INVALID   0xFF
#define U16VALUE_INVALID 0xFFFF

#include	<stdint.h>

#if defined(__alpha__)\
 ||defined(__ia64__)\
 ||defined(__ppc64__)\
 ||defined(__s390x__)\
 ||defined(__x86_64__)
 #include	<sys/types.h>
 #define	SCNld   "d"
 #define	SCNlX   "X"
#else
 #define	SCNld  "ld"
 #define	SCNlX  "lX"
#endif

#define     ON              1
#define     OFF             0

#ifndef __cplusplus
	#ifndef     true
	#define     true        1
	#endif

	#ifndef     false
	#define     false       0
	#endif

	#ifndef NULL
	#define NULL            ((void *)0)
	#endif

	typedef signed char     bool;
#else
	#ifndef NULL
	#define NULL            0
	#endif
#endif
#ifndef FALSE
    #define FALSE           (0)
#endif

#ifndef TRUE
    #define TRUE            (1)
#endif

typedef char                ascii;
typedef unsigned char       byte;

typedef signed char         s8;
typedef int16_t             s16;
typedef int32_t             s32;
typedef int64_t             s64;

typedef unsigned char       u8;
typedef uint16_t            u16;
typedef uint32_t            u32;
typedef uint64_t            u64;

#define	U8_MAX        0xFF
#define	U16_MAX       0xFFFF
#define	U32_MAX       0xFFFFFFFF

#define	S8_MAX        0x7F
#define	S16_MAX       0x7FFF
#define	S32_MAX       0x7FFFFFFF

#define	S8_MIN        -0x80
#define	S16_MIN       -0x8000
#define	S32_MIN       -0x80000000

#ifndef KB
	#define KB 1024
#endif

#endif/*_TYPE_H_INCLUDED_*/
