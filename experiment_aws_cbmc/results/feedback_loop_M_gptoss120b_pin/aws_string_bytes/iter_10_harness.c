#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_string_bytes_harness(void) {
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    struct {
        struct aws_string str;
        uint8_t bytes[MAX_BUFFER_SIZE];
    } s_wrapper;

    struct aws_string *s = &s_wrapper.str;

    struct aws_allocator *allocator = aws_default_allocator();
    s->allocator = allocator;
    s->len = len;

    __CPROVER_assume(aws_string_is_valid(s));

    struct aws_string old = *s;

    const uint8_t *ptr = aws_string_bytes(s);

    __CPROVER_assert(ptr == s->bytes, "ptr equals s->bytes");
    __CPROVER_assert(ptr != NULL, "ptr not null");
    __CPROVER_assert(s->allocator == old.allocator, "allocator unchanged");
    __CPROVER_assert(s->len == old.len, "len unchanged");
    __CPROVER_assert(aws_string_is_valid(s), "string still valid");
}
