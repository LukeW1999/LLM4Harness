#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_string_bytes_harness(void) {
    size_t len = nondet_uint64_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t buffer[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *str = (struct aws_string *)buffer;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    const uint8_t *result = aws_string_bytes(str);

    assert(result == str->bytes);
    assert(aws_string_is_valid(str));
}
