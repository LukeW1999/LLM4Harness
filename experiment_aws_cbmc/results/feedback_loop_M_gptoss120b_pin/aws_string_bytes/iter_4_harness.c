#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_string_bytes_harness(void) {
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *s = (struct aws_string *)storage;

    s->allocator = aws_default_allocator();
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
