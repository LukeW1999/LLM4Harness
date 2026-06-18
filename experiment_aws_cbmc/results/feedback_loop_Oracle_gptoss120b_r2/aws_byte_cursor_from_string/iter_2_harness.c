#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* stack‑allocated aws_string with flexible array member */
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_STRING_LEN];
    } src_buf;

    struct aws_string *src = &src_buf.s;
    src->allocator = NULL;
    src->len = len;

    for (size_t i = 0; i < len; ++i) {
        src->bytes[i] = nondet_uint8();
    }

    __CPROVER_assume(aws_string_is_valid(src));

    /* snapshot original state */
    struct aws_string src_snapshot = *src;
    uint8_t bytes_snapshot[MAX_STRING_LEN];
    memcpy(bytes_snapshot, src->bytes, len);

    /* call function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_string(src);

    /* post‑conditions */
    assert(cur.ptr == aws_string_bytes(src));
    assert(cur.len == src->len);
    assert(src->allocator == src_snapshot.allocator);
    assert(src->len == src_snapshot.len);
    assert(memcmp(src->bytes, bytes_snapshot, len) == 0);
}
