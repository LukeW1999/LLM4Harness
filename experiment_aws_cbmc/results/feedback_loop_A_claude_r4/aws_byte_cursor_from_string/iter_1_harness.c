#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Harness for aws_byte_cursor_from_string
 *
 * From the Doxygen:
 *   "Creates an aws_byte_cursor from an existing string.
 *    If the src is NULL, it returns an empty cursor"
 *
 * Analysis:
 * 1. Changed fields: returns a new aws_byte_cursor (ptr and len set from string)
 * 2. Unchanged fields: src string is not modified
 * 3. Failure path: if src is NULL, returns empty cursor (ptr=NULL, len=0)
 * 4. Validity invariants: returned cursor must be valid; src string unchanged
 */

/* Helper to allocate a valid aws_string non-deterministically */
struct aws_string *ensure_string_is_allocated(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate memory for the string header + bytes + null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) {
        return NULL;
    }
    
    /* Initialize the fields - need to cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    
    /* The bytes are already allocated as part of the struct */
    return str;
}

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL or a valid string */
    bool is_null = nondet_bool();
    
    const struct aws_string *src = NULL;
    size_t original_len = 0;
    const uint8_t *original_bytes = NULL;
    
    if (!is_null) {
        /* Allocate a valid aws_string */
        size_t len;
        __CPROVER_assume(len <= 32); /* bound the size for tractability */
        
        struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);
        
        /* Initialize fields */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = len;
        
        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
        
        src = str;
        original_len = src->len;
        original_bytes = aws_string_bytes(src);
    }
    
    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);
    
    if (src == NULL) {
        /* NULL input: returns empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* Valid input: cursor points to string bytes with same length */
        assert(result.ptr == original_bytes);
        assert(result.len == original_len);
        
        /* The source string must not have been modified */
        assert(src->len == original_len);
        assert(aws_string_bytes(src) == original_bytes);
        
        /* The returned cursor must be valid */
        assert(aws_byte_cursor_is_valid(&result));
        
        /* The source string must still be valid */
        assert(aws_string_is_valid(src));
    }
}
