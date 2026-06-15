#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>

void aws_byte_cursor_from_string_harness() {
    /* 1. Declare and bound data structures */
    const struct aws_string *src = nondet_bool() ? NULL : malloc(sizeof(struct aws_string));
    if (src != NULL) {
        /* Bound the length field */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* 2. Call function under test */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    /* 3. Assert postconditions */
    if (src == NULL) {
        /* Function returns an empty cursor when src is NULL */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* Function returns cursor to bytes of string */
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        /* Ensure we can read the bytes */
        __CPROVER_assume(src->len > 0);
        assert(AWS_MEM_IS_READABLE(cursor.ptr, cursor.len));
    }

    /* 4. Assert validity invariants */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 5. Free memory */
    free(src);
}
