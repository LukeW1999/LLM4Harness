#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_with_buffer {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

void aws_string_destroy_secure_harness(void) {
    struct aws_string_with_buffer *storage = NULL;
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        storage = malloc(sizeof(*storage));
        __CPROVER_assume(storage != NULL);

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        storage->allocator = nondet_bool() ? aws_default_allocator() : NULL;
        storage->len = len;

        for (size_t i = 0; i <= MAX_BUFFER_SIZE; ++i) {
            storage->bytes[i] = nondet_uint8_t();
        }
        storage->bytes[len] = 0;

        str = (struct aws_string *)storage;

        __CPROVER_assume(aws_string_is_valid(str));
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(aws_string_bytes(str), str->len));
    }

    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = str ? str->allocator : NULL;
    size_t old_len = str ? str->len : 0;
    const uint8_t *old_bytes = str ? aws_string_bytes(str) : NULL;

    aws_string_destroy_secure(str);

    assert(str == old_str);

    if (old_str == NULL) {
        assert(str == NULL);
    } else if (old_allocator == NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert(aws_string_bytes(str) == old_bytes);

        for (size_t i = 0; i < old_len; ++i) {
            assert(old_bytes[i] == 0);
        }

        assert(old_bytes[old_len] == 0);
        assert(aws_string_is_valid(str));

        free(storage);
    } else {
        assert(old_allocator == aws_default_allocator());
        assert(!AWS_MEM_IS_READABLE(old_str, sizeof(struct aws_string)));
    }
}
