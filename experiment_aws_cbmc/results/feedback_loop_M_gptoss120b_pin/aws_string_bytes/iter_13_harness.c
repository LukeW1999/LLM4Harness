#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_bytes_harness(void) {
    size_t len = nondet_uint64_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    struct {
        struct aws_string s;
        uint8_t bytes[MAX_BUFFER_SIZE];
    } container;

    struct aws_string *s = &container.s;

    s->allocator = aws_default_allocator();
    __CPROVER_assume(s->allocator != 0);
    s->len = len;

    struct aws_allocator *old_allocator = s->allocator;
    size_t old_len = s->len;

    const uint8_t *ptr = aws_string_bytes(s);

    __CPROVER_assert(ptr == container.bytes, "ptr points to bytes");
    __CPROVER_assert(ptr != 0, "ptr not null");
    __CPROVER_assert(s->allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(s->len == old_len, "len unchanged");
}
