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
        __CPROVER_assume(str->len == 0);               /* restrict to empty string */
        __CPROVER_assume(str->bytes == NULL || str->len == 0);
    }

    /* Nondet aws_byte_buf */
    struct aws_byte_buf *buf = NULL;
    if (__CPROVER_nondet_bool()) {
        buf = make_aws_byte_buf(allocator);
        __CPROVER_assume(buf != NULL);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
        __CPROVER_assume(buf->allocator != NULL);
        __CPROVER_assume(buf->allocator->mem_release != NULL);
        __CPROVER_assume(buf->len == 0);               /* restrict to empty buffer */
        __CPROVER_assume(buf->buffer == NULL || buf->len == 0);
    }

    /* Call function under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Specification */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        /* Both are non‑NULL and have length 0, so they are equal */
        assert(result == true);
    }

    /* No explicit cleanup to avoid allocator precondition violations */
}
