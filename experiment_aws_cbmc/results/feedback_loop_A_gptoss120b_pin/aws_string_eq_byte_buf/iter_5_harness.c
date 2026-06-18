#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str;
    struct aws_byte_buf *buf;

    /* nondet decide if str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        uint8_t *data = malloc(len);
        __CPROVER_assume(data != NULL);
        str = aws_string_new_from_array(aws_default_allocator(), data, len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondet decide if buf is NULL */
    if (nondet_bool()) {
        buf = NULL;
    } else {
        struct aws_byte_buf b;
        b.allocator = aws_default_allocator();

        /* allocate a nondet capacity */
        b.capacity = nondet_size_t();
        __CPROVER_assume(b.capacity <= MAX_BUFFER_SIZE);

        if (b.capacity > 0) {
            b.buffer = malloc(b.capacity);
            __CPROVER_assume(b.buffer != NULL);
        } else {
            b.buffer = NULL;
        }

        /* set a nondet length that is within capacity */
        b.len = nondet_size_t();
        __CPROVER_assume(b.len <= b.capacity);

        __CPROVER_assume(aws_byte_buf_is_valid(&b));
        buf = &b;
    }

    /* Save old state */
    struct aws_string old_str;
    struct aws_byte_buf old_buf;
    if (str != NULL) {
        old_str = *str;
    }
    if (buf != NULL) {
        old_buf = *buf;
    }

    /* Preconditions for the function under test */
    __CPROVER_assume(!buf || aws_byte_buf_is_valid(buf));

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Post‑condition on return value */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
        assert(result == expected);
    }

    /* Unchanged fields (function is pure) */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
    }
    if (buf != NULL) {
        assert(buf->allocator == old_buf.allocator);
        assert(buf->capacity == old_buf.capacity);
        assert(buf->len == old_buf.len);
        assert(buf->buffer == old_buf.buffer);
    }

    /* Validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
