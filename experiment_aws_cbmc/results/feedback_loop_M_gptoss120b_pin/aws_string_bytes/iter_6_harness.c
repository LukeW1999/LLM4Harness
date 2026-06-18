#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_string_bytes_harness(void) {
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    union {
        struct aws_string s;
        uint8_t buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    } storage;

    struct aws_string *s = &storage.s;

    struct aws_allocator *allocator = aws_default_allocator();
    s->allocator = allocator;
    s->len = len;

    __CPROVER_assume(aws_string_is_valid(s));

    struct aws_string old = *s;

    const uint8_t *ptr = aws_string_bytes(s);

    assert(ptr == s->bytes);
    assert(ptr != NULL);
    assert(s->allocator == old.allocator);
    assert(s->len == old.len);
    assert(aws_string_is_valid(s));
}
