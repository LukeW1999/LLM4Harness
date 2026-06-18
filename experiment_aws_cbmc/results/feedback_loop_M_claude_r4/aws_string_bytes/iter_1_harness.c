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
     * Use a fixed-size buffer to represent the aws_string with some bytes. */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate memory for aws_string header + len bytes + 1 null terminator */
    /* The struct has bytes[1] as flexible-array-like member */
    size_t total_size = sizeof(struct aws_string) + len; /* bytes[1] already accounts for 1 byte */
    uint8_t *raw = malloc(total_size);
    __CPROVER_assume(raw != NULL);

    struct aws_string *str = (struct aws_string *)raw;

    /* Set the length field (const, so we use a trick) */
    *((size_t *)&str->len) = len;
    /* Set allocator to NULL (static-like) or default */
    *((struct aws_allocator **)&str->allocator) = NULL;

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
    assert(result == old_bytes_ptr);

    /* 5. Assert fields that must NOT change */
    assert(str->len == old_len);
    assert(str->allocator == old_allocator);
    assert(str->bytes == old_bytes_ptr);

    /* 6. Assert validity invariant still holds */
    assert(aws_string_is_valid(str));

    /* 7. Assert result is non-null (bytes field is always part of the struct) */
    assert(result != NULL);
}
