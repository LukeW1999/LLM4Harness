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
    
    /* c_str can be NULL or point to a valid null-terminated string */
    /* We non-deterministically choose between NULL and a valid string */
    bool is_null = nondet_bool();
    if (is_null) {
        c_str = NULL;
    } else {
        /* Create a bounded null-terminated string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        uint8_t *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        buf[len] = '\0';
        c_str = (const char *)buf;
    }

    /* 2. Call function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);

    /* 3. Assert postconditions */
    
    /* From the implementation:
     * - buf.len = (c_str == NULL) ? 0 : strlen(c_str)
     * - buf.capacity = buf.len
     * - buf.buffer = (capacity == 0) ? NULL : (uint8_t *)c_str
     * - buf.allocator = NULL
     */
    
    if (c_str == NULL) {
        /* NULL input: len=0, capacity=0, buffer=NULL */
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
    
    /* allocator is always NULL */
    assert(result.allocator == NULL);
    
    /* capacity always equals len */
    assert(result.capacity == result.len);
    
    /* 4. Assert validity invariant */
    assert(aws_byte_buf_is_valid(&result));
}
