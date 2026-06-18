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

    struct aws_allocator *old_allocator = s->allocator;
    size_t old_len = s->len;

    const uint8_t *ptr = aws_string_bytes(s);

    __CPROVER_assert(ptr == s_wrapper.bytes, "ptr equals s_wrapper.bytes");
    __CPROVER_assert(ptr != NULL, "ptr not null");
    __CPROVER_assert(s->allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(s->len == old_len, "len unchanged");
    __CPROVER_assert(aws_string_is_valid(s), "string still valid");
}
