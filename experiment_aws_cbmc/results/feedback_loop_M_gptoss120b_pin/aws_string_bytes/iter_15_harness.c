#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_string_bytes_harness(void) {
    uint64_t nondet_len = nondet_uint64_t();
    size_t len = (size_t)nondet_len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *s = (struct aws_string *)buffer;
    uint8_t *bytes = buffer + sizeof(struct aws_string);

    s->allocator = aws_default_allocator();
    s->len = len;

    struct aws_allocator *old_allocator = s->allocator;
    size_t old_len = s->len;

    const uint8_t *ptr = aws_string_bytes(s);

    __CPROVER_assert(ptr == bytes, "ptr points to bytes");
    __CPROVER_assert(ptr != 0, "ptr not null");
    __CPROVER_assert(s->allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(s->len == old_len, "len unchanged");
}
