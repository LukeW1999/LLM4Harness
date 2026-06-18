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
    if (str == NULL) return NULL;
    
    /* Initialize the fields - cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
    *(size_t *)&str->len = len;
    
    /* The bytes are already allocated as part of the struct */
    return str;
}

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL or valid */
    bool is_null = nondet_bool();
    
    const struct aws_string *src = NULL;
    
    if (!is_null) {
        /* Create a valid aws_string */
        size_t len;
        __CPROVER_assume(len <= 10); /* bound for CBMC tractability */
        
        /* Allocate memory for the string: header + len bytes + 1 null terminator */
        struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);
        
        /* Initialize fields (cast away const for setup) */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = len;
        
        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
        
        src = str;
    }
    
    /* Save state of src before call */
    const struct aws_string *old_src = src;
    size_t old_len = (src != NULL) ? src->len : 0;
    const uint8_t *old_bytes = (src != NULL) ? aws_string_bytes(src) : NULL;
    
    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);
    
    /* Postconditions */
    if (src == NULL) {
        /* NULL input: returns empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* Valid input: cursor points to string bytes with correct length */
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
        
        /* The source string must not have been modified */
        assert(src == old_src);
        assert(src->len == old_len);
        assert(aws_string_bytes(src) == old_bytes);
    }
    
    /* Validity invariant: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));
    
    /* If src is non-null, it must still be valid */
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
}
