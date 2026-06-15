#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    /* Default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondet aws_string (non‑NULL) */
    struct aws_string *str = make_aws_string(allocator);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));
    __CPROVER_assume(str->allocator != NULL);
    __CPROVER_assume(str->allocator->mem_release != NULL);

    /* Nondet aws_byte_buf (non‑NULL) */
    struct aws_byte_buf *buf = make_aws_byte_buf(allocator);
    __CPROVER_assume(buf != NULL);
    __CPROVER_assume(aws_byte_buf_is_valid(buf));
    __CPROVER_assume(buf->allocator != NULL);
    __CPROVER_assume(buf->allocator->mem_release != NULL);

    /* Constrain lengths to be within allocated capacity */
    size_t max_len = 64; /* arbitrary bound for verification */
    size_t len = __CPROVER_nondet_uint();
    __CPROVER_assume(len <= max_len);
    __CPROVER_assume(len <= str->len);
    __CPROVER_assume(len <= buf->capacity);
    str->len = len;
    buf->len = len;

    /* Ensure buffers are readable for the chosen length */
    __CPROVER_assume(str->bytes != NULL || len == 0);
    __CPROVER_assume(buf->buffer != NULL || len == 0);

    /* Call function under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Specification */
    bool expected = false;
    if (str->len == buf->len) {
        expected = (memcmp(str->bytes, buf->buffer, str->len) == 0);
    }
    assert(result == expected);
}
