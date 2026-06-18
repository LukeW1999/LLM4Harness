#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        const size_t max_len = 16;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        uint8_t buf[16];
        /* The contents of buf are irrelevant for destroy; they can remain nondet. */
        str = aws_string_new_from_array(aws_default_allocator(), buf, len);
        __CPROVER_assume(str != NULL);
    }

    struct aws_string *old_str = str;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_len = (str != NULL) ? str->len : 0;

    aws_string_destroy(str);

    assert(str == old_str);

    if (str == NULL || old_allocator == NULL) {
        if (str != NULL) {
            assert(str->allocator == old_allocator);
            assert(str->len == old_len);
        }
    } else {
        /* allocator was non‑NULL; memory may have been released. No further checks. */
    }
}
