#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    /* Create a valid aws_string */
    struct aws_string *str;
    str = nondet_allocate_aws_string_bounded(MAX_STRING_LEN);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save state before call for frame condition checks */
    size_t old_len = str->len;
    const uint8_t *old_bytes = aws_string_bytes(str);

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(str);

    /* Postcondition 1: Return value validity */
    assert(aws_byte_cursor_is_valid(&result));

    /* Postcondition 2: Output buffer length invariant */
    assert(result.len == old_len);

    /* Postcondition 3: The cursor points to the string's bytes */
    assert(result.ptr == old_bytes);

    /* Frame condition: string was not modified */
    assert(aws_string_is_valid(str));
}
