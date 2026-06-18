#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_cursor_from_c_str_harness(void) {
    /* 1. Set up a non-deterministic null-terminated string */
    /* We need a bounded c_str. Use nondet to decide if it's NULL or a valid string. */
    bool is_null = nondet_bool();
    
    const char *c_str;
    
    if (is_null) {
        c_str = NULL;
    } else {
        /* Allocate a bounded string */
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        
        char *buf = malloc(str_len + 1);
        __CPROVER_assume(buf != NULL);
        /* Make it a valid null-terminated string by setting the last byte to '\0' */
        buf[str_len] = '\0';
        c_str = buf;
    }
    
    /* 2. Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);
    
    /* 3. Assert postconditions */
    
    /* If c_str is NULL, ptr should be NULL and len should be 0 */
    if (c_str == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If c_str is non-NULL, ptr should point to c_str and len should be strlen(c_str) */
        assert(result.ptr == (uint8_t *)c_str);
        assert(result.len == strlen(c_str));
    }
    
    /* 4. Assert validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
