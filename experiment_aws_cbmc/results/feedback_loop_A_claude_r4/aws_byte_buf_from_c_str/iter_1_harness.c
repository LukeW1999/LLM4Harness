#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Set up a non-deterministic c_str input */
    /* c_str can be NULL or a valid null-terminated string */
    const char *c_str;
    
    /* Non-deterministically choose whether c_str is NULL or a valid string */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* Create a bounded null-terminated string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        char *str = malloc(len + 1);
        __CPROVER_assume(str != NULL);
        str[len] = '\0';
        c_str = str;
    }

    /* 2. Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    /* 3. Assert postconditions */
    
    /* From the implementation:
     * - buf.len = (c_str == NULL) ? 0 : strlen(c_str)
     * - buf.capacity = buf.len
     * - buf.buffer = (capacity == 0) ? NULL : (uint8_t *)c_str
     * - buf.allocator = NULL
     */
    
    if (c_str == NULL) {
        /* NULL input: len = 0, capacity = 0, buffer = NULL */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        /* Non-NULL input: len = strlen(c_str), capacity = len, buffer = c_str (if len > 0) */
        size_t expected_len = strlen(c_str);
        assert(buf.len == expected_len);
        assert(buf.capacity == expected_len);
        if (expected_len == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer == (uint8_t *)c_str);
        }
    }
    
    /* allocator is always NULL */
    assert(buf.allocator == NULL);
    
    /* len == capacity always */
    assert(buf.len == buf.capacity);
    
    /* 4. Assert validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
