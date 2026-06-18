#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_bytes_harness() {
    /* 1. Allocate and set up a valid aws_string */
    /* We need to allocate memory for the struct + bytes inline.
     * Use a fixed-size buffer to represent the string. */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate memory for the string header + len + 1 (null terminator) bytes */
    size_t total_size = sizeof(struct aws_string) + len; /* bytes[1] already in struct, so +len covers len+1 total */
    /* Actually: struct aws_string has bytes[1], so total = sizeof(struct aws_string) + len bytes extra */
    /* The struct has bytes[1], so total allocation = sizeof(struct aws_string) + len */
    /* This gives us bytes[0..len] where bytes[len] is the null terminator */

    struct aws_string *str = malloc(total_size);
    __CPROVER_assume(str != NULL);

    /* Set up the string fields - we need to write through const pointers */
    /* Use a writable version to initialize */
    struct aws_string *mutable_str = str;
    
    /* Initialize allocator field (const, so use memcpy trick or direct assignment via cast) */
    struct aws_allocator **alloc_ptr = (struct aws_allocator **)&mutable_str->allocator;
    *alloc_ptr = aws_default_allocator();
    
    /* Initialize len field */
    size_t *len_ptr = (size_t *)&mutable_str->len;
    *len_ptr = len;
    
    /* Initialize bytes - make them nondet but ensure null terminator */
    uint8_t *bytes_ptr = (uint8_t *)mutable_str->bytes;
    /* bytes are nondet by default from malloc + CBMC nondet memory */
    /* Ensure null terminator at position len */
    bytes_ptr[len] = 0;

    /* Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 3. Assert postconditions */
    /* The function returns str->bytes, which is the pointer to the bytes array */
    assert(result == str->bytes);
    
    /* The result should not be NULL since str is valid and bytes is embedded in the struct */
    assert(result != NULL);
    
    /* The string fields should be unchanged */
    assert(str->len == len);
    assert(str->allocator == aws_default_allocator());
    
    /* The result points to readable memory of at least len+1 bytes */
    /* (null terminator is always present) */
    __CPROVER_assert(AWS_MEM_IS_READABLE(result, len + 1), "result is readable for len+1 bytes");
    
    /* The null terminator is at position len */
    assert(result[len] == 0);
    
    /* Validity invariant still holds */
    assert(aws_string_is_valid(str));
}
