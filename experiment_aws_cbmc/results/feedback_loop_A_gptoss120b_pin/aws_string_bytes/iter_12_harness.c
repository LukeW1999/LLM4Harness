#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    const size_t max_len = 256;
    size_t len = nondet_size_t();
    __CPROVER_assume(len < max_len); /* leave space for null terminator */

    union {
        struct aws_string s;
        uint8_t raw[sizeof(struct aws_string) + max_len + 1];
    } u;
    struct aws_string *s = &u.s;

    struct aws_allocator *allocator = aws_default_allocator();
    s->allocator = allocator;
    s->len = len;

    uint8_t *bytes = s->bytes;
    for (size_t i = 0; i < len; ++i) {
        bytes[i] = nondet_uint8_t();
    }
    bytes[len] = 0; /* null terminator */

    __CPROVER_assume(aws_string_is_valid(s));

    struct aws_string old = *s;                     /* shallow copy of fixed fields */
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(bytes, s->len, &old_bytes);

    const uint8_t *result = aws_string_bytes(s);

    assert(result == s->bytes);
    assert(s->allocator == old.allocator);
    assert(s->len == old.len);
    assert_byte_from_buffer_matches(bytes, &old_bytes);
    assert(aws_string_is_valid(s));
}
