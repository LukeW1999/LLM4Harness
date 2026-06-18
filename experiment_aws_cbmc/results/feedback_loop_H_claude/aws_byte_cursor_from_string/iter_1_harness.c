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
 * 4. Validity invariants: returned cursor must be valid; src string (if non-NULL) must remain valid
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
    /* bytes are non-deterministic (already set by malloc in CBMC) */
    /* ensure null terminator */
    ((uint8_t *)str->bytes)[len] = 0;
    
    return str;
}

void aws_byte_cursor_from_string_harness(void) {
    /* 1. Non-deterministically choose whether src is NULL or valid */
    bool src_is_null = nondet_bool();
    
    struct aws_string *src = NULL;
    
    if (!src_is_null) {
        /* Allocate a valid aws_string */
        size_t len;
        __CPROVER_assume(len <= 10); /* bound for CBMC tractability */
        
        /* Allocate memory for the string header + bytes + null terminator */
        src = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(src != NULL);
        
        /* Initialize the fields */
        *(struct aws_allocator **)&src->allocator = aws_default_allocator();
        *(size_t *)&src->len = len;
        /* ensure null terminator */
        ((uint8_t *)src->bytes)[len] = 0;
        
        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(src));
    }
    
    /* Save old state of src (if non-NULL) */
    size_t old_len = (src != NULL) ? src->len : 0;
    const uint8_t *old_bytes_ptr = (src != NULL) ? src->bytes : NULL;
    struct aws_allocator *old_allocator = (src != NULL) ? src->allocator : NULL;
    
    /* 3. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);
    
    /* 4. Assert postconditions */
    if (src == NULL) {
        /* If src is NULL, returns an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is non-NULL, cursor points to string bytes with same length */
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
        
        /* The source string must not have been modified */
        assert(src->len == old_len);
        assert(src->bytes == old_bytes_ptr);
        assert(src->allocator == old_allocator);
        
        /* Source string must still be valid */
        assert(aws_string_is_valid(src));
    }
    
    /* The returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));
}
