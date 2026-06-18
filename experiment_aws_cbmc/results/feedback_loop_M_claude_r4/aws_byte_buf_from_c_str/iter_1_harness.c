#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Set up a non-deterministic null-terminated C string */
    /* We use a bounded array to represent the c_str input */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

    /* Non-deterministically choose NULL or a valid string */
    bool use_null = nondet_bool();
    const char *c_str;

    if (use_null) {
        c_str = NULL;
    } else {
        /* Allocate a buffer of str_len + 1 bytes for the null-terminated string */
        char *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);
        /* Ensure null terminator at position str_len */
        buf[str_len] = '\0';
        c_str = buf;
    }

    /* 2. Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 3. Assert postconditions */

    /* The function sets allocator to NULL always */
    assert(result.allocator == NULL);

    if (c_str == NULL) {
        /* NULL input: len = 0, capacity = 0, buffer = NULL */
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        /* Non-NULL input: len = strlen(c_str), capacity = len, buffer = (uint8_t*)c_str */
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        if (expected_len == 0) {
            assert(result.buffer == NULL);
        } else {
            assert(result.buffer == (uint8_t *)c_str);
        }
    }

    /* 4. Assert validity invariant */
    assert(aws_byte_buf_is_valid(&result));
}
