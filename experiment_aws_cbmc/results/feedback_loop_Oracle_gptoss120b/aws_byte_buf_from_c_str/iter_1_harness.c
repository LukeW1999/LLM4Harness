#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
char nondet_char(void);
bool nondet_bool(void);

void aws_byte_buf_from_c_str_harness(void) {
    /* Choose whether the input string is NULL */
    bool is_null = nondet_bool();

    char *c_str = NULL;
    size_t alloc_len = 0;

    if (!is_null) {
        /* Allocate a nondeterministic length (bounded) */
        alloc_len = nondet_size_t();
        __CPROVER_assume(alloc_len < 256);               /* bound for tractability */

        c_str = malloc(alloc_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* Fill with nondeterministic characters */
        for (size_t i = 0; i < alloc_len; ++i) {
            c_str[i] = nondet_char();
        }
        /* Ensure null‑termination */
        c_str[alloc_len] = '\0';
    }

    /* Make a copy of the original memory for frame condition checking */
    char *c_str_copy = NULL;
    if (c_str != NULL) {
        c_str_copy = malloc(alloc_len + 1);
        __CPROVER_assume(c_str_copy != NULL);
        for (size_t i = 0; i <= alloc_len; ++i) {
            c_str_copy[i] = c_str[i];
        }
    }

    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 1. Return value / error code correctness (validity predicate) */
    assert(aws_byte_buf_is_valid(&result));

    /* 2. Output buffer length/capacity invariants */
    assert(result.len == result.capacity);
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        /* Compute expected length (strlen) manually, bounded by alloc_len */
        size_t expected_len = 0;
        while (expected_len < alloc_len && c_str[expected_len] != '\0') {
            ++expected_len;
        }
        /* The string must be null‑terminated within the allocated region */
        assert(c_str[expected_len] == '\0');

        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        assert(result.buffer == (uint8_t *)c_str);
    }
    assert(result.allocator == NULL);

    /* 3. Memory not modified beyond the function's contract (frame condition) */
    if (c_str != NULL) {
        for (size_t i = 0; i <= alloc_len; ++i) {
            assert(c_str[i] == c_str_copy[i]);
        }
    }

    /* Clean up */
    free(c_str);
    free(c_str_copy);
    return 0;
}
