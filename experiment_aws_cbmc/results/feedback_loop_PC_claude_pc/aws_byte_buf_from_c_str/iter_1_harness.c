#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Set up a non-deterministic null-terminated C string */
    /* We use a bounded array to keep the state space finite */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    char *c_str;
    if (nondet_bool()) {
        /* NULL pointer case */
        c_str = NULL;
    } else {
        /* Non-NULL: allocate a buffer of length `len + 1` for the null terminator */
        c_str = malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        /* Ensure it is a valid null-terminated string */
        c_str[len] = '\0';
    }

    /* 2. Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 3. Assert postconditions */

    /* allocator must always be NULL (no allocation is performed) */
    assert(result.allocator == NULL);

    if (c_str == NULL) {
        /* NULL input: len == 0, capacity == 0, buffer == NULL */
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        size_t expected_len = strlen(c_str);
        /* len equals strlen(c_str) */
        assert(result.len == expected_len);
        /* capacity equals len */
        assert(result.capacity == expected_len);
        if (expected_len == 0) {
            /* empty string: buffer is NULL */
            assert(result.buffer == NULL);
        } else {
            /* non-empty string: buffer points to the original c_str */
            assert(result.buffer == (uint8_t *)c_str);
        }
    }

    /* 4. Validity invariant */
    assert(aws_byte_buf_is_valid(&result));
}
