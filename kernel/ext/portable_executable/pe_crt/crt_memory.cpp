#include "crt_memory.hpp"

#if !defined(__clang__)
#include <intrin.h>
#endif

void portable_executable::pe_crt::memcpy(void* destination, const void* source, pe_crt::size_t size)
{
#if defined(__clang__)
    __asm__ __volatile__(
        "1:\n\t"
        "movb (%1), %%al\n\t"
        "movb %%al, (%0)\n\t"
        "inc %0\n\t"
        "inc %1\n\t"
        "dec %2\n\t"
        "jnz 1b\n"
        : "=r" (destination), "=r" (source), "=r" (size)
        : "0" (destination), "1" (source), "2" (size)
        : "memory", "%al");
#else
    __movsb(static_cast<pe_crt::uint8_t*>(destination), static_cast<const pe_crt::uint8_t*>(source), size);
#endif
}

void portable_executable::pe_crt::memset(void* destination, pe_crt::uint8_t value, pe_crt::size_t size)
{
#if defined(__clang__)
    __asm__ __volatile__(
        "1:\n\t"
        "movb %1, (%0)\n\t"
        "inc %0\n\t"
        "dec %2\n\t"
        "jnz 1b\n"
        : "=r" (destination), "=r" (value), "=r" (size)
        : "0" (destination), "1" (value), "2" (size)
        : "memory");
#else
    __stosb(static_cast<pe_crt::uint8_t*>(destination), value, size);
#endif
}