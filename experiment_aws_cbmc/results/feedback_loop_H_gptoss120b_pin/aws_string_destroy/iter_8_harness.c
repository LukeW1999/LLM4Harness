#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_harness(void) {
    uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);

        str = (struct aws_string *)storage;

        struct aws_allocator *allocator = aws_default_allocator();
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = allocator;
        }

        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
    } else {
        str = NULL;
    }

    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_len = (str != NULL) ? str->len : 0;

    aws_string_destroy(str);

    /* The pointer value should not change */
    assert(str == old_str);

    if (old_str != NULL && old_allocator == NULL) {
        /* No allocator means the string memory was not freed; it should remain valid */
        assert(aws_string_is_valid(str));
        assert(str->len == old_len);
        if (old_len > 0) {
            assert(str->bytes[0] == old_str->bytes[0]);
        }
    }
}
