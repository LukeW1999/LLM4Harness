#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_harness(void) {
    uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);

        str = (struct aws_string *)storage;

        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }

        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
    } else {
        str = NULL;
    }

    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_len = (str != NULL) ? str->len : 0;

    aws_string_destroy(str);

    assert(str == old_str);

    if (old_str != NULL && old_allocator != NULL) {
        /* memory was released – no further checks */
    } else if (old_str != NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        if (old_len > 0) {
            assert(((uint8_t *)str->bytes)[0] == ((uint8_t *)old_str->bytes)[0]);
        }
        assert(aws_string_is_valid(str));
    } else {
        /* str was NULL – nothing to check */
    }

    if (str != NULL && old_allocator == NULL) {
        assert(aws_string_is_valid(str));
    }
}
