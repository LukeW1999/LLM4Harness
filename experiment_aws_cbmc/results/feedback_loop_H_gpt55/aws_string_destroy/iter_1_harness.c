#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    struct aws_string *str = NULL;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len <= SIZE_MAX - sizeof(struct aws_string));

    size_t allocation_size = sizeof(struct aws_string) + len;

    bool use_null_string = nondet_bool();
    bool use_dynamic_string = nondet_bool();

    if (!use_null_string) {
        str = aws_mem_acquire(allocator, allocation_size);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(str, allocation_size));

        struct aws_allocator **allocator_field = (struct aws_allocator **)(void *)&str->allocator;
        size_t *len_field = (size_t *)(void *)&str->len;
        uint8_t *bytes = (uint8_t *)(void *)((uint8_t *)str + offsetof(struct aws_string, bytes));

        *allocator_field = use_dynamic_string ? allocator : NULL;
        *len_field = len;

        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
        bytes[len] = 0;

        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = str ? str->allocator : NULL;
    size_t old_len = str ? str->len : 0;
    size_t old_allocation_size = allocation_size;

    struct store_byte_from_buffer old_byte;
    if (str != NULL) {
        save_byte_from_array(str->bytes, old_len + 1, &old_byte);
    }

    aws_string_destroy(str);

    if (old_str == NULL) {
        assert(str == NULL);
    } else if (old_allocator == NULL) {
        assert(str == old_str);
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert_byte_from_buffer_matches(str->bytes, &old_byte);
        assert(aws_string_is_valid(str));

        aws_mem_release(allocator, str);
    } else {
        assert(str == old_str);
        assert(old_allocator == allocator);
        assert(old_len == len);
        assert(!AWS_MEM_IS_READABLE(str, old_allocation_size));
    }
}
