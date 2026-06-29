#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    /* Allocate a valid aws_string with non-deterministic length */
    const struct aws_string *src = make_arbitrary_aws_string(aws_default_allocator());
    __CPROVER_assume(src != NULL);
    __CPROVER_assume(aws_string_is_valid(src));

    /* Save state before call for frame condition checks */
    size_t old_len = src->len;
    const uint8_t *old_bytes = aws_string_bytes(src);

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions: validity */
    assert(aws_byte_cursor_is_valid(&result));

    /* Non-NULL input: cursor must point to string bytes with correct length */
    assert(result.ptr == old_bytes);
    assert(result.len == old_len);

    /* Frame condition: src was not modified */
    assert(src->len == old_len);
    assert(aws_string_bytes(src) == old_bytes);
    assert(aws_string_is_valid(src));

    /* Postconditions: length invariant */
    assert(result.len == src->len);
}
