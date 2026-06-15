#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness() {
    /* Non-deterministic choice of NULL or a valid string */
    const struct aws_string *src;
    bool is_null = nondet_bool();
    if (is_null) {
        src = NULL;
    } else {
        /* Allocate a nondet-length string */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        src = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(src != NULL);
        /* Make it a valid string */
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Call the function */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions */
    if (src == NULL) {
        assert(result.ptr == NULL);
        assert(result.len == 0);
        assert(aws_byte_cursor_is_valid(&result));
    } else {
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
        assert(aws_byte_cursor_is_valid(&result));
    }
}
