#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>
#include <stdbool.h>

#define ARRAY_LEN(A)  sizeof(A) / sizeof(A[0])

#define FAST_RAND_MODULO_8(range_size)   ( (uint8_t) ( ((uint16_t)rand() * range_size) >> 8 ))

#endif // _COMMON_H


