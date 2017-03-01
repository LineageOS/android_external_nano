/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* CPU and C ABI indicator */
#ifndef __i386__
/* #undef __i386__ */
#endif
#ifndef __x86_64_x32__
/* #undef __x86_64_x32__ */
#endif
#ifndef __x86_64__
/* #undef __x86_64__ */
#endif
#ifndef __alpha__
/* #undef __alpha__ */
#endif
#ifndef __arm__
#define __arm__ 1
#endif
#ifndef __armhf__
/* #undef __armhf__ */
#endif
#ifndef __arm64_ilp32__
/* #undef __arm64_ilp32__ */
#endif
#ifndef __arm64__
/* #undef __arm64__ */
#endif
#ifndef __hppa__
/* #undef __hppa__ */
#endif
#ifndef __hppa64__
/* #undef __hppa64__ */
#endif
#ifndef __ia64_ilp32__
/* #undef __ia64_ilp32__ */
#endif
#ifndef __ia64__
/* #undef __ia64__ */
#endif
#ifndef __m68k__
/* #undef __m68k__ */
#endif
#ifndef __mips__
/* #undef __mips__ */
#endif
#ifndef __mipsn32__
/* #undef __mipsn32__ */
#endif
#ifndef __mips64__
/* #undef __mips64__ */
#endif
#ifndef __powerpc__
/* #undef __powerpc__ */
#endif
#ifndef __powerpc64__
/* #undef __powerpc64__ */
#endif
#ifndef __powerpc64_elfv2__
/* #undef __powerpc64_elfv2__ */
#endif
#ifndef __s390__
/* #undef __s390__ */
#endif
#ifndef __s390x__
/* #undef __s390x__ */
#endif
#ifndef __sh__
/* #undef __sh__ */
#endif
#ifndef __sparc__
/* #undef __sparc__ */
#endif
#ifndef __sparc64__
/* #undef __sparc64__ */
#endif


/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to the number of bits in type 'ptrdiff_t'. */
/* #undef BITSIZEOF_PTRDIFF_T */

/* Define to the number of bits in type 'sig_atomic_t'. */
/* #undef BITSIZEOF_SIG_ATOMIC_T */

/* Define to the number of bits in type 'size_t'. */
/* #undef BITSIZEOF_SIZE_T */

/* Define to the number of bits in type 'wchar_t'. */
/* #undef BITSIZEOF_WCHAR_T */

/* Define to the number of bits in type 'wint_t'. */
/* #undef BITSIZEOF_WINT_T */

/* Define if you wish *printf() functions that have a safe handling of
   non-IEEE-754 'long double' values. */
#define CHECK_PRINTF_SAFE 1

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define to 1 if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Define to 1 if the C locale may have encoding errors. */
#define C_LOCALE_MAYBE_EILSEQ 1

/* Define as the bit index in the word where to find bit 0 of the exponent of
   'double'. */
#define DBL_EXPBIT0_BIT 20

/* Define as the word index where to find the exponent of 'double'. */
#define DBL_EXPBIT0_WORD 1

/* Define as the bit index in the word where to find the sign of 'double'. */
/* #undef DBL_SIGNBIT_BIT */

/* Define as the word index where to find the sign of 'double'. */
/* #undef DBL_SIGNBIT_WORD */

/* Define this to enable debug messages and abortion on failing asserts. */
/* #undef DEBUG */

/* the name of the file descriptor member of DIR */
/* #undef DIR_FD_MEMBER_NAME */

#ifdef DIR_FD_MEMBER_NAME
# define DIR_TO_FD(Dir_p) ((Dir_p)->DIR_FD_MEMBER_NAME)
#else
# define DIR_TO_FD(Dir_p) -1
#endif


/* Define this to disable text wrapping as root by default. */
/* #undef DISABLE_ROOTWRAPPING */

/* Define this to enable the built-in file browser. */
#define ENABLE_BROWSER 1

/* Define this to have syntax highlighting. */
#define ENABLE_COLOR 1

/* Define this to enable the comment/uncomment function. */
#define ENABLE_COMMENT 1

/* Define this to have an Easter egg. */
#define ENABLE_EXTRA 1

/* Define this to enable the Ctrl+G help texts. */
#define ENABLE_HELP 1

/* Define this to have search and position histories. */
#define ENABLE_HISTORIES 1

/* Define this to have the routines for justifying. */
#define ENABLE_JUSTIFY 1

/* Define this to enable line numbering. */
#define ENABLE_LINENUMBERS 1

/* Define this to enable mouse support. */
#define ENABLE_MOUSE 1

/* Define this to enable multiple file buffers. */
#define ENABLE_MULTIBUFFER 1

/* Define this to enable the use of .nanorc files. */
#define ENABLE_NANORC 1

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
/* #undef ENABLE_NLS */

/* Define this to allow setting an operating directory (a chroot of sorts). */
#define ENABLE_OPERATINGDIR 1

/* Define this to have the spell-checker functions. */
#define ENABLE_SPELLER 1

/* Define this to have tab completion for filenames and search strings. */
#define ENABLE_TABCOMP 1

/* Define this if your system has sufficient UTF-8 support. */
/* #undef ENABLE_UTF8 */

/* Define this to enable the word-completion function. */
#define ENABLE_WORDCOMPLETION 1

/* Define this to have hard text wrapping. */
#define ENABLE_WRAPPING 1

/* Define to nothing if C supports flexible array members, and to 1 if it does
   not. That way, with a declaration like 'struct s { int n; double
   d[FLEXIBLE_ARRAY_MEMBER]; };', the struct hack can be used with pre-C99
   compilers. When computing the size of such an object, don't use 'sizeof
   (struct s)' as it overestimates the size. Use 'offsetof (struct s, d)'
   instead. Don't use 'offsetof (struct s, d[0])', as this doesn't work with
   MSVC and with C++ compilers. */
#define FLEXIBLE_ARRAY_MEMBER /**/

/* Define as the bit index in the word where to find bit 0 of the exponent of
   'float'. */
#define FLT_EXPBIT0_BIT 23

/* Define as the word index where to find the exponent of 'float'. */
#define FLT_EXPBIT0_WORD 0

/* Define as the bit index in the word where to find the sign of 'float'. */
/* #undef FLT_SIGNBIT_BIT */

/* Define as the word index where to find the sign of 'float'. */
/* #undef FLT_SIGNBIT_WORD */

/* Define to 1 if nl_langinfo (YESEXPR) returns a non-empty string. */
#define FUNC_NL_LANGINFO_YESEXPR_WORKS 1

/* Define to 1 if futimesat mishandles a NULL file name. */
/* #undef FUTIMESAT_NULL_BUG */

/* Define if gettimeofday clobbers the localtime buffer. */
/* #undef GETTIMEOFDAY_CLOBBERS_LOCALTIME */

/* Define this to 'void' or 'struct timezone' to match the system's
   declaration of the second argument to gettimeofday. */
#define GETTIMEOFDAY_TIMEZONE struct timezone

/* Define to a C preprocessor expression that evaluates to 1 or 0, depending
   whether the gnulib module fscanf shall be considered present. */
#define GNULIB_FSCANF 1

/* Define to a C preprocessor expression that evaluates to 1 or 0, depending
   whether the gnulib module lock shall be considered present. */
#define GNULIB_LOCK 1

/* Define to a C preprocessor expression that evaluates to 1 or 0, depending
   whether the gnulib module msvc-nothrow shall be considered present. */
#define GNULIB_MSVC_NOTHROW 1

/* Define to 1 if printf and friends should be labeled with attribute
   "__gnu_printf__" instead of "__printf__" */
/* #undef GNULIB_PRINTF_ATTRIBUTE_FLAVOR_GNU */

/* Define to a C preprocessor expression that evaluates to 1 or 0, depending
   whether the gnulib module scanf shall be considered present. */
#define GNULIB_SCANF 1

/* Define to a C preprocessor expression that evaluates to 1 or 0, depending
   whether the gnulib module snprintf shall be considered present. */
#define GNULIB_SNPRINTF 1

/* Define to 1 when the gnulib module btowc should be tested. */
#define GNULIB_TEST_BTOWC 1

/* Define to 1 when the gnulib module closedir should be tested. */
#define GNULIB_TEST_CLOSEDIR 1

/* Define to 1 when the gnulib module dirfd should be tested. */
#define GNULIB_TEST_DIRFD 1

/* Define to 1 when the gnulib module frexp should be tested. */
#define GNULIB_TEST_FREXP 1

/* Define to 1 when the gnulib module frexpl should be tested. */
#define GNULIB_TEST_FREXPL 1

/* Define to 1 when the gnulib module fstat should be tested. */
#define GNULIB_TEST_FSTAT 1

/* Define to 1 when the gnulib module futimens should be tested. */
#define GNULIB_TEST_FUTIMENS 1

/* Define to 1 when the gnulib module getdelim should be tested. */
#define GNULIB_TEST_GETDELIM 1

/* Define to 1 when the gnulib module getline should be tested. */
#define GNULIB_TEST_GETLINE 1

/* Define to 1 when the gnulib module getlogin_r should be tested. */
#define GNULIB_TEST_GETLOGIN_R 1

/* Define to 1 when the gnulib module gettimeofday should be tested. */
#define GNULIB_TEST_GETTIMEOFDAY 1

/* Define to 1 when the gnulib module iswblank should be tested. */
#define GNULIB_TEST_ISWBLANK 1

/* Define to 1 when the gnulib module localeconv should be tested. */
#define GNULIB_TEST_LOCALECONV 1

/* Define to 1 when the gnulib module lstat should be tested. */
#define GNULIB_TEST_LSTAT 1

/* Define to 1 when the gnulib module malloc-posix should be tested. */
#define GNULIB_TEST_MALLOC_POSIX 1

/* Define to 1 when the gnulib module mbrtowc should be tested. */
#define GNULIB_TEST_MBRTOWC 1

/* Define to 1 when the gnulib module mbsinit should be tested. */
#define GNULIB_TEST_MBSINIT 1

/* Define to 1 when the gnulib module mbsrtowcs should be tested. */
#define GNULIB_TEST_MBSRTOWCS 1

/* Define to 1 when the gnulib module mbtowc should be tested. */
#define GNULIB_TEST_MBTOWC 1

/* Define to 1 when the gnulib module memchr should be tested. */
#define GNULIB_TEST_MEMCHR 1

/* Define to 1 when the gnulib module mempcpy should be tested. */
#define GNULIB_TEST_MEMPCPY 1

/* Define to 1 when the gnulib module nl_langinfo should be tested. */
#define GNULIB_TEST_NL_LANGINFO 1

/* Define to 1 when the gnulib module opendir should be tested. */
#define GNULIB_TEST_OPENDIR 1

/* Define to 1 when the gnulib module raise should be tested. */
#define GNULIB_TEST_RAISE 1

/* Define to 1 when the gnulib module readdir should be tested. */
#define GNULIB_TEST_READDIR 1

/* Define to 1 when the gnulib module sigaction should be tested. */
#define GNULIB_TEST_SIGACTION 1

/* Define to 1 when the gnulib module signbit should be tested. */
#define GNULIB_TEST_SIGNBIT 1

/* Define to 1 when the gnulib module sigprocmask should be tested. */
#define GNULIB_TEST_SIGPROCMASK 1

/* Define to 1 when the gnulib module snprintf should be tested. */
#define GNULIB_TEST_SNPRINTF 1

/* Define to 1 when the gnulib module stat should be tested. */
#define GNULIB_TEST_STAT 1

/* Define to 1 when the gnulib module strcasestr should be tested. */
#define GNULIB_TEST_STRCASESTR 1

/* Define to 1 when the gnulib module strnlen should be tested. */
#define GNULIB_TEST_STRNLEN 1

/* Define to 1 when the gnulib module utime should be tested. */
#define GNULIB_TEST_UTIME 1

/* Define to 1 when the gnulib module vsnprintf should be tested. */
#define GNULIB_TEST_VSNPRINTF 1

/* Define to 1 when the gnulib module wcrtomb should be tested. */
#define GNULIB_TEST_WCRTOMB 1

/* Define to 1 when the gnulib module wcwidth should be tested. */
#define GNULIB_TEST_WCWIDTH 1

/* Define this if the system supports GNU-style word boundaries in regexes. */
#define GNU_WORDBOUNDS 1

/* Define to 1 if you have 'alloca' after including <alloca.h>, a header that
   may be supplied by this distribution. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the <bp-sym.h> header file. */
/* #undef HAVE_BP_SYM_H */

/* Define to 1 if you have the `btowc' function. */
#define HAVE_BTOWC 1

/* Define to 1 if you have the Mac OS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
/* #undef HAVE_CFLOCALECOPYCURRENT */

/* Define to 1 if you have the Mac OS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
/* #undef HAVE_CFPREFERENCESCOPYAPPVALUE */

/* Define to 1 if you have the `clock_gettime' function. */
#define HAVE_CLOCK_GETTIME 1

/* Define to 1 if you have the `clock_settime' function. */
#define HAVE_CLOCK_SETTIME 1

/* Define to 1 if you have the `closedir' function. */
#define HAVE_CLOSEDIR 1

/* Define if the copysignf function is declared in <math.h> and available in
   libc. */
/* #undef HAVE_COPYSIGNF_IN_LIBC */

/* Define if the copysignl function is declared in <math.h> and available in
   libc. */
/* #undef HAVE_COPYSIGNL_IN_LIBC */

/* Define if the copysign function is declared in <math.h> and available in
   libc. */
/* #undef HAVE_COPYSIGN_IN_LIBC */

/* Define to 1 if you have the <crtdefs.h> header file. */
/* #undef HAVE_CRTDEFS_H */

/* Define to 1 if you have the <curses.h> header file. */
/* #undef HAVE_CURSES_H */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
/* #undef HAVE_DCGETTEXT */

/* Define to 1 if you have the declaration of `alarm', and to 0 if you don't.
   */
#define HAVE_DECL_ALARM 1

/* Define to 1 if you have the declaration of `copysign', and to 0 if you
   don't. */
/* #undef HAVE_DECL_COPYSIGN */

/* Define to 1 if you have the declaration of `copysignf', and to 0 if you
   don't. */
/* #undef HAVE_DECL_COPYSIGNF */

/* Define to 1 if you have the declaration of `copysignl', and to 0 if you
   don't. */
/* #undef HAVE_DECL_COPYSIGNL */

/* Define to 1 if you have the declaration of `dirfd', and to 0 if you don't.
   */
#define HAVE_DECL_DIRFD 1

/* Define to 1 if you have the declaration of `getc_unlocked', and to 0 if you
   don't. */
#define HAVE_DECL_GETC_UNLOCKED 1

/* Define to 1 if you have the declaration of `getdelim', and to 0 if you
   don't. */
#define HAVE_DECL_GETDELIM 1

/* Define to 1 if you have the declaration of `getline', and to 0 if you
   don't. */
#define HAVE_DECL_GETLINE 1

/* Define to 1 if you have the declaration of `getlogin', and to 0 if you
   don't. */
#define HAVE_DECL_GETLOGIN 1

/* Define to 1 if you have the declaration of `getlogin_r', and to 0 if you
   don't. */
#define HAVE_DECL_GETLOGIN_R 1

/* Define to 1 if you have the declaration of `isblank', and to 0 if you
   don't. */
/* #undef HAVE_DECL_ISBLANK */

/* Define to 1 if you have the declaration of `iswblank', and to 0 if you
   don't. */
#define HAVE_DECL_ISWBLANK 1

/* Define to 1 if you have the declaration of `mbrtowc', and to 0 if you
   don't. */
/* #undef HAVE_DECL_MBRTOWC */

/* Define to 1 if you have the declaration of `mbsinit', and to 0 if you
   don't. */
/* #undef HAVE_DECL_MBSINIT */

/* Define to 1 if you have the declaration of `mbsrtowcs', and to 0 if you
   don't. */
/* #undef HAVE_DECL_MBSRTOWCS */

/* Define to 1 if you have the declaration of `snprintf', and to 0 if you
   don't. */
#define HAVE_DECL_SNPRINTF 1

/* Define to 1 if you have the declaration of `strncasecmp', and to 0 if you
   don't. */
#define HAVE_DECL_STRNCASECMP 1

/* Define to 1 if you have the declaration of `strnlen', and to 0 if you
   don't. */
#define HAVE_DECL_STRNLEN 1

/* Define to 1 if you have the declaration of `towlower', and to 0 if you
   don't. */
/* #undef HAVE_DECL_TOWLOWER */

/* Define to 1 if you have the declaration of `vsnprintf', and to 0 if you
   don't. */
#define HAVE_DECL_VSNPRINTF 1

/* Define to 1 if you have the declaration of `wcrtomb', and to 0 if you
   don't. */
/* #undef HAVE_DECL_WCRTOMB */

/* Define to 1 if you have the declaration of `wcwidth', and to 0 if you
   don't. */
#define HAVE_DECL_WCWIDTH 1

/* Define to 1 if you have the declaration of `_snprintf', and to 0 if you
   don't. */
#define HAVE_DECL__SNPRINTF 0

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the `dirfd' function. */
#define HAVE_DIRFD 1

/* Define to 1 if you have the <features.h> header file. */
#define HAVE_FEATURES_H 1

/* Define to 1 if you have the `flockfile' function. */
/* #undef HAVE_FLOCKFILE */

/* Define if the frexpl function is available in libc. */
#define HAVE_FREXPL_IN_LIBC 1

/* Define if the frexp function is available in libc. */
#define HAVE_FREXP_IN_LIBC 1

/* Define to 1 if you have the `funlockfile' function. */
/* #undef HAVE_FUNLOCKFILE */

/* Define to 1 if you have the `futimens' function. */
#define HAVE_FUTIMENS 1

/* Define to 1 if you have the `futimes' function. */
#define HAVE_FUTIMES 1

/* Define to 1 if you have the `futimesat' function. */
#define HAVE_FUTIMESAT 1

/* Define to 1 if you have the `getdelim' function. */
#define HAVE_GETDELIM 1

/* Define to 1 if you have the `getlogin_r' function. */
#define HAVE_GETLOGIN_R 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getopt_long_only' function. */
#define HAVE_GETOPT_LONG_ONLY 1

/* Define to 1 if you have the `getpwnam_r' function. */
#define HAVE_GETPWNAM_R 1

/* Define if the GNU gettext() function is already present or preinstalled. */
/* #undef HAVE_GETTEXT */

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <glob.h> header file. */
#define HAVE_GLOB_H 1

/* Define if you have the iconv() function and it works. */
/* #undef HAVE_ICONV */

/* Define if you have the 'intmax_t' type in <stdint.h> or <inttypes.h>. */
#define HAVE_INTMAX_T 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if <inttypes.h> exists, doesn't clash with <sys/types.h>, and
   declares uintmax_t. */
#define HAVE_INTTYPES_H_WITH_UINTMAX 1

/* Define to 1 if you have the `isblank' function. */
#define HAVE_ISBLANK 1

/* Define if the isnan(double) function is available in libc. */
#define HAVE_ISNAND_IN_LIBC 1

/* Define if the isnan(float) function is available in libc. */
#define HAVE_ISNANF_IN_LIBC 1

/* Define if the isnan(long double) function is available in libc. */
#define HAVE_ISNANL_IN_LIBC 1

/* Define to 1 if you have the `iswalnum' function. */
/* #undef HAVE_ISWALNUM */

/* Define to 1 if you have the `iswblank' function. */
#define HAVE_ISWBLANK 1

/* Define to 1 if you have the `iswcntrl' function. */
#define HAVE_ISWCNTRL 1

/* Define to 1 if you have the `iswctype' function. */
#define HAVE_ISWCTYPE 1

/* Define to 1 if you have the `iswpunct' function. */
/* #undef HAVE_ISWPUNCT */

/* Define this if your curses library has the key_defined() function. */
#define HAVE_KEY_DEFINED 1

/* Define if you have <langinfo.h> and nl_langinfo(CODESET). */
#define HAVE_LANGINFO_CODESET 1

/* Define to 1 if you have the <langinfo.h> header file. */
#define HAVE_LANGINFO_H 1

/* Define if the ldexpl function is available in libc. */
#define HAVE_LDEXPL_IN_LIBC 1

/* Define if the ldexp function is available in libc. */
#define HAVE_LDEXP_IN_LIBC 1

/* Define to 1 if you have the <libintl.h> header file. */
#define HAVE_LIBINTL_H 1

/* Define to 1 if you have the `magic' library (-lmagic). */
/* #undef HAVE_LIBMAGIC */

/* Define to 1 if you have the `z' library (-lz). */
/* #undef HAVE_LIBZ */

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if the system has the type 'long long int'. */
#define HAVE_LONG_LONG_INT 1

/* Define to 1 if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define to 1 if you have the `lutimes' function. */
#define HAVE_LUTIMES 1

/* Define to 1 if you have the <magic.h> header file. */
/* #undef HAVE_MAGIC_H */

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define if the 'malloc' function is POSIX compliant. */
#define HAVE_MALLOC_POSIX 1

/* Define to 1 if mmap()'s MAP_ANONYMOUS flag is available after including
   config.h and <sys/mman.h>. */
#define HAVE_MAP_ANONYMOUS 1

/* Define to 1 if you have the <math.h> header file. */
#define HAVE_MATH_H 1

/* Define to 1 if you have the `mblen' function. */
/* #undef HAVE_MBLEN */

/* Define to 1 if you have the `mbrtowc' function. */
#define HAVE_MBRTOWC 1

/* Define to 1 if you have the `mbsinit' function. */
#define HAVE_MBSINIT 1

/* Define to 1 if you have the `mbsrtowcs' function. */
#define HAVE_MBSRTOWCS 1

/* Define to 1 if <wchar.h> declares mbstate_t. */
#define HAVE_MBSTATE_T 1

/* Define to 1 if you have the `mbstowcs' function. */
/* #undef HAVE_MBSTOWCS */

/* Define to 1 if you have the `mbtowc' function. */
/* #undef HAVE_MBTOWC */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `mempcpy' function. */
#define HAVE_MEMPCPY 1

/* Define to 1 if you have the `mprotect' function. */
#define HAVE_MPROTECT 1

/* Define to 1 on MSVC platforms that have the "invalid parameter handler"
   concept. */
/* #undef HAVE_MSVC_INVALID_PARAMETER_HANDLER */

/* Define to 1 if you have the `nanotime' function. */
/* #undef HAVE_NANOTIME */

/* Define to 1 if you have the <ncurses.h> header file. */
/* #undef HAVE_NCURSES_H */

/* Define to 1 if you have the `nl_langinfo' function. */
#define HAVE_NL_LANGINFO 1

/* Define to 1 if you have the `opendir' function. */
#define HAVE_OPENDIR 1

/* Define if the <pthread.h> defines PTHREAD_MUTEX_RECURSIVE. */
#define HAVE_PTHREAD_MUTEX_RECURSIVE 1

/* Define if the POSIX multithreading library has read/write locks. */
#define HAVE_PTHREAD_RWLOCK 1

/* Define if the 'pthread_rwlock_rdlock' function prefers a writer to a
   reader. */
/* #undef HAVE_PTHREAD_RWLOCK_RDLOCK_PREFER_WRITER */

/* Define to 1 if you have the <pwd.h> header file. */
#define HAVE_PWD_H 1

/* Define to 1 if you have the `raise' function. */
#define HAVE_RAISE 1

/* Define to 1 if acosf is declared even after undefining macros. */
#define HAVE_RAW_DECL_ACOSF 1

/* Define to 1 if acosl is declared even after undefining macros. */
#define HAVE_RAW_DECL_ACOSL 1

/* Define to 1 if alphasort is declared even after undefining macros. */
#define HAVE_RAW_DECL_ALPHASORT 1

/* Define to 1 if asinf is declared even after undefining macros. */
#define HAVE_RAW_DECL_ASINF 1

/* Define to 1 if asinl is declared even after undefining macros. */
#define HAVE_RAW_DECL_ASINL 1

/* Define to 1 if atanf is declared even after undefining macros. */
#define HAVE_RAW_DECL_ATANF 1

/* Define to 1 if atanl is declared even after undefining macros. */
#define HAVE_RAW_DECL_ATANL 1

/* Define to 1 if atoll is declared even after undefining macros. */
#define HAVE_RAW_DECL_ATOLL 1

/* Define to 1 if btowc is declared even after undefining macros. */
#define HAVE_RAW_DECL_BTOWC 1

/* Define to 1 if canonicalize_file_name is declared even after undefining
   macros. */
#define HAVE_RAW_DECL_CANONICALIZE_FILE_NAME 1

/* Define to 1 if cbrt is declared even after undefining macros. */
#define HAVE_RAW_DECL_CBRT 1

/* Define to 1 if cbrtf is declared even after undefining macros. */
#define HAVE_RAW_DECL_CBRTF 1

/* Define to 1 if cbrtl is declared even after undefining macros. */
#define HAVE_RAW_DECL_CBRTL 1

/* Define to 1 if ceilf is declared even after undefining macros. */
#define HAVE_RAW_DECL_CEILF 1

/* Define to 1 if ceill is declared even after undefining macros. */
#define HAVE_RAW_DECL_CEILL 1

/* Define to 1 if chdir is declared even after undefining macros. */
#define HAVE_RAW_DECL_CHDIR 1

/* Define to 1 if chown is declared even after undefining macros. */
#define HAVE_RAW_DECL_CHOWN 1

/* Define to 1 if closedir is declared even after undefining macros. */
#define HAVE_RAW_DECL_CLOSEDIR 1

/* Define to 1 if copysign is declared even after undefining macros. */
#define HAVE_RAW_DECL_COPYSIGN 1

/* Define to 1 if copysignf is declared even after undefining macros. */
#define HAVE_RAW_DECL_COPYSIGNF 1

/* Define to 1 if copysignl is declared even after undefining macros. */
#define HAVE_RAW_DECL_COPYSIGNL 1

/* Define to 1 if cosf is declared even after undefining macros. */
#define HAVE_RAW_DECL_COSF 1

/* Define to 1 if coshf is declared even after undefining macros. */
#define HAVE_RAW_DECL_COSHF 1

/* Define to 1 if cosl is declared even after undefining macros. */
#define HAVE_RAW_DECL_COSL 1

/* Define to 1 if dirfd is declared even after undefining macros. */
#define HAVE_RAW_DECL_DIRFD 1

/* Define to 1 if dprintf is declared even after undefining macros. */
#define HAVE_RAW_DECL_DPRINTF 1

/* Define to 1 if dup is declared even after undefining macros. */
#define HAVE_RAW_DECL_DUP 1

/* Define to 1 if dup2 is declared even after undefining macros. */
#define HAVE_RAW_DECL_DUP2 1

/* Define to 1 if dup3 is declared even after undefining macros. */
#define HAVE_RAW_DECL_DUP3 1

/* Define to 1 if duplocale is declared even after undefining macros. */
#define HAVE_RAW_DECL_DUPLOCALE 1

/* Define to 1 if endusershell is declared even after undefining macros. */
#define HAVE_RAW_DECL_ENDUSERSHELL 1

/* Define to 1 if environ is declared even after undefining macros. */
#define HAVE_RAW_DECL_ENVIRON 1

/* Define to 1 if euidaccess is declared even after undefining macros. */
#define HAVE_RAW_DECL_EUIDACCESS 1

/* Define to 1 if exp2 is declared even after undefining macros. */
#define HAVE_RAW_DECL_EXP2 1

/* Define to 1 if exp2f is declared even after undefining macros. */
#define HAVE_RAW_DECL_EXP2F 1

/* Define to 1 if exp2l is declared even after undefining macros. */
#define HAVE_RAW_DECL_EXP2L 1

/* Define to 1 if expf is declared even after undefining macros. */
#define HAVE_RAW_DECL_EXPF 1

/* Define to 1 if expl is declared even after undefining macros. */
#define HAVE_RAW_DECL_EXPL 1

/* Define to 1 if expm1 is declared even after undefining macros. */
#define HAVE_RAW_DECL_EXPM1 1

/* Define to 1 if expm1f is declared even after undefining macros. */
#define HAVE_RAW_DECL_EXPM1F 1

/* Define to 1 if expm1l is declared even after undefining macros. */
#define HAVE_RAW_DECL_EXPM1L 1

/* Define to 1 if fabsf is declared even after undefining macros. */
#define HAVE_RAW_DECL_FABSF 1

/* Define to 1 if fabsl is declared even after undefining macros. */
#define HAVE_RAW_DECL_FABSL 1

/* Define to 1 if faccessat is declared even after undefining macros. */
#define HAVE_RAW_DECL_FACCESSAT 1

/* Define to 1 if fchdir is declared even after undefining macros. */
#define HAVE_RAW_DECL_FCHDIR 1

/* Define to 1 if fchmodat is declared even after undefining macros. */
#define HAVE_RAW_DECL_FCHMODAT 1

/* Define to 1 if fchownat is declared even after undefining macros. */
#define HAVE_RAW_DECL_FCHOWNAT 1

/* Define to 1 if fcntl is declared even after undefining macros. */
#define HAVE_RAW_DECL_FCNTL 1

/* Define to 1 if fdatasync is declared even after undefining macros. */
#define HAVE_RAW_DECL_FDATASYNC 1

/* Define to 1 if fdopendir is declared even after undefining macros. */
#define HAVE_RAW_DECL_FDOPENDIR 1

/* Define to 1 if ffs is declared even after undefining macros. */
#define HAVE_RAW_DECL_FFS 1

/* Define to 1 if ffsl is declared even after undefining macros. */
#define HAVE_RAW_DECL_FFSL 1

/* Define to 1 if ffsll is declared even after undefining macros. */
#define HAVE_RAW_DECL_FFSLL 1

/* Define to 1 if floorf is declared even after undefining macros. */
#define HAVE_RAW_DECL_FLOORF 1

/* Define to 1 if floorl is declared even after undefining macros. */
#define HAVE_RAW_DECL_FLOORL 1

/* Define to 1 if fma is declared even after undefining macros. */
#define HAVE_RAW_DECL_FMA 1

/* Define to 1 if fmaf is declared even after undefining macros. */
#define HAVE_RAW_DECL_FMAF 1

/* Define to 1 if fmal is declared even after undefining macros. */
#define HAVE_RAW_DECL_FMAL 1

/* Define to 1 if fmod is declared even after undefining macros. */
#define HAVE_RAW_DECL_FMOD 1

/* Define to 1 if fmodf is declared even after undefining macros. */
#define HAVE_RAW_DECL_FMODF 1

/* Define to 1 if fmodl is declared even after undefining macros. */
#define HAVE_RAW_DECL_FMODL 1

/* Define to 1 if fpurge is declared even after undefining macros. */
/* #undef HAVE_RAW_DECL_FPURGE */

/* Define to 1 if frexpf is declared even after undefining macros. */
#define HAVE_RAW_DECL_FREXPF 1

/* Define to 1 if frexpl is declared even after undefining macros. */
#define HAVE_RAW_DECL_FREXPL 1

/* Define to 1 if fseeko is declared even after undefining macros. */
#define HAVE_RAW_DECL_FSEEKO 1

/* Define to 1 if fstat is declared even after undefining macros. */
#define HAVE_RAW_DECL_FSTAT 1

/* Define to 1 if fstatat is declared even after undefining macros. */
#define HAVE_RAW_DECL_FSTATAT 1

/* Define to 1 if fsync is declared even after undefining macros. */
#define HAVE_RAW_DECL_FSYNC 1

/* Define to 1 if ftello is declared even after undefining macros. */
#define HAVE_RAW_DECL_FTELLO 1

/* Define to 1 if ftruncate is declared even after undefining macros. */
#define HAVE_RAW_DECL_FTRUNCATE 1

/* Define to 1 if futimens is declared even after undefining macros. */
#define HAVE_RAW_DECL_FUTIMENS 1

/* Define to 1 if getcwd is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETCWD 1

/* Define to 1 if getdelim is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETDELIM 1

/* Define to 1 if getdomainname is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETDOMAINNAME 1

/* Define to 1 if getdtablesize is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETDTABLESIZE 1

/* Define to 1 if getgroups is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETGROUPS 1

/* Define to 1 if gethostname is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETHOSTNAME 1

/* Define to 1 if getline is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETLINE 1

/* Define to 1 if getloadavg is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETLOADAVG 1

/* Define to 1 if getlogin is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETLOGIN 1

/* Define to 1 if getlogin_r is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETLOGIN_R 1

/* Define to 1 if getpagesize is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETPAGESIZE 1

/* Define to 1 if gets is declared even after undefining macros. */
/* #undef HAVE_RAW_DECL_GETS */

/* Define to 1 if getsubopt is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETSUBOPT 1

/* Define to 1 if gettimeofday is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETTIMEOFDAY 1

/* Define to 1 if getusershell is declared even after undefining macros. */
#define HAVE_RAW_DECL_GETUSERSHELL 1

/* Define to 1 if grantpt is declared even after undefining macros. */
#define HAVE_RAW_DECL_GRANTPT 1

/* Define to 1 if group_member is declared even after undefining macros. */
#define HAVE_RAW_DECL_GROUP_MEMBER 1

/* Define to 1 if hypotf is declared even after undefining macros. */
#define HAVE_RAW_DECL_HYPOTF 1

/* Define to 1 if hypotl is declared even after undefining macros. */
#define HAVE_RAW_DECL_HYPOTL 1

/* Define to 1 if ilogb is declared even after undefining macros. */
#define HAVE_RAW_DECL_ILOGB 1

/* Define to 1 if ilogbf is declared even after undefining macros. */
#define HAVE_RAW_DECL_ILOGBF 1

/* Define to 1 if ilogbl is declared even after undefining macros. */
#define HAVE_RAW_DECL_ILOGBL 1

/* Define to 1 if initstate is declared even after undefining macros. */
#define HAVE_RAW_DECL_INITSTATE 1

/* Define to 1 if initstate_r is declared even after undefining macros. */
#define HAVE_RAW_DECL_INITSTATE_R 1

/* Define to 1 if isatty is declared even after undefining macros. */
#define HAVE_RAW_DECL_ISATTY 1

/* Define to 1 if isblank is declared even after undefining macros. */
#define HAVE_RAW_DECL_ISBLANK 1

/* Define to 1 if iswctype is declared even after undefining macros. */
#define HAVE_RAW_DECL_ISWCTYPE 1

/* Define to 1 if lchmod is declared even after undefining macros. */
#define HAVE_RAW_DECL_LCHMOD 1

/* Define to 1 if lchown is declared even after undefining macros. */
#define HAVE_RAW_DECL_LCHOWN 1

/* Define to 1 if ldexpf is declared even after undefining macros. */
#define HAVE_RAW_DECL_LDEXPF 1

/* Define to 1 if ldexpl is declared even after undefining macros. */
#define HAVE_RAW_DECL_LDEXPL 1

/* Define to 1 if link is declared even after undefining macros. */
#define HAVE_RAW_DECL_LINK 1

/* Define to 1 if linkat is declared even after undefining macros. */
#define HAVE_RAW_DECL_LINKAT 1

/* Define to 1 if log is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOG 1

/* Define to 1 if log10 is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOG10 1

/* Define to 1 if log10f is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOG10F 1

/* Define to 1 if log10l is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOG10L 1

/* Define to 1 if log1p is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOG1P 1

/* Define to 1 if log1pf is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOG1PF 1

/* Define to 1 if log1pl is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOG1PL 1

/* Define to 1 if log2 is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOG2 1

/* Define to 1 if log2f is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOG2F 1

/* Define to 1 if log2l is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOG2L 1

/* Define to 1 if logb is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOGB 1

/* Define to 1 if logbf is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOGBF 1

/* Define to 1 if logbl is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOGBL 1

/* Define to 1 if logf is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOGF 1

/* Define to 1 if logl is declared even after undefining macros. */
#define HAVE_RAW_DECL_LOGL 1

/* Define to 1 if lseek is declared even after undefining macros. */
#define HAVE_RAW_DECL_LSEEK 1

/* Define to 1 if lstat is declared even after undefining macros. */
#define HAVE_RAW_DECL_LSTAT 1

/* Define to 1 if mbrlen is declared even after undefining macros. */
#define HAVE_RAW_DECL_MBRLEN 1

/* Define to 1 if mbrtowc is declared even after undefining macros. */
#define HAVE_RAW_DECL_MBRTOWC 1

/* Define to 1 if mbsinit is declared even after undefining macros. */
#define HAVE_RAW_DECL_MBSINIT 1

/* Define to 1 if mbsnrtowcs is declared even after undefining macros. */
#define HAVE_RAW_DECL_MBSNRTOWCS 1

/* Define to 1 if mbsrtowcs is declared even after undefining macros. */
#define HAVE_RAW_DECL_MBSRTOWCS 1

/* Define to 1 if memmem is declared even after undefining macros. */
#define HAVE_RAW_DECL_MEMMEM 1

/* Define to 1 if mempcpy is declared even after undefining macros. */
#define HAVE_RAW_DECL_MEMPCPY 1

/* Define to 1 if memrchr is declared even after undefining macros. */
#define HAVE_RAW_DECL_MEMRCHR 1

/* Define to 1 if mkdirat is declared even after undefining macros. */
#define HAVE_RAW_DECL_MKDIRAT 1

/* Define to 1 if mkdtemp is declared even after undefining macros. */
#define HAVE_RAW_DECL_MKDTEMP 1

/* Define to 1 if mkfifo is declared even after undefining macros. */
#define HAVE_RAW_DECL_MKFIFO 1

/* Define to 1 if mkfifoat is declared even after undefining macros. */
#define HAVE_RAW_DECL_MKFIFOAT 1

/* Define to 1 if mknod is declared even after undefining macros. */
#define HAVE_RAW_DECL_MKNOD 1

/* Define to 1 if mknodat is declared even after undefining macros. */
#define HAVE_RAW_DECL_MKNODAT 1

/* Define to 1 if mkostemp is declared even after undefining macros. */
#define HAVE_RAW_DECL_MKOSTEMP 1

/* Define to 1 if mkostemps is declared even after undefining macros. */
#define HAVE_RAW_DECL_MKOSTEMPS 1

/* Define to 1 if mkstemp is declared even after undefining macros. */
#define HAVE_RAW_DECL_MKSTEMP 1

/* Define to 1 if mkstemps is declared even after undefining macros. */
#define HAVE_RAW_DECL_MKSTEMPS 1

/* Define to 1 if modf is declared even after undefining macros. */
#define HAVE_RAW_DECL_MODF 1

/* Define to 1 if modff is declared even after undefining macros. */
#define HAVE_RAW_DECL_MODFF 1

/* Define to 1 if modfl is declared even after undefining macros. */
#define HAVE_RAW_DECL_MODFL 1

/* Define to 1 if nl_langinfo is declared even after undefining macros. */
#define HAVE_RAW_DECL_NL_LANGINFO 1

/* Define to 1 if openat is declared even after undefining macros. */
#define HAVE_RAW_DECL_OPENAT 1

/* Define to 1 if opendir is declared even after undefining macros. */
#define HAVE_RAW_DECL_OPENDIR 1

/* Define to 1 if pclose is declared even after undefining macros. */
#define HAVE_RAW_DECL_PCLOSE 1

/* Define to 1 if pipe is declared even after undefining macros. */
#define HAVE_RAW_DECL_PIPE 1

/* Define to 1 if pipe2 is declared even after undefining macros. */
#define HAVE_RAW_DECL_PIPE2 1

/* Define to 1 if popen is declared even after undefining macros. */
#define HAVE_RAW_DECL_POPEN 1

/* Define to 1 if posix_openpt is declared even after undefining macros. */
#define HAVE_RAW_DECL_POSIX_OPENPT 1

/* Define to 1 if powf is declared even after undefining macros. */
#define HAVE_RAW_DECL_POWF 1

/* Define to 1 if pread is declared even after undefining macros. */
#define HAVE_RAW_DECL_PREAD 1

/* Define to 1 if pthread_sigmask is declared even after undefining macros. */
#define HAVE_RAW_DECL_PTHREAD_SIGMASK 1

/* Define to 1 if ptsname is declared even after undefining macros. */
#define HAVE_RAW_DECL_PTSNAME 1

/* Define to 1 if ptsname_r is declared even after undefining macros. */
#define HAVE_RAW_DECL_PTSNAME_R 1

/* Define to 1 if pwrite is declared even after undefining macros. */
#define HAVE_RAW_DECL_PWRITE 1

/* Define to 1 if qsort_r is declared even after undefining macros. */
#define HAVE_RAW_DECL_QSORT_R 1

/* Define to 1 if random is declared even after undefining macros. */
#define HAVE_RAW_DECL_RANDOM 1

/* Define to 1 if random_r is declared even after undefining macros. */
#define HAVE_RAW_DECL_RANDOM_R 1

/* Define to 1 if rawmemchr is declared even after undefining macros. */
#define HAVE_RAW_DECL_RAWMEMCHR 1

/* Define to 1 if readdir is declared even after undefining macros. */
#define HAVE_RAW_DECL_READDIR 1

/* Define to 1 if readlink is declared even after undefining macros. */
#define HAVE_RAW_DECL_READLINK 1

/* Define to 1 if readlinkat is declared even after undefining macros. */
#define HAVE_RAW_DECL_READLINKAT 1

/* Define to 1 if reallocarray is declared even after undefining macros. */
/* #undef HAVE_RAW_DECL_REALLOCARRAY */

/* Define to 1 if realpath is declared even after undefining macros. */
#define HAVE_RAW_DECL_REALPATH 1

/* Define to 1 if remainder is declared even after undefining macros. */
#define HAVE_RAW_DECL_REMAINDER 1

/* Define to 1 if remainderf is declared even after undefining macros. */
#define HAVE_RAW_DECL_REMAINDERF 1

/* Define to 1 if remainderl is declared even after undefining macros. */
#define HAVE_RAW_DECL_REMAINDERL 1

/* Define to 1 if renameat is declared even after undefining macros. */
#define HAVE_RAW_DECL_RENAMEAT 1

/* Define to 1 if rewinddir is declared even after undefining macros. */
#define HAVE_RAW_DECL_REWINDDIR 1

/* Define to 1 if rint is declared even after undefining macros. */
#define HAVE_RAW_DECL_RINT 1

/* Define to 1 if rintf is declared even after undefining macros. */
#define HAVE_RAW_DECL_RINTF 1

/* Define to 1 if rintl is declared even after undefining macros. */
#define HAVE_RAW_DECL_RINTL 1

/* Define to 1 if rmdir is declared even after undefining macros. */
#define HAVE_RAW_DECL_RMDIR 1

/* Define to 1 if round is declared even after undefining macros. */
#define HAVE_RAW_DECL_ROUND 1

/* Define to 1 if roundf is declared even after undefining macros. */
#define HAVE_RAW_DECL_ROUNDF 1

/* Define to 1 if roundl is declared even after undefining macros. */
#define HAVE_RAW_DECL_ROUNDL 1

/* Define to 1 if rpmatch is declared even after undefining macros. */
#define HAVE_RAW_DECL_RPMATCH 1

/* Define to 1 if scandir is declared even after undefining macros. */
#define HAVE_RAW_DECL_SCANDIR 1

/* Define to 1 if secure_getenv is declared even after undefining macros. */
#define HAVE_RAW_DECL_SECURE_GETENV 1

/* Define to 1 if setenv is declared even after undefining macros. */
#define HAVE_RAW_DECL_SETENV 1

/* Define to 1 if sethostname is declared even after undefining macros. */
#define HAVE_RAW_DECL_SETHOSTNAME 1

/* Define to 1 if setlocale is declared even after undefining macros. */
#define HAVE_RAW_DECL_SETLOCALE 1

/* Define to 1 if setstate is declared even after undefining macros. */
#define HAVE_RAW_DECL_SETSTATE 1

/* Define to 1 if setstate_r is declared even after undefining macros. */
#define HAVE_RAW_DECL_SETSTATE_R 1

/* Define to 1 if setusershell is declared even after undefining macros. */
#define HAVE_RAW_DECL_SETUSERSHELL 1

/* Define to 1 if sigaction is declared even after undefining macros. */
#define HAVE_RAW_DECL_SIGACTION 1

/* Define to 1 if sigaddset is declared even after undefining macros. */
#define HAVE_RAW_DECL_SIGADDSET 1

/* Define to 1 if sigdelset is declared even after undefining macros. */
#define HAVE_RAW_DECL_SIGDELSET 1

/* Define to 1 if sigemptyset is declared even after undefining macros. */
#define HAVE_RAW_DECL_SIGEMPTYSET 1

/* Define to 1 if sigfillset is declared even after undefining macros. */
#define HAVE_RAW_DECL_SIGFILLSET 1

/* Define to 1 if sigismember is declared even after undefining macros. */
#define HAVE_RAW_DECL_SIGISMEMBER 1

/* Define to 1 if sigpending is declared even after undefining macros. */
#define HAVE_RAW_DECL_SIGPENDING 1

/* Define to 1 if sigprocmask is declared even after undefining macros. */
#define HAVE_RAW_DECL_SIGPROCMASK 1

/* Define to 1 if sinf is declared even after undefining macros. */
#define HAVE_RAW_DECL_SINF 1

/* Define to 1 if sinhf is declared even after undefining macros. */
#define HAVE_RAW_DECL_SINHF 1

/* Define to 1 if sinl is declared even after undefining macros. */
#define HAVE_RAW_DECL_SINL 1

/* Define to 1 if sleep is declared even after undefining macros. */
#define HAVE_RAW_DECL_SLEEP 1

/* Define to 1 if snprintf is declared even after undefining macros. */
#define HAVE_RAW_DECL_SNPRINTF 1

/* Define to 1 if sqrtf is declared even after undefining macros. */
#define HAVE_RAW_DECL_SQRTF 1

/* Define to 1 if sqrtl is declared even after undefining macros. */
#define HAVE_RAW_DECL_SQRTL 1

/* Define to 1 if srandom is declared even after undefining macros. */
#define HAVE_RAW_DECL_SRANDOM 1

/* Define to 1 if srandom_r is declared even after undefining macros. */
#define HAVE_RAW_DECL_SRANDOM_R 1

/* Define to 1 if stat is declared even after undefining macros. */
#define HAVE_RAW_DECL_STAT 1

/* Define to 1 if stpcpy is declared even after undefining macros. */
#define HAVE_RAW_DECL_STPCPY 1

/* Define to 1 if stpncpy is declared even after undefining macros. */
#define HAVE_RAW_DECL_STPNCPY 1

/* Define to 1 if strcasecmp is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRCASECMP 1

/* Define to 1 if strcasestr is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRCASESTR 1

/* Define to 1 if strchrnul is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRCHRNUL 1

/* Define to 1 if strdup is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRDUP 1

/* Define to 1 if strerror_r is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRERROR_R 1

/* Define to 1 if strncasecmp is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRNCASECMP 1

/* Define to 1 if strncat is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRNCAT 1

/* Define to 1 if strndup is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRNDUP 1

/* Define to 1 if strnlen is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRNLEN 1

/* Define to 1 if strpbrk is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRPBRK 1

/* Define to 1 if strsep is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRSEP 1

/* Define to 1 if strsignal is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRSIGNAL 1

/* Define to 1 if strtod is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRTOD 1

/* Define to 1 if strtok_r is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRTOK_R 1

/* Define to 1 if strtoll is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRTOLL 1

/* Define to 1 if strtoull is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRTOULL 1

/* Define to 1 if strverscmp is declared even after undefining macros. */
#define HAVE_RAW_DECL_STRVERSCMP 1

/* Define to 1 if symlink is declared even after undefining macros. */
#define HAVE_RAW_DECL_SYMLINK 1

/* Define to 1 if symlinkat is declared even after undefining macros. */
#define HAVE_RAW_DECL_SYMLINKAT 1

/* Define to 1 if tanf is declared even after undefining macros. */
#define HAVE_RAW_DECL_TANF 1

/* Define to 1 if tanhf is declared even after undefining macros. */
#define HAVE_RAW_DECL_TANHF 1

/* Define to 1 if tanl is declared even after undefining macros. */
#define HAVE_RAW_DECL_TANL 1

/* Define to 1 if tmpfile is declared even after undefining macros. */
#define HAVE_RAW_DECL_TMPFILE 1

/* Define to 1 if towctrans is declared even after undefining macros. */
#define HAVE_RAW_DECL_TOWCTRANS 1

/* Define to 1 if trunc is declared even after undefining macros. */
#define HAVE_RAW_DECL_TRUNC 1

/* Define to 1 if truncate is declared even after undefining macros. */
#define HAVE_RAW_DECL_TRUNCATE 1

/* Define to 1 if truncf is declared even after undefining macros. */
#define HAVE_RAW_DECL_TRUNCF 1

/* Define to 1 if truncl is declared even after undefining macros. */
#define HAVE_RAW_DECL_TRUNCL 1

/* Define to 1 if ttyname_r is declared even after undefining macros. */
#define HAVE_RAW_DECL_TTYNAME_R 1

/* Define to 1 if unlink is declared even after undefining macros. */
#define HAVE_RAW_DECL_UNLINK 1

/* Define to 1 if unlinkat is declared even after undefining macros. */
#define HAVE_RAW_DECL_UNLINKAT 1

/* Define to 1 if unlockpt is declared even after undefining macros. */
#define HAVE_RAW_DECL_UNLOCKPT 1

/* Define to 1 if unsetenv is declared even after undefining macros. */
#define HAVE_RAW_DECL_UNSETENV 1

/* Define to 1 if usleep is declared even after undefining macros. */
#define HAVE_RAW_DECL_USLEEP 1

/* Define to 1 if utime is declared even after undefining macros. */
#define HAVE_RAW_DECL_UTIME 1

/* Define to 1 if utimensat is declared even after undefining macros. */
#define HAVE_RAW_DECL_UTIMENSAT 1

/* Define to 1 if vdprintf is declared even after undefining macros. */
#define HAVE_RAW_DECL_VDPRINTF 1

/* Define to 1 if vsnprintf is declared even after undefining macros. */
#define HAVE_RAW_DECL_VSNPRINTF 1

/* Define to 1 if waitpid is declared even after undefining macros. */
#define HAVE_RAW_DECL_WAITPID 1

/* Define to 1 if wcpcpy is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCPCPY 1

/* Define to 1 if wcpncpy is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCPNCPY 1

/* Define to 1 if wcrtomb is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCRTOMB 1

/* Define to 1 if wcscasecmp is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSCASECMP 1

/* Define to 1 if wcscat is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSCAT 1

/* Define to 1 if wcschr is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSCHR 1

/* Define to 1 if wcscmp is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSCMP 1

/* Define to 1 if wcscoll is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSCOLL 1

/* Define to 1 if wcscpy is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSCPY 1

/* Define to 1 if wcscspn is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSCSPN 1

/* Define to 1 if wcsdup is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSDUP 1

/* Define to 1 if wcsftime is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSFTIME 1

/* Define to 1 if wcslen is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSLEN 1

/* Define to 1 if wcsncasecmp is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSNCASECMP 1

/* Define to 1 if wcsncat is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSNCAT 1

/* Define to 1 if wcsncmp is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSNCMP 1

/* Define to 1 if wcsncpy is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSNCPY 1

/* Define to 1 if wcsnlen is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSNLEN 1

/* Define to 1 if wcsnrtombs is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSNRTOMBS 1

/* Define to 1 if wcspbrk is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSPBRK 1

/* Define to 1 if wcsrchr is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSRCHR 1

/* Define to 1 if wcsrtombs is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSRTOMBS 1

/* Define to 1 if wcsspn is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSSPN 1

/* Define to 1 if wcsstr is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSSTR 1

/* Define to 1 if wcstok is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSTOK 1

/* Define to 1 if wcswidth is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSWIDTH 1

/* Define to 1 if wcsxfrm is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCSXFRM 1

/* Define to 1 if wctob is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCTOB 1

/* Define to 1 if wctrans is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCTRANS 1

/* Define to 1 if wctype is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCTYPE 1

/* Define to 1 if wcwidth is declared even after undefining macros. */
#define HAVE_RAW_DECL_WCWIDTH 1

/* Define to 1 if wmemchr is declared even after undefining macros. */
#define HAVE_RAW_DECL_WMEMCHR 1

/* Define to 1 if wmemcmp is declared even after undefining macros. */
#define HAVE_RAW_DECL_WMEMCMP 1

/* Define to 1 if wmemcpy is declared even after undefining macros. */
#define HAVE_RAW_DECL_WMEMCPY 1

/* Define to 1 if wmemmove is declared even after undefining macros. */
#define HAVE_RAW_DECL_WMEMMOVE 1

/* Define to 1 if wmemset is declared even after undefining macros. */
#define HAVE_RAW_DECL_WMEMSET 1

/* Define to 1 if _Exit is declared even after undefining macros. */
#define HAVE_RAW_DECL__EXIT 1

/* Define to 1 if you have the `readdir' function. */
#define HAVE_READDIR 1

/* Define to 1 if 'long double' and 'double' have the same representation. */
/* #undef HAVE_SAME_LONG_DOUBLE_AS_DOUBLE */

/* Define this if your curses library has the set_escdelay() function. */
#define HAVE_SET_ESCDELAY 1

/* Define to 1 if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define to 1 if you have the `sigaltstack' function. */
#define HAVE_SIGALTSTACK 1

/* Define to 1 if the system has the type `siginfo_t'. */
/* #undef HAVE_SIGINFO_T */

/* Define to 1 if you have the `siginterrupt' function. */
#define HAVE_SIGINTERRUPT 1

/* Define to 1 if 'sig_atomic_t' is a signed integer type. */
/* #undef HAVE_SIGNED_SIG_ATOMIC_T */

/* Define to 1 if 'wchar_t' is a signed integer type. */
/* #undef HAVE_SIGNED_WCHAR_T */

/* Define to 1 if 'wint_t' is a signed integer type. */
/* #undef HAVE_SIGNED_WINT_T */

/* Define to 1 if the system has the type `sigset_t'. */
#define HAVE_SIGSET_T 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define if the return value of the snprintf function is the number of of
   bytes (excluding the terminating NUL) that would have been produced if the
   buffer had been large enough. */
#define HAVE_SNPRINTF_RETVAL_C99 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define if <stdint.h> exists, doesn't clash with <sys/types.h>, and declares
   uintmax_t. */
#define HAVE_STDINT_H_WITH_UINTMAX 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strcasestr' function. */
#define HAVE_STRCASESTR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the `strnlen' function. */
#define HAVE_STRNLEN 1

/* Define if there is a member named d_type in the struct describing directory
   headers. */
#define HAVE_STRUCT_DIRENT_D_TYPE 1

/* Define to 1 if `decimal_point' is a member of `struct lconv'. */
/* #undef HAVE_STRUCT_LCONV_DECIMAL_POINT */

/* Define to 1 if `sa_sigaction' is a member of `struct sigaction'. */
#define HAVE_STRUCT_SIGACTION_SA_SIGACTION 1

/* Define to 1 if `st_atimensec' is a member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_ATIMENSEC */

/* Define to 1 if `st_atimespec.tv_nsec' is a member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_ATIMESPEC_TV_NSEC */

/* Define to 1 if `st_atim.st__tim.tv_nsec' is a member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_ATIM_ST__TIM_TV_NSEC */

/* Define to 1 if `st_atim.tv_nsec' is a member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_ATIM_TV_NSEC 1

/* Define to 1 if `st_birthtimensec' is a member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_BIRTHTIMENSEC */

/* Define to 1 if `st_birthtimespec.tv_nsec' is a member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_BIRTHTIMESPEC_TV_NSEC */

/* Define to 1 if `st_birthtim.tv_nsec' is a member of `struct stat'. */
/* #undef HAVE_STRUCT_STAT_ST_BIRTHTIM_TV_NSEC */

/* Define to 1 if you have the `symlink' function. */
#define HAVE_SYMLINK 1

/* Define to 1 if you have the <sys/bitypes.h> header file. */
/* #undef HAVE_SYS_BITYPES_H */

/* Define to 1 if you have the <sys/cdefs.h> header file. */
#define HAVE_SYS_CDEFS_H 1

/* Define to 1 if you have the <sys/inttypes.h> header file. */
/* #undef HAVE_SYS_INTTYPES_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/wait.h> header file. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <termios.h> header file. */
#define HAVE_TERMIOS_H 1

/* Define to 1 if you have the `towlower' function. */
#define HAVE_TOWLOWER 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if the system has the type 'unsigned long long int'. */
#define HAVE_UNSIGNED_LONG_LONG_INT 1

/* Define this if your curses library has the use_default_colors() function.
   */
#define HAVE_USE_DEFAULT_COLORS 1

/* Define to 1 if you have the `utime' function. */
#define HAVE_UTIME 1

/* Define to 1 if you have the `utimensat' function. */
#define HAVE_UTIMENSAT 1

/* Define to 1 if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* Define to 1 if you have the `vasnprintf' function. */
/* #undef HAVE_VASNPRINTF */

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define if you have the 'wchar_t' type. */
#define HAVE_WCHAR_T 1

/* Define to 1 if you have the `wcrtomb' function. */
#define HAVE_WCRTOMB 1

/* Define to 1 if you have the `wcslen' function. */
#define HAVE_WCSLEN 1

/* Define to 1 if you have the `wcsnlen' function. */
#define HAVE_WCSNLEN 1

/* Define to 1 if you have the `wctomb' function. */
/* #undef HAVE_WCTOMB */

/* Define to 1 if you have the <wctype.h> header file. */
#define HAVE_WCTYPE_H 1

/* Define to 1 if you have the `wcwidth' function. */
#define HAVE_WCWIDTH 1

/* Define to 1 if you have the <winsock2.h> header file. */
/* #undef HAVE_WINSOCK2_H */

/* Define if you have the 'wint_t' type. */
#define HAVE_WINT_T 1

/* Define to 1 if you have the `wmemchr' function. */
#define HAVE_WMEMCHR 1

/* Define to 1 if you have the `wmemcpy' function. */
#define HAVE_WMEMCPY 1

/* Define to 1 if you have the `wmempcpy' function. */
#define HAVE_WMEMPCPY 1

/* Define to 1 if O_NOATIME works. */
#define HAVE_WORKING_O_NOATIME 1

/* Define to 1 if O_NOFOLLOW works. */
#define HAVE_WORKING_O_NOFOLLOW 1

/* Define if utimes works properly. */
#define HAVE_WORKING_UTIMES 1

/* Define to 1 if you have the <xlocale.h> header file. */
#define HAVE_XLOCALE_H 1

/* Define to 1 if you have the <zlib.h> header file. */
/* #undef HAVE_ZLIB_H */

/* Define to 1 if the system has the type `_Bool'. */
#define HAVE__BOOL 1

/* Define to 1 if you have the `_set_invalid_parameter_handler' function. */
/* #undef HAVE__SET_INVALID_PARAMETER_HANDLER */

/* Define to 1 if the compiler supports __builtin_expect,
   and to 2 if <builtins.h> does.  */
#define HAVE___BUILTIN_EXPECT 1
#ifndef HAVE___BUILTIN_EXPECT
# define __builtin_expect(e, c) (e)
#elif HAVE___BUILTIN_EXPECT == 2
# include <builtins.h>
#endif
    

/* Define to 1 if the compiler supports the keyword '__inline'. */
#define HAVE___INLINE 1

/* Define as the bit index in the word where to find bit 0 of the exponent of
   'long double'. */
/* #undef LDBL_EXPBIT0_BIT */

/* Define as the word index where to find the exponent of 'long double'. */
/* #undef LDBL_EXPBIT0_WORD */

/* Define as the bit index in the word where to find the sign of 'long
   double'. */
/* #undef LDBL_SIGNBIT_BIT */

/* Define as the word index where to find the sign of 'long double'. */
/* #undef LDBL_SIGNBIT_WORD */

/* Define to 1 if 'lstat' dereferences a symlink specified with a trailing
   slash. */
#define LSTAT_FOLLOWS_SLASHED_SYMLINK 1

/* If malloc(0) is != NULL, define this to 1. Otherwise define this to 0. */
#define MALLOC_0_IS_NONNULL 1

/* Define to a substitute value for mmap()'s MAP_ANONYMOUS flag. */
/* #undef MAP_ANONYMOUS */

/* Define if the mbrtowc function does not return (size_t) -2 for empty input.
   */
/* #undef MBRTOWC_EMPTY_INPUT_BUG */

/* Define if the mbrtowc function has the NULL pwc argument bug. */
/* #undef MBRTOWC_NULL_ARG1_BUG */

/* Define if the mbrtowc function has the NULL string argument bug. */
/* #undef MBRTOWC_NULL_ARG2_BUG */

/* Define if the mbrtowc function does not return 0 for a NUL character. */
/* #undef MBRTOWC_NUL_RETVAL_BUG */

/* Define if the mbrtowc function returns a wrong return value. */
/* #undef MBRTOWC_RETVAL_BUG */

/* Use GNU style printf and scanf.  */
#ifndef __USE_MINGW_ANSI_STDIO
# define __USE_MINGW_ANSI_STDIO 1
#endif


/* Flag(s) to use to get the full range of extended regular expressions */
#define NANO_REG_EXTENDED REG_EXTENDED

/* Define this to make the nano executable as small as possible. */
/* #undef NANO_TINY */

/* Shut up assert warnings :-) */
#define NDEBUG 1

/* Define if the vasnprintf implementation needs special code for the 'a' and
   'A' directives. */
/* #undef NEED_PRINTF_DIRECTIVE_A */

/* Define if the vasnprintf implementation needs special code for the 'F'
   directive. */
/* #undef NEED_PRINTF_DIRECTIVE_F */

/* Define if the vasnprintf implementation needs special code for the 'ls'
   directive. */
/* #undef NEED_PRINTF_DIRECTIVE_LS */

/* Define if the vasnprintf implementation needs special code for 'double'
   arguments. */
/* #undef NEED_PRINTF_DOUBLE */

/* Define if the vasnprintf implementation needs special code for surviving
   out-of-memory conditions. */
/* #undef NEED_PRINTF_ENOMEM */

/* Define if the vasnprintf implementation needs special code for the ' flag.
   */
/* #undef NEED_PRINTF_FLAG_GROUPING */

/* Define if the vasnprintf implementation needs special code for the '-'
   flag. */
/* #undef NEED_PRINTF_FLAG_LEFTADJUST */

/* Define if the vasnprintf implementation needs special code for the 0 flag.
   */
/* #undef NEED_PRINTF_FLAG_ZERO */

/* Define if the vasnprintf implementation needs special code for infinite
   'double' arguments. */
/* #undef NEED_PRINTF_INFINITE_DOUBLE */

/* Define if the vasnprintf implementation needs special code for infinite
   'long double' arguments. */
/* #undef NEED_PRINTF_INFINITE_LONG_DOUBLE */

/* Define if the vasnprintf implementation needs special code for 'long
   double' arguments. */
/* #undef NEED_PRINTF_LONG_DOUBLE */

/* Define if the vasnprintf implementation needs special code for supporting
   large precisions without arbitrary bounds. */
/* #undef NEED_PRINTF_UNBOUNDED_PRECISION */

/* Define this if you need the _XOPEN_SOURCE_EXTENDED macro for color support.
   */
/* #undef NEED_XOPEN_SOURCE_EXTENDED */

/* Name of package */
#define PACKAGE "nano"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "nano-devel@gnu.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "GNU nano"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "GNU nano 2.9.8"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "nano"

/* Define to the home page for this package. */
#define PACKAGE_URL "http://www.gnu.org/software/nano/"

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.9.8"

/* Where data are placed to. */
#define PKGDATADIR "/usr/local/share/nano"

/* Define if the pthread_in_use() detection is hard. */
/* #undef PTHREAD_IN_USE_DETECTION_HARD */

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'ptrdiff_t'. */
/* #undef PTRDIFF_T_SUFFIX */

/* Specify an alternate rcfile name (default: .nanorc). */
/* #undef RCFILE_NAME */

/* Define this if you know your curses library allows LINES and COLS to be
   redefined to deal with a resizing bug. */
#define REDEFINING_MACROS_OK 1

/* Define to 1 if gnulib's dirfd() replacement is used. */
/* #undef REPLACE_DIRFD */

/* Define to 1 if stat needs help when passed a file name with a trailing
   slash */
/* #undef REPLACE_FUNC_STAT_FILE */

/* Define if nl_langinfo exists but is overridden by gnulib. */
/* #undef REPLACE_NL_LANGINFO */

/* Define if vasnprintf exists but is overridden by gnulib. */
/* #undef REPLACE_VASNPRINTF */

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'sig_atomic_t'. */
/* #undef SIG_ATOMIC_T_SUFFIX */

/* Define as the maximum value of type 'size_t', if the system doesn't define
   it. */
#ifndef SIZE_MAX
/* # undef SIZE_MAX */
#endif

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'size_t'. */
/* #undef SIZE_T_SUFFIX */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at runtime.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define to 1 if the `S_IS*' macros in <sys/stat.h> do not work properly. */
/* #undef STAT_MACROS_BROKEN */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if the type of the st_atim member of a struct stat is struct
   timespec. */
#define TYPEOF_STRUCT_STAT_ST_ATIM_IS_STRUCT_TIMESPEC 1

/* Define if the POSIX multithreading library can be used. */
#define USE_POSIX_THREADS 1

/* Define if references to the POSIX multithreading library should be made
   weak. */
#define USE_POSIX_THREADS_WEAK 1

/* Define if the GNU Pth multithreading library can be used. */
/* #undef USE_PTH_THREADS */

/* Define if references to the GNU Pth multithreading library should be made
   weak. */
/* #undef USE_PTH_THREADS_WEAK */

/* Define this to use the slang wrappers for curses instead of native curses.
   */
/* #undef USE_SLANG */

/* Define if the old Solaris multithreading library can be used. */
/* #undef USE_SOLARIS_THREADS */

/* Define if references to the old Solaris multithreading library should be
   made weak. */
/* #undef USE_SOLARIS_THREADS_WEAK */

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable general extensions on macOS.  */
#ifndef _DARWIN_C_SOURCE
# define _DARWIN_C_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable NetBSD extensions on NetBSD.  */
#ifndef _NETBSD_SOURCE
# define _NETBSD_SOURCE 1
#endif
/* Enable OpenBSD extensions on NetBSD.  */
#ifndef _OPENBSD_SOURCE
# define _OPENBSD_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-5:2014.  */
#ifndef __STDC_WANT_IEC_60559_ATTRIBS_EXT__
# define __STDC_WANT_IEC_60559_ATTRIBS_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-1:2014.  */
#ifndef __STDC_WANT_IEC_60559_BFP_EXT__
# define __STDC_WANT_IEC_60559_BFP_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-2:2015.  */
#ifndef __STDC_WANT_IEC_60559_DFP_EXT__
# define __STDC_WANT_IEC_60559_DFP_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-4:2015.  */
#ifndef __STDC_WANT_IEC_60559_FUNCS_EXT__
# define __STDC_WANT_IEC_60559_FUNCS_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-3:2015.  */
#ifndef __STDC_WANT_IEC_60559_TYPES_EXT__
# define __STDC_WANT_IEC_60559_TYPES_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TR 24731-2:2010.  */
#ifndef __STDC_WANT_LIB_EXT2__
# define __STDC_WANT_LIB_EXT2__ 1
#endif
/* Enable extensions specified by ISO/IEC 24747:2009.  */
#ifndef __STDC_WANT_MATH_SPEC_FUNCS__
# define __STDC_WANT_MATH_SPEC_FUNCS__ 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable X/Open extensions if necessary.  HP-UX 11.11 defines
   mbstate_t only if _XOPEN_SOURCE is defined to 500, regardless of
   whether compiling with -Ae or -D_HPUX_SOURCE=1.  */
#ifndef _XOPEN_SOURCE
/* # undef _XOPEN_SOURCE */
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Define if the native Windows multithreading API can be used. */
/* #undef USE_WINDOWS_THREADS */

/* Version number of package */
#define VERSION "2.9.8"

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'wchar_t'. */
/* #undef WCHAR_T_SUFFIX */

/* Define to l, ll, u, ul, ull, etc., as suitable for constants of type
   'wint_t'. */
/* #undef WINT_T_SUFFIX */

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Enable large inode numbers on Mac OS X 10.5. */
#define _DARWIN_USE_64_BIT_INODE 1

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to 1 on Solaris. */
/* #undef _LCONV_C99 */

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 1 to make NetBSD features available. MINIX 3 needs this. */
#define _NETBSD_SOURCE 1

/* The _Noreturn keyword of C11.  */
#if ! (defined _Noreturn \
       || (defined __STDC_VERSION__ && 201112 <= __STDC_VERSION__))
# if (3 <= __GNUC__ || (__GNUC__ == 2 && 8 <= __GNUC_MINOR__) \
      || 0x5110 <= __SUNPRO_C)
#  define _Noreturn __attribute__ ((__noreturn__))
# elif defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif


/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for 'stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define if you want <regex.h> to include <limits.h>, so that it consistently
   overrides <limits.h>'s RE_DUP_MAX. */
/* #undef _REGEX_INCLUDE_LIMITS_H */

/* Define if you want regoff_t to be at least as wide POSIX requires. */
/* #undef _REGEX_LARGE_OFFSETS */

/* For standard stat data types on VMS. */
#define _USE_STD_STAT 1

/* Define to rpl_ if the getopt replacement functions and variables should be
   used. */
/* #undef __GETOPT_PREFIX */

/* Define to 1 if the system <stdint.h> predates C++11. */
/* #undef __STDC_CONSTANT_MACROS */

/* Define to 1 if the system <stdint.h> predates C++11. */
/* #undef __STDC_LIMIT_MACROS */

/* Please see the Gnulib manual for how to use these macros.

   Suppress extern inline with HP-UX cc, as it appears to be broken; see
   <http://lists.gnu.org/archive/html/bug-texinfo/2013-02/msg00030.html>.

   Suppress extern inline with Sun C in standards-conformance mode, as it
   mishandles inline functions that call each other.  E.g., for 'inline void f
   (void) { } inline void g (void) { f (); }', c99 incorrectly complains
   'reference to static identifier "f" in extern inline function'.
   This bug was observed with Sun C 5.12 SunOS_i386 2011/11/16.

   Suppress the use of extern inline on Apple's platforms, as Libc at least
   through Libc-825.26 (2013-04-09) is incompatible with it; see, e.g.,
   <http://lists.gnu.org/archive/html/bug-gnulib/2012-12/msg00023.html>.
   Perhaps Apple will fix this some day.  */
#if ((__GNUC__ \
      ? defined __GNUC_STDC_INLINE__ && __GNUC_STDC_INLINE__ \
      : (199901L <= __STDC_VERSION__ \
         && !defined __HP_cc \
         && !(defined __SUNPRO_C && __STDC__))) \
     && !defined __APPLE__)
# define _GL_INLINE inline
# define _GL_EXTERN_INLINE extern inline
#elif (2 < __GNUC__ + (7 <= __GNUC_MINOR__) && !defined __STRICT_ANSI__ \
       && !defined __APPLE__)
# if __GNUC_GNU_INLINE__
   /* __gnu_inline__ suppresses a GCC 4.2 diagnostic.  */
#  define _GL_INLINE extern inline __attribute__ ((__gnu_inline__))
# else
#  define _GL_INLINE extern inline
# endif
# define _GL_EXTERN_INLINE extern
#else
# define _GL_INLINE static _GL_UNUSED
# define _GL_EXTERN_INLINE static _GL_UNUSED
#endif

#if 4 < __GNUC__ + (6 <= __GNUC_MINOR__)
# if defined __GNUC_STDC_INLINE__ && __GNUC_STDC_INLINE__
#  define _GL_INLINE_HEADER_CONST_PRAGMA
# else
#  define _GL_INLINE_HEADER_CONST_PRAGMA \
     _Pragma ("GCC diagnostic ignored \"-Wsuggest-attribute=const\"")
# endif
  /* Suppress GCC's bogus "no previous prototype for 'FOO'"
     and "no previous declaration for 'FOO'"  diagnostics,
     when FOO is an inline function in the header; see
     <http://gcc.gnu.org/bugzilla/show_bug.cgi?id=54113>.  */
# define _GL_INLINE_HEADER_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wmissing-prototypes\"") \
    _Pragma ("GCC diagnostic ignored \"-Wmissing-declarations\"") \
    _GL_INLINE_HEADER_CONST_PRAGMA
# define _GL_INLINE_HEADER_END \
    _Pragma ("GCC diagnostic pop")
#else
# define _GL_INLINE_HEADER_BEGIN
# define _GL_INLINE_HEADER_END
#endif

/* Define to a replacement function name for fnmatch(). */
/* #undef fnmatch */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* A replacement for va_copy, if needed.  */
#define gl_va_copy(a,b) ((a) = (b))

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to long or long long if <stdint.h> and <inttypes.h> don't define. */
/* #undef intmax_t */

/* Work around a bug in Apple GCC 4.0.1 build 5465: In C99 mode, it supports
   the ISO C 99 semantics of 'extern inline' (unlike the GNU C semantics of
   earlier versions), but does not display it by setting __GNUC_STDC_INLINE__.
   __APPLE__ && __MACH__ test for Mac OS X.
   __APPLE_CC__ tests for the Apple compiler and its version.
   __STDC_VERSION__ tests for the C99 mode.  */
#if defined __APPLE__ && defined __MACH__ && __APPLE_CC__ >= 5465 && !defined __cplusplus && __STDC_VERSION__ >= 199901L && !defined __GNUC_STDC_INLINE__
# define __GNUC_STDC_INLINE__ 1
#endif

/* Define to a type if <wchar.h> does not define. */
/* #undef mbstate_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define to the type of st_nlink in struct stat, or a supertype. */
/* #undef nlink_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define as the type of the result of subtracting two pointers, if the system
   doesn't define it. */
/* #undef ptrdiff_t */

/* Define to rpl_re_comp if the replacement should be used. */
/* #undef re_comp */

/* Define to rpl_re_compile_fastmap if the replacement should be used. */
/* #undef re_compile_fastmap */

/* Define to rpl_re_compile_pattern if the replacement should be used. */
/* #undef re_compile_pattern */

/* Define to rpl_re_exec if the replacement should be used. */
/* #undef re_exec */

/* Define to rpl_re_match if the replacement should be used. */
/* #undef re_match */

/* Define to rpl_re_match_2 if the replacement should be used. */
/* #undef re_match_2 */

/* Define to rpl_re_search if the replacement should be used. */
/* #undef re_search */

/* Define to rpl_re_search_2 if the replacement should be used. */
/* #undef re_search_2 */

/* Define to rpl_re_set_registers if the replacement should be used. */
/* #undef re_set_registers */

/* Define to rpl_re_set_syntax if the replacement should be used. */
/* #undef re_set_syntax */

/* Define to rpl_re_syntax_options if the replacement should be used. */
/* #undef re_syntax_options */

/* Define to rpl_regcomp if the replacement should be used. */
/* #undef regcomp */

/* Define to rpl_regerror if the replacement should be used. */
/* #undef regerror */

/* Define to rpl_regexec if the replacement should be used. */
/* #undef regexec */

/* Define to rpl_regfree if the replacement should be used. */
/* #undef regfree */

/* Define to the equivalent of the C99 'restrict' keyword, or to
   nothing if this is not supported.  Do not define if restrict is
   supported directly.  */
#define restrict __restrict
/* Work around a bug in Sun C++: it does not support _Restrict or
   __restrict__, even though the corresponding Sun C compiler ends up with
   "#define restrict _Restrict" or "#define restrict __restrict__" in the
   previous line.  Perhaps some future version of Sun C++ will work with
   restrict; if so, hopefully it defines __RESTRICT like Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
# define __restrict__
#endif

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define as a signed type of the same size as size_t. */
/* #undef ssize_t */

/* Define to `int' if <sys/types.h> doesn't define. */
/* #undef uid_t */

/* Define as a marker that can be attached to declarations that might not
    be used.  This helps to reduce warnings, such as from
    GCC -Wunused-parameter.  */
#if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
# define _GL_UNUSED __attribute__ ((__unused__))
#else
# define _GL_UNUSED
#endif
/* The name _UNUSED_PARAMETER_ is an earlier spelling, although the name
   is a misnomer outside of parameter lists.  */
#define _UNUSED_PARAMETER_ _GL_UNUSED

/* gcc supports the "unused" attribute on possibly unused labels, and
   g++ has since version 4.5.  Note to support C++ as well as C,
   _GL_UNUSED_LABEL should be used with a trailing ;  */
#if !defined __cplusplus || __GNUC__ > 4 \
    || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
# define _GL_UNUSED_LABEL _GL_UNUSED
#else
# define _GL_UNUSED_LABEL
#endif

/* The __pure__ attribute was added in gcc 2.96.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
#else
# define _GL_ATTRIBUTE_PURE /* empty */
#endif

/* The __const__ attribute was added in gcc 2.95.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
# define _GL_ATTRIBUTE_CONST __attribute__ ((__const__))
#else
# define _GL_ATTRIBUTE_CONST /* empty */
#endif


/* Define as a macro for copying va_list variables. */
/* #undef va_copy */
