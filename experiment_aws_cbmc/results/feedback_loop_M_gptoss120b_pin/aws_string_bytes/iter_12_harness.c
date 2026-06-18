#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_bytes_harness(void) {
    size_t len = nondet_uint64_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t raw[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *s = (struct aws_string *)raw;

    s->allocator = aws_default_allocator();
    s->len = len;

    __CPROVER_assume(s->allocator != 0);

    struct aws_allocator *old_allocator = s->allocator;
    size_t old_len = s->len;

    const uint8_t *ptr = aws_string_bytes(s);

    __CPROVER_assert(ptr == raw + sizeof(struct aws_string), "ptr points to bytes");
    __CPROVER_assert(ptr != 0, "ptr not null");
    __CPROVER_assert(s->allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(s->len == old_len, "len unchanged");
}
