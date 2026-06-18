#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

static const uint8_t data_buf[256] = {0};

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len < 256);

        str = aws_string_new_from_array(aws_default_allocator(), data_buf, len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        if (nondet_bool()) {
            str->allocator = NULL;
        }
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
    }

    aws_string_destroy(str);

    assert(str == str);

    if (str != NULL && old_allocator != NULL) {
        /* object was freed; no further checks */
    } else {
        assert(aws_string_is_valid(str));
        if (str != NULL) {
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
        }
    }

    if (str != NULL && old_allocator == NULL) {
        assert(aws_string_is_valid(str));
    }
}
