#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    /* Nondeterministically choose whether to pass NULL or a valid string */
    bool is_null;
    __CPROVER_assume(true); /* no-op, just for clarity */

    struct aws_string *str;

    if (is_null) {
        str = NULL;
    } else {
        /* Create a valid aws_string */
        str = nondet_allocate_aws_string_bounded(MAX_STRING_LEN);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Save state before call for frame condition checks */
    size_t old_len = str ? str->len : 0;
    const uint8_t *old_bytes = str ? aws_string_bytes(str) : NULL;

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(str);

    /* Postcondition 1: Return value validity */
    if (str == NULL) {
        /* If src is NULL, result should be an empty/zeroed cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is valid, result should point to the string's bytes with correct length */
        assert(aws_byte_cursor_is_valid(&result));

        /* Postcondition 2: Output buffer length invariant */
        assert(result.len == str->len);

        /* Postcondition 3: The cursor points to the string's bytes */
        assert(result.ptr == aws_string_bytes(str));

        /* Frame condition: string was not modified */
        assert(str->len == old_len);
        assert(aws_string_bytes(str) == old_bytes);
        assert(aws_string_is_valid(str));
    }
}
