#include "crypto.h"

uint64_t crypto::xor64(uint64_t input, uint64_t key)
{
	return input ^= key;
}
