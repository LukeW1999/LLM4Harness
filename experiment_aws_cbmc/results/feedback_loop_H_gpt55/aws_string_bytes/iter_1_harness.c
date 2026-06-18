#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_mutable_for_harness {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[1];
};

void aws_string_bytes_harness(void) {
    size_t len = nondet_size_t();

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len < SIZE_MAX);
    __CPROVER_assume(len <= SIZE_MAX - sizeof(struct aws_string_mutable_for_harness));

    size_t allocation_size = sizeof(struct aws_string_mutable_for_harness) + len;

    struct aws_string_mutable_for_harness *mutable_str =
        (struct aws_string_mutable_for_harness *)malloc(allocation_size);
    __CPROVER_assume(mutable_str != NULL);

    mutable_str->allocator = aws_default_allocator();
    mutable_str->len = len;

    for (size_t i = 0; i < len; ++i) {
        mutable_str->bytes[i] = nondet_uint8_t();
    }
    mutable_str->bytes[len] = 0;

    const struct aws_string *str = (const struct aws_string *)mutable_str;

    __CPROVER_assume(AWS_MEM_IS_READABLE(str, sizeof(struct aws_string)));
    __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len + 1));
    __CPROVER_assume(aws_string_is_valid(str));

    const struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    const uint8_t *old_bytes = str->bytes;

    uint8_t *old_bytes_copy = (uint8_t *)malloc(old_len + 1);
    __CPROVER_assume(old_bytes_copy != NULL);

    for (size_t i = 0; i <= old_len; ++i) {
        old_bytes_copy[i] = str->bytes[i];
    }

    const uint8_t *result = aws_string_bytes(str);

    assert(result == str->bytes);
    assert(result == old_bytes);
    assert(result != NULL);

    assert(str == old_str);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(str->bytes == old_bytes);

    for (size_t i = 0; i <= old_len; ++i) {
        assert(str->bytes[i] == old_bytes_copy[i]);
    }

    assert(str->bytes[str->len] == 0);
    assert(AWS_MEM_IS_READABLE(result, str->len + 1));
    assert(aws_string_is_valid(str));
}

void aws_string_bytes_harness(void) {
    aws_string_bytes_harness();
    return 0;
}
