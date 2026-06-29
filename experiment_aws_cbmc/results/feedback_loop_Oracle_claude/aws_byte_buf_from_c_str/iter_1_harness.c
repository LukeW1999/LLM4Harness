#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* Nondeterministic choice: null or non-null c_str */
    const char *c_str;
    bool is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);

    if (is_null) {
        c_str = NULL;
    } else {
        /* Create a bounded-length null-terminated string */
        size_t len;
        __CPROVER_assume(len <= 32);
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        buf[len] = '\0';
        c_str = buf;
    }

    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* Postcondition 1: Validity - result must be a valid aws_byte_buf */
    assert(aws_byte_buf_is_valid(&result));

    /* Postcondition 2: allocator is always NULL (no allocation performed) */
    assert(result.allocator == NULL);

    /* Postcondition 3: Length/capacity invariants */
    /* len == capacity always */
    assert(result.len == result.capacity);

    if (c_str == NULL) {
        /* Postcondition 4: null input -> zero length, zero capacity, null buffer */
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);

        /* Postcondition 5: non-null input -> len equals strlen(c_str) */
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);

        if (expected_len == 0) {
            /* Empty string -> buffer is NULL */
            assert(result.buffer == NULL);
        } else {
            /* Non-empty string -> buffer points to c_str */
            assert(result.buffer == (uint8_t *)c_str);
        }
    }

    /* Postcondition 6: Frame condition - buffer pointer (if non-null) points
       into the original c_str memory, not a new allocation */
    if (result.buffer != NULL) {
        assert(result.buffer == (uint8_t *)c_str);
    }
}
