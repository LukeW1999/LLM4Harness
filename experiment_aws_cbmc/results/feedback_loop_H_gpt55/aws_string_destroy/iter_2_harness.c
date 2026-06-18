#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);
extern int nondet_int(void);

void aws_string_destroy_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    struct aws_string *str = NULL;
    size_t len = nondet_size_t();

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len <= SIZE_MAX - sizeof(struct aws_string) - 1);

    size_t allocation_size = sizeof(struct aws_string) + len + 1;

    int use_null_string = nondet_int();
    int use_dynamic_string = nondet_int();

    if (!use_null_string) {
        str = aws_mem_acquire(allocator, allocation_size);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(__CPROVER_w_ok(str, allocation_size));

        struct aws_allocator **allocator_field = (struct aws_allocator **)(void *)&str->allocator;
        size_t *len_field = (size_t *)(void *)&str->len;
        uint8_t *bytes = (uint8_t *)(void *)str->bytes;

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

    aws_string_destroy(str);

    if (old_str == NULL) {
        assert(str == NULL);
    } else if (old_allocator == NULL) {
        assert(str == old_str);
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert(aws_string_is_valid(str));

        aws_mem_release(allocator, str);
    } else {
        assert(str == old_str);
        assert(old_allocator == allocator);
        assert(old_len == len);
    }
}
