#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    struct {
        struct aws_string s;
        uint8_t bytes[MAX_STRING_LEN];
    } src_buf;

    struct aws_string *src = &src_buf.s;
    src->allocator = aws_default_allocator();
    src->len = len;

    for (size_t i = 0; i < len; ++i) {
        src_buf.bytes[i] = nondet_uint8_t();
    }

    __CPROVER_assume(aws_string_is_valid(src));

    struct aws_allocator *alloc_snapshot = src->allocator;
    size_t len_snapshot = src->len;
    uint8_t bytes_snapshot[MAX_STRING_LEN];
    for (size_t i = 0; i < len; ++i) {
        bytes_snapshot[i] = src_buf.bytes[i];
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    assert(cur.ptr == aws_string_bytes(src));
    assert(cur.len == src->len);
    assert(src->allocator == alloc_snapshot);
    assert(src->len == len_snapshot);
    for (size_t i = 0; i < len; ++i) {
        assert(src_buf.bytes[i] == bytes_snapshot[i]);
    }
}
