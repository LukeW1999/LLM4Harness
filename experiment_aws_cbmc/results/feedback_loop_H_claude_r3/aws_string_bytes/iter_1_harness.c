#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_bytes_harness(void) {
    /* 1. Declare and set up a valid aws_string */
    /* We need to allocate an aws_string with some bytes following it.
     * Use nondet size for the string length, bounded to keep state space manageable. */
    size_t len;
    __CPROVER_assume(len <= 10); /* bound the length */

    /* Allocate memory for the aws_string header plus len+1 bytes (for null terminator) */
    /* The struct has bytes[1] as flexible array member placeholder */
    size_t alloc_size = sizeof(struct aws_string) + len; /* bytes[1] already accounts for 1 byte, so +len gives len+1 total */
    struct aws_string *str = malloc(alloc_size);
    __CPROVER_assume(str != NULL);

    /* Set up the string fields - they are const so we use a cast trick */
    /* We'll use a writable struct to initialize */
    struct {
        struct aws_allocator *allocator;
        size_t len;
        uint8_t bytes[11]; /* max len+1 */
    } *mutable_str = (void *)str;

    mutable_str->allocator = aws_default_allocator();
    mutable_str->len = len;
    /* bytes are nondet (already nondet from malloc) */

    /* Ensure the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state */
    const struct aws_allocator *old_allocator = str->allocator;
    const size_t old_len = str->len;
    const uint8_t *old_bytes_ptr = str->bytes;

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */
    /* The function returns str->bytes, which is the pointer to the data bytes */
    assert(result == str->bytes);
    assert(result != NULL);

    /* 5. Assert fields that must NOT change */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(str->bytes == old_bytes_ptr);

    /* 6. Assert validity invariant still holds */
    assert(aws_string_is_valid(str));

    /* 7. Assert the result pointer is readable for len bytes */
    /* The result should point to the bytes of the string */
    assert(result == old_bytes_ptr);
}
