#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include "config.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <arpa/inet.h>


// Murmur3 hash function
uint32_t murmur3(const uint8_t *data, size_t len, uint32_t seed);

#endif // HASH_FUNCTIONS_H