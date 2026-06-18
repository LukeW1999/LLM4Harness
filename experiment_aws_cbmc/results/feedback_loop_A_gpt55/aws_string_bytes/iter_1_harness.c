#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_bounded_storage {
    struct aws_allocator *const allocator;
    const size_t len;
    const uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

void aws_string_bytes_harness() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    struct aws_string_bounded_storage storage = {
        NULL,
        len,
        {0},
    };

    const struct aws_string *str = (const struct aws_string *)&storage;

    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    const uint8_t *old_bytes = str->bytes;

    const uint8_t *result = aws_string_bytes(str);

    assert(result == str->bytes);
    assert(result == old_bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(str->bytes == old_bytes);
    assert(aws_string_is_valid(str));
}
