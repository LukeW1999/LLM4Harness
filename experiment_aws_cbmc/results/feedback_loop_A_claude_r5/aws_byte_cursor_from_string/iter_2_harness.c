#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL or valid */
    bool is_null;
    
    const struct aws_string *src = NULL;
    
    if (!is_null) {
        /* Allocate a valid aws_string */
        size_t len;
        __CPROVER_assume(len <= 10); /* bound for CBMC */
        
        struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);
        
        /* Initialize fields */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = len;
        
        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
        
        src = str;
    }
    
    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);
    
    /* Postconditions */
    if (src == NULL) {
        /* If src is NULL, returns an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is valid, cursor points to string bytes with same length */
        assert(result.len == src->len);
        assert(result.ptr == aws_string_bytes(src));
        
        /* The cursor should be valid */
        assert(aws_byte_cursor_is_valid(&result));
        
        /* The source string should be unchanged */
        assert(aws_string_is_valid(src));
    }
}
