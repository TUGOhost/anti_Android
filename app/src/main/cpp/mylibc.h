#ifndef DETECTDEBUGGER_MYLIBC_H
#define DETECTDEBUGGER_MYLIBC_H

/*
 * These functions are copied from glibc, android libc, apple libc open source code.
 * This is to avoid easy bypass through libc functions
 */

__attribute__((always_inline))
static inline size_t
my_strlcpy(char *dst, const char *src, size_t siz) {
    char *d = dst;
    const char *s = src;
    size_t n = siz;
    /* Copy as many bytes as will fit */
    if (n != 0) {
        while (--n != 0) {
            if ((*d++ = *s++) == '\0')
                break;
        }
    }
    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (siz != 0)
            *d = '\0';        /* NUL-terminate dst */
        while (*s++);
    }
    return (s - src - 1);    /* count does not include NUL */
}

__attribute__((always_inline))
static inline
size_t my_strlen(const char *s) {
    size_t len = 0;
    while (*s++) len++;
    return len;
}

__attribute__((always_inline))
static inline int
my_strncmp(const char *s1, const char *s2, size_t n) {
    if (n == 0)
        return (0);
    do {
        if (*s1 != *s2++)
            return (*(unsigned char *) s1 - *(unsigned char *) --s2);
        if (*s1++ == 0)
            break;
    } while (--n != 0);
    return (0);
}

__attribute__((always_inline))
static inline char *
my_strstr(const char *s, const char *find) {
    char c, sc;
    size_t len;

    if ((c = *find++) != '\0') {
        len = my_strlen(find);
        do {
            do {
                if ((sc = *s++) == '\0')
                    return (NULL);
            } while (sc != c);
        } while (my_strncmp(s, find, len) != 0);
        s--;
    }
    return ((char *) s);
}

__attribute__((always_inline))
static inline
void *my_memset(void *dst, int c, size_t n) {
    char *q = (char *) dst;
    char *end = q + n;
    for (;;) {
        if (q >= end) break;
        *q++ = (char) c;
        if (q >= end) break;
        *q++ = (char) c;
        if (q >= end) break;
        *q++ = (char) c;
        if (q >= end) break;
        *q++ = (char) c;
    }
    return dst;
}

__attribute__((always_inline))
static inline int
my_strcmp(const char *s1, const char *s2) {
    while (*s1 == *s2++)
        if (*s1++ == 0)
            return (0);
    return (*(unsigned char *) s1 - *(unsigned char *) --s2);
}

__attribute__((always_inline))
static inline int my_atoi(const char *s) {
    int n = 0, neg = 0;
    while (isspace(*s)) s++;
    switch (*s) {
        case '-':
            neg = 1;
        case '+':
            s++;
    }
    /* Compute n as a negative number to avoid overflow on INT_MIN */
    while (isdigit(*s))
        n = 10 * n - (*s++ - '0');
    return neg ? n : -n;
}

__attribute__((always_inline))
char *
my_strtok_r(char *s, const char *delim, char **last) {
    char *spanp;
    int c, sc;
    char *tok;
    if (s == NULL && (s = *last) == NULL)
        return (NULL);
    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
    cont:
    c = *s++;
    for (spanp = (char *) delim; (sc = *spanp++) != 0;) {
        if (c == sc)
            goto cont;
    }
    if (c == 0) {        /* no non-delimiter characters */
        *last = NULL;
        return (NULL);
    }
    tok = s - 1;
    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;) {
        c = *s++;
        spanp = (char *) delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *last = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}


#endif //DETECTDEBUGGER_MYLIBC_H
