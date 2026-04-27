#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

/*
 * aws_string_bytes simply returns str->bytes (the pointer to the bytes array
 * inside the aws_string struct). It does not modify anything.
 *
 * Preconditions:
 *   - str must be a valid aws_string (non-NULL, readable)
 *
 * Postconditions:
 *   - The returned pointer equals str->bytes
 *   - The function does not modify str in any way
 */

void aws_string_bytes_harness(void) {
    /* 1. Declare and set up a valid aws_string */
    /* We need a non-deterministic aws_string. Since aws_string has a flexible
     * array member (bytes[1]), we allocate a concrete struct with some bytes. */
    
    /* Use a bounded length for the string */
    size_t len;
    __CPROVER_assume(len <= 16); /* bound the string length */
    
    /* Allocate memory for the aws_string with len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);
    
    /* Set up the string fields - cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = NULL; /* allocator can be NULL for static strings */
    *(size_t *)&str->len = len;
    /* bytes are non-deterministic (already set by malloc) */
    /* Ensure null terminator */
    ((uint8_t *)str->bytes)[len] = 0;
    
    /* Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));
    
    /* 2. Save old state */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    const uint8_t *old_bytes_ptr = str->bytes;
    
    /* 3. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);
    
    /* 4. Assert postconditions */
    
    /* The function returns str->bytes */
    assert(result == str->bytes);
    
    /* The function returns the same pointer as the internal bytes array */
    assert(result == old_bytes_ptr);
    
    /* The function does not modify any fields of str */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(str->bytes == old_bytes_ptr);
    
    /* The result pointer is readable (at least len+1 bytes for null terminator) */
    __CPROVER_assert(AWS_MEM_IS_READABLE(result, old_len + 1), 
                     "result pointer is readable for len+1 bytes");
    
    /* The string remains valid after the call */
    assert(aws_string_is_valid(str));
    
    /* The null terminator is preserved */
    assert(result[old_len] == 0);
}
