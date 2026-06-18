#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_buf_from_c_str_harness(void) {
    /* 1. Set up a non-deterministic null-terminated string */
    bool is_null = nondet_bool();
    
    const char *c_str;
    
    if (is_null) {
        c_str = NULL;
    } else {
        /* Allocate a bounded string with known content */
        size_t str_len;
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        
        char *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);
        /* Make the string null-terminated at exactly str_len */
        /* Ensure no embedded nulls so strlen(buf) == str_len */
        for (size_t i = 0; i < str_len; i++) {
            __CPROVER_assume(buf[i] != '\0');
        }
        buf[str_len] = '\0';
        c_str = buf;
    }
    
    /* 2. Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_c_str(c_str);
    
    /* 3. Assert postconditions */
    if (c_str == NULL) {
        /* NULL input: len=0, capacity=0, buffer=NULL */
        assert(result.len == 0);
        assert(result.capacity == 0);
        assert(result.buffer == NULL);
    } else {
        size_t actual_len = strlen(c_str);
        assert(result.len == actual_len);
        assert(result.capacity == actual_len);
        if (actual_len == 0) {
            assert(result.buffer == NULL);
        } else {
            assert(result.buffer == (uint8_t *)c_str);
        }
    }
    
    /* Allocator is always NULL (no allocation performed) */
    assert(result.allocator == NULL);
    
    /* 4. Assert validity invariant */
    assert(aws_byte_buf_is_valid(&result));
}
