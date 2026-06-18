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
 * 1. Changed fields: none (pure accessor, returns pointer to str->bytes)
 * 2. Unchanged fields: all fields of str remain unchanged
 * 3. Failure: no failure path (assumes valid input)
 * 4. Validity invariants: str remains valid after the call
 *
 * Postconditions:
 *   - result == str->bytes
 *   - str fields are unchanged
 *   - str is still valid
 */
void aws_string_bytes_harness(void) {
    /* 1. Allocate and set up a valid aws_string */
    struct aws_string *str = (struct aws_string *)malloc(sizeof(struct aws_string) + 1);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    const uint8_t *old_bytes_ptr = str->bytes;

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */

    /* The return value must equal str->bytes */
    assert(result == str->bytes);

    /* All fields of str must be unchanged */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(str->bytes == old_bytes_ptr);

    /* Validity invariant must still hold */
    assert(aws_string_is_valid(str));
}
