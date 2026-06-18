#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Harness for aws_string_eq_byte_buf
 *
 * From the implementation:
 *   - If both str and buf are NULL, returns true
 *   - If exactly one is NULL, returns false
 *   - Otherwise, returns aws_array_eq(str->bytes, str->len, buf->buffer, buf->len)
 *
 * Neither str nor buf are modified by this function.
 * The function is a pure comparison — no fields change.
 */

/* We need a helper to allocate an aws_string with bounded length */
struct aws_string *ensure_string_is_allocated(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    
    /* Allocate memory for the aws_string header plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) {
        return NULL;
    }
    
    /* Initialize the fields — note they are const so we use a cast trick */
    /* We write through a non-const pointer to initialize */
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;
    
    struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
    *alloc_ptr = nondet_bool() ? aws_default_allocator() : NULL;
    
    /* bytes are already allocated as part of the struct, CBMC will treat them as nondet */
    
    return str;
}

void aws_string_eq_byte_buf_harness(void) {
    /* 1. Declare inputs non-deterministically */
    
    /* str can be NULL or a valid aws_string */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();
    
    if (!str_is_null) {
        /* Allocate a bounded aws_string */
        size_t str_len;
        __CPROVER_assume(str_len <= 10); /* bound for CBMC tractability */
        
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);
        
        /* Initialize const fields via pointer cast */
        size_t *len_ptr = (size_t *)&str->len;
        *len_ptr = str_len;
        
        struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
        *alloc_ptr = nondet_bool() ? aws_default_allocator() : NULL;
        
        __CPROVER_assume(aws_string_is_valid(str));
    }
    
    /* buf can be NULL or a valid aws_byte_buf */
    struct aws_byte_buf *buf = NULL;
    bool buf_is_null = nondet_bool();
    
    struct aws_byte_buf buf_val;
    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf_val, 10));
        ensure_byte_buf_has_allocated_buffer_member(&buf_val);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf_val));
        buf = &buf_val;
    }
    
    /* 2. Save old state (nothing changes, but we verify immutability) */
    /* Save str fields if non-null */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;
    
    /* Save buf fields if non-null */
    size_t old_buf_len = (buf != NULL) ? buf->len : 0;
    size_t old_buf_capacity = (buf != NULL) ? buf->capacity : 0;
    struct aws_allocator *old_buf_allocator = (buf != NULL) ? buf->allocator : NULL;
    uint8_t *old_buf_buffer = (buf != NULL) ? buf->buffer : NULL;
    
    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);
    
    /* 4. Assert postconditions */
    
    /* Case: both NULL → must return true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }
    
    /* Case: exactly one NULL → must return false */
    if (str == NULL && buf != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf == NULL) {
        assert(result == false);
    }
    
    /* 5. Assert immutability — neither str nor buf are modified */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        /* Validity still holds */
        assert(aws_string_is_valid(str));
    }
    
    if (buf != NULL) {
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_capacity);
        assert(buf->allocator == old_buf_allocator);
        assert(buf->buffer == old_buf_buffer);
        /* Validity still holds */
        assert(aws_byte_buf_is_valid(buf));
    }
    
    /* 6. Result is a bool — must be true or false */
    assert(result == true || result == false);
}

void aws_string_eq_byte_buf_harness(void) {
    aws_string_eq_byte_buf_harness();
    return 0;
}
