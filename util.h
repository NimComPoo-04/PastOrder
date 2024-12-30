#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>

#define ARRAY_BLOCK_SIZE 32

static inline void *array_append(void *array, int *current_size, void *element, int element_size)
{
	if(*current_size % ARRAY_BLOCK_SIZE == 0)	
	{
		array = realloc(array, (*current_size + ARRAY_BLOCK_SIZE) * element_size);
		assert(array != NULL);
	}

	memcpy(array + *current_size * element_size, element, element_size);
	++*current_size;

	return array;
}

static inline float clamp(float t, float a, float b)
{
	return (t < a ? a : (t > b ? b : t));
}

static inline float min(float x, float y)
{
	return x < y ? x : y;
}

static inline float max(float x, float y)
{
	return x > y ? x : y;
}

#define sign(a) ((a) < 0 ? -1 : 1)
#define swap(x, y, t) do { t __tmp = x; x = y; y = __tmp; } while(0)

#endif
