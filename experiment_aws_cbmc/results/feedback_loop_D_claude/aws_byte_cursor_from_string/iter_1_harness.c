// === STEP 1: SUCCESS PATH ===
// When src is non-NULL, aws_byte_cursor_from_string returns a cursor where:
//   - cursor.ptr: points to aws_string_bytes(src) = (uint8_t *)src->bytes
//   - cursor.len: equals src->len
//
// === STEP 2: FAILURE PATH (NULL src) ===
// When src is NULL, returns a zeroed cursor:
//   - cursor.ptr: NULL (0)
//   - cursor.len: 0
//
// === STEP 3: FRAME CONDITIONS ===
// The function takes a const struct aws_string *src and returns a struct aws_byte_cursor by value.
// src (const struct aws_string *):
//   - allocator: UNCHANGED always (not modified)
//   - len: UNCHANGED always (not modified)
//   - bytes: UNCHANGED always (not modified)
// Return value (struct aws_byte_cursor):
//   - ptr: CHANGED based on src (NULL if src==NULL, else aws_string_bytes(src))
//   - len: CHANGED based on src (0 if src==NULL, else src->len)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&result): YES (must hold after call)
//   - aws_string_is_valid(src): YES if src != NULL (precondition)

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

void aws_byte_cursor_from_string_harness(void) {
    /* Nondeterministically choose whether src is NULL or a valid aws_string */
    struct aws_string *src;
    
    if (nondet_bool()) {
        /* NULL case */
        src = NULL;
    } else {
        /* Non-NULL case: allocate a valid aws_string */
        /* We need to allocate an aws_string with a bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        
        /* Allocate memory for the aws_string struct with bytes of length len+1 */
        src = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(src != NULL);
        
        /* Set the length field (cast away const for initialization) */
        *(size_t *)(&src->len) = len;
        
        /* allocator can be anything (including NULL for static strings) */
        /* bytes are already allocated as part of the struct */
        
        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(src));
    }
    
    /* Save old state of src if non-NULL */
    size_t old_len = (src != NULL) ? src->len : 0;
    const uint8_t *old_bytes_ptr = (src != NULL) ? aws_string_bytes(src) : NULL;
    
    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);
    
    /* === Assertions for NULL case === */
    if (src == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* === Assertions for non-NULL case === */
        assert(result.ptr == old_bytes_ptr);
        assert(result.len == old_len);
        
        /* Frame condition: src is unchanged */
        assert(src->len == old_len);
        assert(aws_string_bytes(src) == old_bytes_ptr);
    }
    
    /* === Validity invariant === */
    assert(aws_byte_cursor_is_valid(&result));
}
