#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STR_LEN 32

void aws_byte_buf_from_c_str_harness(void) {
    /* Non-deterministically choose whether c_str is NULL or a valid string */
    bool is_null;
    const char *c_str;

    if (is_null) {
        c_str = NULL;
    } else {
        /* Allocate a bounded non-deterministic null-terminated string */
        size_t len;
        __CPROVER_assume(len <= MAX_STR_LEN);

        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);

        /* Ensure null terminator is present */
        buf[len] = '\0';

        c_str = buf;
    }

    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* Postcondition: allocator is always NULL */
    assert(result.allocator == NULL);

    /* Postcondition: len == capacity */
    assert(result.len == result.capacity);

    if (c_str == NULL) {
        /* Postcondition: NULL input yields zero-length buffer with NULL buffer pointer */
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);

        /* Postcondition: len matches strlen */
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);

        if (expected_len == 0) {
            /* Postcondition: empty string yields NULL buffer */
            assert(result.buffer == NULL);
        } else {
            /* Postcondition: non-empty string yields buffer pointing to c_str */
            assert(result.buffer == (uint8_t *)c_str);
        }
    }

    /* Postcondition: the result is a valid aws_byte_buf */
    assert(aws_byte_buf_is_valid(&result));
}
