#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* Default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondet aws_string */
    struct aws_string *str = NULL;
    if (__CPROVER_nondet_bool()) {
        str = make_aws_string(allocator);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
        __CPROVER_assume(str->allocator != NULL);
        __CPROVER_assume(str->allocator->mem_release != NULL);
        __CPROVER_assume(str->len == 0 || str->bytes != NULL);
    }

    /* Nondet aws_byte_buf */
    struct aws_byte_buf *buf = NULL;
    if (__CPROVER_nondet_bool()) {
        buf = make_aws_byte_buf(allocator);
        __CPROVER_assume(buf != NULL);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
        __CPROVER_assume(buf->allocator != NULL);
        __CPROVER_assume(buf->allocator->mem_release != NULL);
        __CPROVER_assume(buf->len == 0 || buf->buffer != NULL);
    }

    /* Call function under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Specification */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        bool expected = false;
        if (str->len == buf->len) {
            if (str->len == 0) {
                expected = true;
            } else {
                expected = (memcmp(str->bytes, buf->buffer, str->len) == 0);
            }
        }
        assert(result == expected);
    }

    /* Clean up */
    if (str) aws_string_destroy(str);
    if (buf) aws_byte_buf_clean_up(buf);
}
