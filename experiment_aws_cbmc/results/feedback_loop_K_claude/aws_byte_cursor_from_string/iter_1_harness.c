#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    /* Non-deterministic choice: NULL or valid aws_string */
    bool use_null;
    __CPROVER_assume(true); /* no constraint needed, use_null is free */

    const struct aws_string *src;

    if (use_null) {
        src = NULL;
    } else {
        /* Allocate a valid aws_string with non-deterministic length */
        src = make_arbitrary_aws_string(aws_default_allocator());
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Save state before call for frame condition checks */
    size_t old_len = (src != NULL) ? src->len : 0;
    const uint8_t *old_bytes = (src != NULL) ? aws_string_bytes(src) : NULL;

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions: validity */
    assert(aws_byte_cursor_is_valid(&result));

    if (src == NULL) {
        /* NULL input: cursor must be zeroed */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* Non-NULL input: cursor must point to string bytes with correct length */
        assert(result.ptr == old_bytes);
        assert(result.len == old_len);

        /* Frame condition: src was not modified */
        assert(src->len == old_len);
        assert(aws_string_bytes(src) == old_bytes);
        assert(aws_string_is_valid(src));
    }

    /* Postconditions: length invariant */
    if (src != NULL) {
        assert(result.len == src->len);
    } else {
        assert(result.len == 0);
    }
}
