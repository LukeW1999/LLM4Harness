#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Set up a non-deterministic null-terminated string */
    /* We bound the string length to keep the state space manageable */
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

    /* Non-deterministically choose whether c_str is NULL */
    bool is_null = nondet_bool();
    const char *c_str;

    if (is_null) {
        c_str = NULL;
    } else {
        /* Allocate a string of bounded length */
        char *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);
        /* Ensure null terminator */
        buf[str_len] = '\0';
        c_str = buf;
    }

    /* 2. Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 3. Assert postconditions */

    /* The function returns a struct by value. Check all fields. */

    /* allocator is always NULL (no allocation is done) */
    assert(result.allocator == NULL);

    if (c_str == NULL) {
        /* NULL input: len = 0, capacity = 0, buffer = NULL */
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        /* Non-NULL input: len = strlen(c_str), capacity = len */
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        /* buffer points into the string data - check it's non-null when len > 0 or capacity > 0 */
        if (expected_len > 0) {
            assert(result.buffer != NULL);
        }
    }

    /* len == capacity always */
    assert(result.len == result.capacity);

    /* 4. Validity invariant */
    assert(aws_byte_buf_is_valid(&result));
}
