#include <stdlib.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>

void aws_string_eq_byte_buf_harness(void) {
    /* ----- Allocate a non‑NULL aws_string ----- */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
    uint8_t *str_data = malloc(str_len);
    __CPROVER_assume(str_data != NULL);
    struct aws_string *str = aws_string_new_from_array(aws_default_allocator(), str_data, str_len);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* ----- Allocate a non‑NULL aws_byte_buf ----- */
    struct aws_byte_buf b;
    b.allocator = aws_default_allocator();

    b.capacity = nondet_size_t();
    __CPROVER_assume(b.capacity <= MAX_BUFFER_SIZE);
    if (b.capacity > 0) {
        b.buffer = malloc(b.capacity);
        __CPROVER_assume(b.buffer != NULL);
    } else {
        b.buffer = NULL;
    }

    b.len = nondet_size_t();
    __CPROVER_assume(b.len <= b.capacity);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));
    struct aws_byte_buf *buf = &b;

    /* ----- Save old state for purity checks ----- */
    struct aws_string old_str = *str;
    struct aws_byte_buf old_buf = *buf;

    /* ----- Preconditions ----- */
    __CPROVER_assume(aws_string_is_valid(str));
    __CPROVER_assume(aws_byte_buf_is_valid(buf));

    /* ----- Call function under test ----- */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* ----- Expected result ----- */
    bool expected = aws_array_eq(str->bytes, str->len, buf->buffer, buf->len);
    assert(result == expected);

    /* ----- Purity: unchanged fields ----- */
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    assert(buf->allocator == old_buf.allocator);
    assert(buf->capacity == old_buf.capacity);
    assert(buf->len == old_buf.len);
    assert(buf->buffer == old_buf.buffer);

    /* ----- Validity invariants after the call ----- */
    assert(aws_string_is_valid(str));
    assert(aws_byte_buf_is_valid(buf));
}
