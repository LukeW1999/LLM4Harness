#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_bytes_harness(void) {
    /* Allocate a non-deterministic aws_string */
    struct aws_string *str = ensure_string_is_allocated_nondet_length();

    /* Precondition: str must be non-null and valid */
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save state before call for frame condition checks */
    const size_t old_len = str->len;
    struct aws_allocator *const old_allocator = str->allocator;
    const uint8_t *const expected_bytes = str->bytes;

    /* Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* Postcondition: return value is non-null */
    assert(result != NULL);

    /* Postcondition: return value equals str->bytes */
    assert(result == str->bytes);

    /* Postcondition: return value equals the expected bytes pointer */
    assert(result == expected_bytes);

    /* Postcondition (frame): str->len is unchanged */
    assert(str->len == old_len);

    /* Postcondition (frame): str->allocator is unchanged */
    assert(str->allocator == old_allocator);

    /* Postcondition: the string is still valid after the call */
    assert(aws_string_is_valid(str));
}
