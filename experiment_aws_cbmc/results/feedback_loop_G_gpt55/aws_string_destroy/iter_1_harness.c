#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_harness_static {
    struct aws_allocator *const allocator;
    const size_t len;
    const uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

void aws_string_destroy_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len < SIZE_MAX);

    uint8_t source[MAX_BUFFER_SIZE + 1];
    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        source[i] = nondet_uint8_t();
    }

    const struct aws_string_harness_static static_string = {NULL, len, {0}};

    struct aws_string *str = NULL;
    uint8_t choice = nondet_uint8_t();

    if (choice % 3 == 0) {
        str = NULL;
    } else if (choice % 3 == 1) {
        str = (struct aws_string *)&static_string;
    } else {
        str = aws_string_new_from_array(aws_default_allocator(), source, len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(str->allocator == aws_default_allocator());
    }

    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    uint8_t old_bytes[MAX_BUFFER_SIZE + 1];

    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(str->len < SIZE_MAX);

        old_allocator = str->allocator;
        old_len = str->len;

        for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
            if (i <= old_len) {
                old_bytes[i] = str->bytes[i];
            }
        }
    }

    aws_string_destroy(str);

    assert(str == old_str);

    if (old_str == NULL) {
        assert(str == NULL);
    } else if (old_allocator == NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert_bytes_match(str->bytes, old_bytes, old_len + 1);
        assert(str->bytes[old_len] == 0);
        assert(aws_string_is_valid(str));
    } else {
        assert(old_allocator == aws_default_allocator());
        assert(!AWS_MEM_IS_READABLE(str, sizeof(struct aws_string)));
    }
}
