#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

void aws_string_bytes_harness(void) {
    /* Allocate a valid aws_string using the standard proof helper */
    struct aws_string *str = ensure_string_is_allocated_nondet_length();
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old state before the call */
    const struct aws_allocator *old_allocator = str->allocator;
    const size_t old_len = str->len;

    /* Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* Assert postconditions */

    /* The return value must equal str->bytes */
    assert(result == str->bytes);

    /* All fields of str must remain unchanged */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);

    /* Validity invariant must still hold */
    assert(aws_string_is_valid(str));
}
