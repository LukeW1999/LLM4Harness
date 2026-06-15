#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>

void aws_byte_cursor_from_string_harness() {
    /* Allocate a valid string with a non-deterministic length */
    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);
    const struct aws_string *src = allocate_aws_string(len);
    __CPROVER_assume(src != NULL && aws_string_is_valid(src));

    /* Call the function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* Postconditions */
    assert(cursor.ptr == aws_string_bytes(src));
    assert(cursor.len == src->len);
    assert(AWS_MEM_IS_READABLE(cursor.ptr, cursor.len));

    /* Validity invariant of the returned cursor */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Clean up */
    free((void *)src);
}
