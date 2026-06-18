#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Harness for aws_string_bytes:
 *   const uint8_t *aws_string_bytes(const struct aws_string *str)
 *
 * Doxygen: "Equivalent to str->bytes."
 *
 * Analysis:
 * 1. Changed fields: none — this is a pure accessor.
 * 2. Unchanged fields: all fields of str (allocator, len, bytes).
 * 3. Failure path: N/A — no failure path; always returns str->bytes.
 * 4. Validity invariants: str remains valid after the call.
 * 5. Return value: must equal str->bytes.
 */

void aws_string_bytes_harness(void) {
    /* 1. Allocate a valid aws_string non-deterministically.
     *    aws_string has a flexible-array-like member; we allocate enough
     *    memory for the header plus at least 1 byte (the mandatory null
     *    terminator stored in bytes[0]).
     *
     *    We use a bounded length to keep the state space finite.
     */
    size_t len;
    __CPROVER_assume(len <= 16); /* bound the string length */

    /* Allocate raw memory for the struct plus len+1 bytes (data + null term) */
    size_t alloc_size = sizeof(struct aws_string) + len; /* bytes[1] already counts 1 byte */
    struct aws_string *str = malloc(alloc_size);
    __CPROVER_assume(str != NULL);

    /* Fill the const fields via a writable alias */
    struct aws_string *mutable_str = (struct aws_string *)str;

    /* Set len (write through the const via memcpy trick or direct cast) */
    *(size_t *)(&mutable_str->len) = len;

    /* allocator may be NULL (static string) or non-NULL */
    /* leave allocator non-deterministic */

    /* Ensure the string is valid per aws_string_is_valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Snapshot the state before the call */
    const struct aws_allocator *old_allocator = str->allocator;
    const size_t old_len = str->len;
    const uint8_t *old_bytes_ptr = str->bytes; /* pointer to the bytes array */

    /* 3. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */

    /* RETURN VALUE: must equal str->bytes */
    assert(result == str->bytes);
    assert(result == old_bytes_ptr);

    /* FRAME: no fields of str should have changed */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(str->bytes == old_bytes_ptr);

    /* INVARIANTS: str is still valid */
    assert(aws_string_is_valid(str));

    /* The returned pointer must be readable (points into the string's data) */
    /* result is non-NULL because str->bytes is an array member (always valid) */
    assert(result != NULL);
}
