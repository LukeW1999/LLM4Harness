#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

/* CBMC nondeterministic helpers */
char nondet_char(void);
size_t nondet_size_t(void);
_Bool nondet_bool(void);

void aws_byte_buf_from_c_str_harness(void) {
    /* nondeterministic input string */
    const char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t max_len = nondet_size_t();
        __CPROVER_assume(max_len <= 256);
        char *buf = malloc(max_len + 1);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < max_len; ++i) {
            buf[i] = nondet_char();
        }
        buf[max_len] = '\0';
        c_str = buf;
    }

    /* make a copy of the input string for frame condition checking */
    size_t orig_len = 0;
    char *orig_copy = NULL;
    if (c_str != NULL) {
        orig_len = strlen(c_str);
        orig_copy = malloc(orig_len + 1);
        __CPROVER_assume(orig_copy != NULL);
        memcpy(orig_copy, c_str, orig_len + 1);
    }

    /* call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 1. Return value / error code correctness (validity predicate) */
    assert(aws_byte_buf_is_valid(&result));

    /* 2. Output buffer length/capacity invariants */
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        if (expected_len == 0) {
            assert(result.buffer == NULL);
        } else {
            assert(result.buffer == (uint8_t *)c_str);
        }
    }
    assert(result.allocator == NULL);

    /* 3. Memory not modified beyond the function's contract (frame condition) */
    if (c_str != NULL) {
        for (size_t i = 0; i <= orig_len; ++i) {
            assert(((const char *)c_str)[i] == orig_copy[i]);
        }
    }

    return 0;
}
