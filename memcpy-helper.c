
#include <stdio.h>
#define __USE_GNU
#include <dlfcn.h>

static size_t memcpy_size = 0;
static size_t last_checkpoint = 0;

void *memcpy (void *dest, const void *src, size_t n)
{
    static void * (*real_memcpy) (void *, const void *, size_t) = 0;

    if (!real_memcpy) {
        real_memcpy = dlsym (RTLD_NEXT, "memcpy");
    }

    memcpy_size += n;

    if (memcpy_size - last_checkpoint > (1024*1024)) {
asm("int3");
        fprintf (stderr, "memcpy total: %d\n", memcpy_size);
        last_checkpoint = memcpy_size;
    }
    return real_memcpy (dest, src, n);
/*
    unsigned char *d = dest;
    unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dest;
*/
}

