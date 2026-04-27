#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Harness for aws_byte_cursor_from_string
 *
 * Implementation analysis:
 * - If src is NULL: returns a zero-initialized cursor (ptr=NULL, len=0)
 * - If src is non-NULL: returns cursor with ptr=aws_string_bytes(src), len=src->len
 *
 * Fields written: cursor.ptr, cursor.len (returned by value)
 * Fields never touched: src is not modified
 * Failure path: NULL input → zero cursor
 * Validity: returned cursor must be valid
 */

/* Helper to create a valid aws_string non-deterministically */
struct aws_string *ensure_string_is_allocated(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate enough space for the struct + len bytes + null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    
    /* Set the length field (it's const, but we can write via cast in harness) */
    *(size_t *)&str->len = len;
    
    /* The bytes array is readable */
    /* allocator can be anything (even NULL for static strings) */
    
    return str;
}

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministically choose whether src is NULL or valid */
    struct aws_string *src;
    
    bool is_null = nondet_bool();
    
    if (is_null) {
        src = NULL;
    } else {
        /* Create a valid aws_string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        /* Allocate struct + bytes + null terminator */
        src = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(src != NULL);
        
        /* Initialize len field */
        *(size_t *)&src->len = len;
        
        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(src));
    }
    
    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);
    
    /* Postconditions */
    if (src == NULL) {
        /* NULL input: cursor should be zero-initialized */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* Non-NULL input: cursor should point to string bytes with correct length */
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
        
        /* The src string is not modified */
        assert(aws_string_is_valid(src));
    }
    
    /* Validity invariant: returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));
}
