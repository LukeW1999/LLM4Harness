#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness() {
    /* Non-deterministic input */
    const struct aws_string *src = nondet_bool() ? NULL : malloc(sizeof(struct aws_string));
    if (src) {
        /* Ensure src is valid */
        __CPROVER_assume(aws_string_is_valid(src));
        /* Ensure the bytes array is readable */
        __CPROVER_assume(AWS_MEM_IS_READABLE(src->bytes, src->len));
    }

    /* Call the function */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions */
    if (src == NULL) {
        /* If src is NULL, return an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* Otherwise, result should match the string's bytes and length */
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
        /* The cursor must be valid */
        assert(aws_byte_cursor_is_valid(&result));
    }
}
