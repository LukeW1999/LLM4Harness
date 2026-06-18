#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Declare input: a non-deterministic C string (may be NULL) */
    const char *c_str;
    
    /* Non-deterministically choose whether c_str is NULL or a valid string */
    bool is_null = nondet_bool();
    if (is_null) {
        c_str = NULL;
    } else {
        /* Create a bounded string for CBMC to handle */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        char *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);
        buf[str_len] = '\0';
        c_str = buf;
    }

    /* 2. Call function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 3. Assert postconditions */
    
    /* Case 1: NULL input */
    if (c_str == NULL) {
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
        assert(result.allocator == NULL);
    } else {
        /* Case 2: Non-NULL input */
        size_t expected_len = strlen(c_str);
        assert(result.len == expected_len);
        assert(result.capacity == expected_len);
        if (expected_len == 0) {
            assert(result.buffer == NULL);
        } else {
            assert(result.buffer == (uint8_t *)c_str);
        }
        assert(result.allocator == NULL);
    }

    /* 4. Assert validity invariant */
    assert(aws_byte_buf_is_valid(&result));
}
