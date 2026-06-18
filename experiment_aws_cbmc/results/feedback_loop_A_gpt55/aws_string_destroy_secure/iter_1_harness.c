#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
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

void aws_string_destroy_secure_harness() {
    if (nondet_bool()) {
        struct aws_string *str = NULL;
        aws_string_destroy_secure(str);
        assert(str == NULL);
        return;
    }

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len < SIZE_MAX - offsetof(struct aws_string, bytes) - 1);

    size_t allocation_size = offsetof(struct aws_string, bytes) + len + 1;
    bool dynamically_allocated = nondet_bool();

    struct aws_allocator *allocator = dynamically_allocated ? aws_default_allocator() : NULL;
    __CPROVER_assume(!dynamically_allocated || allocator != NULL);

    void *allocation = dynamically_allocated ? aws_mem_acquire(allocator, allocation_size) : malloc(allocation_size);
    __CPROVER_assume(allocation != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(allocation, allocation_size));

    struct aws_string_mutable_for_harness *mutable_str = (struct aws_string_mutable_for_harness *)allocation;
    mutable_str->allocator = allocator;
    mutable_str->len = len;

    for (size_t i = 0; i < len; ++i) {
        mutable_str->bytes[i] = nondet_uint8_t();
    }
    mutable_str->bytes[len] = 0;

    struct aws_string *str = (struct aws_string *)mutable_str;

    __CPROVER_assume(aws_string_is_valid(str));
    __CPROVER_assume(AWS_MEM_IS_WRITABLE((void *)aws_string_bytes(str), str->len));

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    const uint8_t *old_bytes = aws_string_bytes(str);
    uint8_t old_null_terminator = old_bytes[old_len];

    aws_string_destroy_secure(str);

    if (dynamically_allocated) {
        assert(old_allocator == allocator);
        assert(old_allocator != NULL);
        assert(old_len == len);
        assert(__CPROVER_was_freed(str));
    } else {
        assert(str->allocator == old_allocator);
        assert(str->allocator == NULL);
        assert(str->len == old_len);
        assert(str->len == len);
        assert(aws_string_bytes(str) == old_bytes);

        for (size_t i = 0; i < len; ++i) {
            assert(aws_string_bytes(str)[i] == 0);
        }

        assert(aws_string_bytes(str)[str->len] == old_null_terminator);
        assert(aws_string_bytes(str)[str->len] == 0);
        assert(aws_string_is_valid(str));

        free(mutable_str);
    }
}
