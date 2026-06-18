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
    /* c_str can be NULL or a valid string - both are valid inputs */
    
    /* If c_str is non-NULL, we need it to be a valid null-terminated string.
       We bound the string length to keep the state space manageable. */
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    
    if (nondet_bool()) {
        /* NULL case */
        c_str = NULL;
    } else {
        /* Non-NULL case: allocate a bounded string */
        uint8_t *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);
        buf[str_len] = '\0';
        c_str = (const char *)buf;
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
