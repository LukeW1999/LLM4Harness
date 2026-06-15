#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 100

// Define aws_array_eq as a simple comparison function
bool aws_array_eq(const void *array_a, size_t len_a, const void *array_b, size_t len_b) {
    if (len_a != len_b) {
        return false;
    }
    if (len_a == 0) {
        return true;
    }
    return memcmp(array_a, array_b, len_a) == 0;
}

void aws_array_eq_harness() {
    size_t len_a;
    size_t len_b;
    
    // Nondeterministic lengths, bounded by MAX_BUFFER_SIZE
    __CPROVER_assume(len_a <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len_b <= MAX_BUFFER_SIZE);
    
    const void *array_a = NULL;
    const void *array_b = NULL;
    
    // Allocate and initialize arrays with nondeterministic values if length > 0
    if (len_a > 0) {
        char *buf_a = malloc(len_a);
        __CPROVER_assume(buf_a != NULL);
        for (size_t i = 0; i < len_a; i++) {
            buf_a[i] = nondet_char();
        }
        array_a = buf_a;
    }
    if (len_b > 0) {
        char *buf_b = malloc(len_b);
        __CPROVER_assume(buf_b != NULL);
        for (size_t i = 0; i < len_b; i++) {
            buf_b[i] = nondet_char();
        }
        array_b = buf_b;
    }
    
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);
    
    bool expected = (len_a == len_b) && (len_a == 0 || memcmp(array_a, array_b, len_a) == 0);
    assert(result == expected);
}
