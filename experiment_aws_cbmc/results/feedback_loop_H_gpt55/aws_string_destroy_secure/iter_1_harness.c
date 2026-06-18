#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_mutable_for_destroy_secure_harness {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

void aws_string_destroy_secure_harness() {
    struct aws_string *str = NULL;
    struct aws_string_mutable_for_destroy_secure_harness *mutable_str = NULL;

    if (nondet_bool()) {
        mutable_str = malloc(sizeof(*mutable_str));
        __CPROVER_assume(mutable_str != NULL);

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        mutable_str->allocator = NULL;
        mutable_str->len = len;

        for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
            mutable_str->bytes[i] = nondet_uint8_t();
        }
        mutable_str->bytes[len] = 0;

        str = (struct aws_string *)mutable_str;

        __CPROVER_assume(aws_string_is_valid(str));
        __CPROVER_assume(AWS_MEM_IS_WRITABLE((void *)aws_string_bytes(str), str->len));
    }

    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    const uint8_t *old_bytes = NULL;
    uint8_t old_terminator = 0;

    if (str) {
        old_allocator = str->allocator;
        old_len = str->len;
        old_bytes = aws_string_bytes(str);
        old_terminator = old_bytes[old_len];
    }

    aws_string_destroy_secure(str);

    assert(str == old_str);

    if (old_str == NULL) {
        assert(str == NULL);
    } else {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert(aws_string_bytes(str) == old_bytes);

        for (size_t i = 0; i < old_len; ++i) {
            assert(old_bytes[i] == 0);
        }

        assert(old_bytes[old_len] == old_terminator);
        assert(old_bytes[old_len] == 0);
        assert(aws_string_is_valid(str));
    }
}
