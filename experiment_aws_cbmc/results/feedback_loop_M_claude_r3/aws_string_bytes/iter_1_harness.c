#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_bytes_harness() {
    /* 1. Allocate and set up a valid aws_string */
    /* We need to allocate memory for the struct + bytes inline.
     * Use a bounded length for the string. */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate memory for the aws_string header + len bytes + 1 null terminator */
    /* The struct has bytes[1] as flexible-array-like member, so total size is:
     * sizeof(struct aws_string) + len bytes (the first byte is already in the struct) */
    size_t total_size = sizeof(struct aws_string) + len;
    __CPROVER_assume(total_size >= sizeof(struct aws_string));
    __CPROVER_assume(total_size <= sizeof(struct aws_string) + MAX_BUFFER_SIZE);

    struct aws_string *str = malloc(total_size);
    __CPROVER_assume(str != NULL);

    /* Set the length field (it's const, so we use a trick via pointer cast) */
    /* We need to initialize the struct properly */
    /* Since fields are const, we initialize via a writable copy of the memory */
    struct aws_allocator *allocator = nondet_bool() ? aws_default_allocator() : NULL;
    
    /* Write to the const fields using memcpy-style approach */
    /* Cast away const to initialize */
    *(struct aws_allocator **)&str->allocator = allocator;
    *(size_t *)&str->len = len;
    /* bytes are already allocated as part of the struct memory */

    /* Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state */
    const struct aws_string *old_str = str;
    size_t old_len = str->len;
    struct aws_allocator *old_allocator = str->allocator;
    const uint8_t *old_bytes_ptr = str->bytes;

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */
    /* The function returns str->bytes, which is the address of the bytes field */
    assert(result == str->bytes);
    assert(result != NULL);
    assert(result == old_bytes_ptr);

    /* 5. Assert unchanged fields */
    assert(str->len == old_len);
    assert(str->allocator == old_allocator);
    assert(str == old_str);

    /* 6. Assert validity invariant still holds */
    assert(aws_string_is_valid(str));
}
